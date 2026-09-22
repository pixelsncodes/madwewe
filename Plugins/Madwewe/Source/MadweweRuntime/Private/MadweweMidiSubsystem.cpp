#include "MadweweMidiSubsystem.h"

#include "MIDIDeviceInputController.h"
#include "MIDIDeviceManager.h"

TArray<FMadweweMidiDevice> UMadweweMidiSubsystem::RefreshDevices()
{
    // Unreal reinitializes every MIDI controller when enumerating. Close ours first,
    // so refresh never silently replaces a live connection or leaves held state.
    Disconnect();
    TArray<FFoundMIDIDevice> Found;
    UMIDIDeviceManager::FindMIDIDevices(Found);
    TArray<FMadweweMidiDevice> Result;
    for (const FFoundMIDIDevice& Device : Found)
    {
        if (Device.bCanReceiveFrom)
        {
            FMadweweMidiDevice Entry;
            Entry.Id = Device.DeviceID;
            Entry.Name = Device.DeviceName;
            Entry.bInUse = Device.bIsAlreadyInUse;
            Result.Add(MoveTemp(Entry));
        }
    }
    AvailableDevices = Result;
    return Result;
}

bool UMadweweMidiSubsystem::Connect(int32 DeviceId)
{
    Disconnect();
    const FMadweweMidiDevice* Device = AvailableDevices.FindByPredicate([DeviceId](const FMadweweMidiDevice& Candidate)
    {
        return Candidate.Id == DeviceId;
    });
    if (!Device || Device->bInUse)
    {
        return false;
    }
    UMIDIDeviceInputController* NewController = UMIDIDeviceManager::CreateMIDIDeviceInputController(DeviceId, 1024);
    if (!NewController)
    {
        return false;
    }
    Controller = NewController;
    ConnectedDeviceName = Device->Name;
    ConnectedDeviceId = DeviceId;
    Controller->OnMIDIRawEvent.AddUObject(this, &UMadweweMidiSubsystem::HandleRawEvent);
    OnConnectionChanged.Broadcast(true);
    return true;
}

void UMadweweMidiSubsystem::Disconnect()
{
    const bool bWasConnected = Controller != nullptr;
    if (Controller)
    {
        Controller->OnMIDIRawEvent.RemoveAll(this);
        Controller->ShutdownDevice();
        Controller = nullptr;
    }
    ConnectedDeviceName.Reset();
    ConnectedDeviceId = INDEX_NONE;
    if (bWasConnected)
    {
        OnConnectionChanged.Broadcast(false);
    }
}

bool UMadweweMidiSubsystem::IsConnected() const
{
    return IsValid(Controller);
}

void UMadweweMidiSubsystem::SetChannelFilter(int32 Channel)
{
    ChannelFilter = FMath::Clamp(Channel, 0, 16);
}

EMadweweMidiKind UMadweweMidiSubsystem::NormalizeType(int32 RawType, int32 Velocity)
{
    if (RawType == 8 || (RawType == 9 && Velocity == 0))
    {
        return EMadweweMidiKind::NoteOff;
    }
    if (RawType == 9)
    {
        return EMadweweMidiKind::NoteOn;
    }
    return RawType == 11 ? EMadweweMidiKind::ControlChange : EMadweweMidiKind::Other;
}

void UMadweweMidiSubsystem::HandleRawEvent(UMIDIDeviceInputController* Source, int32 Timestamp,
    int32 RawType, int32 Channel, int32 Data1, int32 Data2)
{
    if (Source != Controller)
    {
        return;
    }
    RecordEvent(ConnectedDeviceName, Timestamp, RawType, Channel, Data1, Data2, false);
}

bool UMadweweMidiSubsystem::EmitTestEvent(EMadweweMidiKind Kind, int32 Channel, int32 Number, int32 Value)
{
    if (Channel < 1 || Channel > 16 || Number < 0 || Number > 127 || Value < 0 || Value > 127
        || Kind == EMadweweMidiKind::Other)
    {
        return false;
    }
    const int32 RawType = Kind == EMadweweMidiKind::NoteOn ? 9
        : Kind == EMadweweMidiKind::NoteOff ? 8 : 11;
    RecordEvent(TEXT("Test input"), 0, RawType, Channel, Number, Value, true);
    return true;
}

void UMadweweMidiSubsystem::RecordEvent(const FString& DeviceName, int32 Timestamp,
    int32 RawType, int32 Channel, int32 Data1, int32 Data2, bool bSynthetic)
{
    if (ChannelFilter != 0 && ChannelFilter != Channel)
    {
        return;
    }
    FMadweweMidiEvent Event;
    Event.DeviceName = DeviceName;
    Event.Timestamp = Timestamp;
    Event.RawType = RawType;
    Event.Channel = Channel;
    Event.Data1 = Data1;
    Event.Data2 = Data2;
    Event.Kind = NormalizeType(RawType, Data2);
    Event.bSynthetic = bSynthetic;
    if (RecentEvents.Num() == MaxRecentEvents)
    {
        RecentEvents.RemoveAt(0, 1, EAllowShrinking::No);
    }
    RecentEvents.Add(Event);
    OnMidiEvent.Broadcast(Event);
}

void UMadweweMidiSubsystem::Deinitialize()
{
    Disconnect();
    RecentEvents.Reset();
    Super::Deinitialize();
}
