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
    Controller->OnMIDIRawEvent.AddUObject(this, &UMadweweMidiSubsystem::HandleRawEvent);
    return true;
}

void UMadweweMidiSubsystem::Disconnect()
{
    if (Controller)
    {
        Controller->OnMIDIRawEvent.RemoveAll(this);
        Controller->ShutdownDevice();
        Controller = nullptr;
    }
    ConnectedDeviceName.Reset();
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
    if (Source != Controller || (ChannelFilter != 0 && ChannelFilter != Channel))
    {
        return;
    }
    FMadweweMidiEvent Event;
    Event.DeviceName = ConnectedDeviceName;
    Event.Timestamp = Timestamp;
    Event.RawType = RawType;
    Event.Channel = Channel;
    Event.Data1 = Data1;
    Event.Data2 = Data2;
    Event.Kind = NormalizeType(RawType, Data2);
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
