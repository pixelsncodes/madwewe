#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MadweweMidiSubsystem.generated.h"

class UMIDIDeviceInputController;

UENUM(BlueprintType)
enum class EMadweweMidiKind : uint8
{
    NoteOff,
    NoteOn,
    ControlChange,
    Other
};

USTRUCT(BlueprintType)
struct MADWEWERUNTIME_API FMadweweMidiEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    FString DeviceName;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    int32 Channel = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    EMadweweMidiKind Kind = EMadweweMidiKind::Other;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    int32 Data1 = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    int32 Data2 = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    int32 Timestamp = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    int32 RawType = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    bool bSynthetic = false;
};

USTRUCT(BlueprintType)
struct MADWEWERUNTIME_API FMadweweMidiDevice
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    int32 Id = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Madwewe|MIDI")
    bool bInUse = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMadweweMidiEventDelegate, const FMadweweMidiEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMadweweMidiConnectionDelegate, bool, bConnected);

/** Per-game-instance MIDI input and bounded diagnostics. Device selection is explicit. */
UCLASS(BlueprintType)
class MADWEWERUNTIME_API UMadweweMidiSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Madwewe|MIDI")
    FMadweweMidiEventDelegate OnMidiEvent;

    UPROPERTY(BlueprintAssignable, Category = "Madwewe|MIDI")
    FMadweweMidiConnectionDelegate OnConnectionChanged;

    UFUNCTION(BlueprintCallable, Category = "Madwewe|MIDI")
    TArray<FMadweweMidiDevice> RefreshDevices();

    UFUNCTION(BlueprintCallable, Category = "Madwewe|MIDI")
    bool Connect(int32 DeviceId);

    UFUNCTION(BlueprintCallable, Category = "Madwewe|MIDI")
    void Disconnect();

    UFUNCTION(BlueprintPure, Category = "Madwewe|MIDI")
    bool IsConnected() const;

    UFUNCTION(BlueprintPure, Category = "Madwewe|MIDI")
    FString GetConnectedDeviceName() const { return ConnectedDeviceName; }

    UFUNCTION(BlueprintPure, Category = "Madwewe|MIDI")
    int32 GetConnectedDeviceId() const { return ConnectedDeviceId; }

    /** 0 accepts all channels; otherwise accept only 1-16. */
    UFUNCTION(BlueprintCallable, Category = "Madwewe|MIDI")
    void SetChannelFilter(int32 Channel);

    UFUNCTION(BlueprintPure, Category = "Madwewe|MIDI")
    int32 GetChannelFilter() const { return ChannelFilter; }

    /** Oldest first; at most 128 events. */
    UFUNCTION(BlueprintPure, Category = "Madwewe|MIDI")
    TArray<FMadweweMidiEvent> GetRecentEvents() const { return RecentEvents; }

    UFUNCTION(BlueprintCallable, Category = "Madwewe|MIDI")
    void ClearRecentEvents() { RecentEvents.Reset(); }

    /** Emit a deterministic Note On/Off or CC event without a MIDI device. */
    UFUNCTION(BlueprintCallable, Category = "Madwewe|MIDI|Testing")
    bool EmitTestEvent(EMadweweMidiKind Kind, int32 Channel, int32 Number, int32 Value);

    virtual void Deinitialize() override;

    /** Shared normalization used by hardware events and deterministic tests. */
    static EMadweweMidiKind NormalizeType(int32 RawType, int32 Velocity);

private:
    void HandleRawEvent(UMIDIDeviceInputController* Source, int32 Timestamp, int32 RawType,
        int32 Channel, int32 Data1, int32 Data2);
    void RecordEvent(const FString& DeviceName, int32 Timestamp, int32 RawType,
        int32 Channel, int32 Data1, int32 Data2, bool bSynthetic);

    UPROPERTY(Transient)
    TObjectPtr<UMIDIDeviceInputController> Controller;

    FString ConnectedDeviceName;
    int32 ConnectedDeviceId = INDEX_NONE;
    TArray<FMadweweMidiDevice> AvailableDevices;
    int32 ChannelFilter = 0;
    TArray<FMadweweMidiEvent> RecentEvents;
    static constexpr int32 MaxRecentEvents = 128;
};
