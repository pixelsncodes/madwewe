#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MadweweMidiSubsystem.h"
#include "MadweweMidiDemoBinding.generated.h"

class ULightComponent;
class UStaticMeshComponent;

/** Small reversible vertical slice, not the general mapping/preset system. */
UCLASS(ClassGroup = (Madwewe), meta = (BlueprintSpawnableComponent))
class MADWEWERUNTIME_API UMadweweMidiDemoBinding : public UActorComponent
{
    GENERATED_BODY()

public:
    UMadweweMidiDemoBinding();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Madwewe|Demo|MIDI", meta = (ClampMin = "1", ClampMax = "16"))
    int32 Channel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Madwewe|Demo|MIDI", meta = (ClampMin = "0", ClampMax = "127"))
    int32 PadNote = 36;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Madwewe|Demo|MIDI", meta = (ClampMin = "0", ClampMax = "127"))
    int32 HeightCC = 21;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Madwewe|Demo|Response", meta = (ClampMin = "0"))
    float LightBoost = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Madwewe|Demo|Response")
    float HeightRange = 200.0f;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    static float MidiUnit(int32 Value);

private:
    UFUNCTION()
    void HandleMidiEvent(const FMadweweMidiEvent& Event);

    UFUNCTION()
    void HandleConnectionChanged(bool bConnected);

    void RestoreTargets();

    UPROPERTY(Transient)
    TObjectPtr<ULightComponent> Light;

    UPROPERTY(Transient)
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(Transient)
    TObjectPtr<UMadweweMidiSubsystem> Midi;

    float InitialIntensity = 0.0f;
    FVector InitialLocation = FVector::ZeroVector;
};
