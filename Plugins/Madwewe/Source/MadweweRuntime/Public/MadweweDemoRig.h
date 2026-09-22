#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MadweweDemoRig.generated.h"

class UPointLightComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UMadweweMidiDemoBinding;

/** Place in a level for the first MIDI light/motion demonstration. */
UCLASS()
class MADWEWERUNTIME_API AMadweweDemoRig : public AActor
{
    GENERATED_BODY()

public:
    AMadweweDemoRig();

    /** Change the demo's pad target after observing a real MIDI Note On. */
    bool UsePad(int32 InChannel, int32 InNote);
    bool UseHeightCC(int32 InChannel, int32 InCC);
    int32 GetPadNote() const;
    int32 GetPadChannel() const;
    int32 GetHeightCC() const;
    int32 GetHeightChannel() const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<UPointLightComponent> Light;

    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<UCameraComponent> PreviewCamera;

    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<UMadweweMidiDemoBinding> MidiBinding;
};
