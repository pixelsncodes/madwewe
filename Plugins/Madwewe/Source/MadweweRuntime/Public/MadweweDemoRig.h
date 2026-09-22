#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MadweweDemoRig.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;
class UMadweweMidiDemoBinding;

/** Place in a level for the first MIDI light/motion demonstration. */
UCLASS()
class MADWEWERUNTIME_API AMadweweDemoRig : public AActor
{
    GENERATED_BODY()

public:
    AMadweweDemoRig();

private:
    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<UPointLightComponent> Light;

    UPROPERTY(VisibleAnywhere, Category = "Madwewe|Demo")
    TObjectPtr<UMadweweMidiDemoBinding> MidiBinding;
};
