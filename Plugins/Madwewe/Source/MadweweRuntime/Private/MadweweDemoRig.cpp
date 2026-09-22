#include "MadweweDemoRig.h"

#include "MadweweMidiDemoBinding.h"
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
    Mesh->SetRelativeScale3D(FVector(1.0f));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded())
    {
        Mesh->SetStaticMesh(Cube.Object);
    }

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("ReactiveLight"));
    Light->SetupAttachment(SceneRoot);
    Light->SetRelativeLocation(FVector(0.0f, 0.0f, 175.0f));
    Light->SetIntensity(1000.0f);
    Light->SetAttenuationRadius(700.0f);

    MidiBinding = CreateDefaultSubobject<UMadweweMidiDemoBinding>(TEXT("MidiBinding"));
}
