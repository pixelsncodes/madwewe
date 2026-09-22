#include "MadweweDemoRig.h"

#include "MadweweMidiDemoBinding.h"
#include "Camera/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AMadweweDemoRig::AMadweweDemoRig()
{
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneRoot;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReactiveMesh"));
    Mesh->SetupAttachment(SceneRoot);
    Mesh->SetRelativeScale3D(FVector(2.0f));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded())
    {
        Mesh->SetStaticMesh(Cube.Object);
    }

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("ReactiveLight"));
    Light->SetupAttachment(SceneRoot);
    // Light the face seen by the preview camera, not just the cube's top.
    Light->SetRelativeLocation(FVector(-150.0f, 0.0f, 125.0f));
    Light->SetIntensity(2000.0f);
    Light->SetAttenuationRadius(700.0f);

    PreviewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PreviewCamera"));
    PreviewCamera->SetupAttachment(SceneRoot);
    PreviewCamera->SetRelativeLocation(FVector(-350.0f, 0.0f, 120.0f));
    PreviewCamera->SetRelativeRotation(FRotator(-18.0f, 0.0f, 0.0f));

    MidiBinding = CreateDefaultSubobject<UMadweweMidiDemoBinding>(TEXT("MidiBinding"));
}

bool AMadweweDemoRig::UsePad(int32 InChannel, int32 InNote)
{
    return MidiBinding && MidiBinding->UsePad(InChannel, InNote);
}

int32 AMadweweDemoRig::GetPadNote() const
{
    return MidiBinding ? MidiBinding->PadNote : 36;
}

int32 AMadweweDemoRig::GetPadChannel() const
{
    return MidiBinding ? MidiBinding->Channel : 1;
}
