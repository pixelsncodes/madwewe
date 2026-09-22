#include "MadweweMidiDemoBinding.h"

#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Actor.h"

UMadweweMidiDemoBinding::UMadweweMidiDemoBinding()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMadweweMidiDemoBinding::BeginPlay()
{
    Super::BeginPlay();
    Light = GetOwner()->FindComponentByClass<ULightComponent>();
    Mesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
    if (Light)
    {
        InitialIntensity = Light->Intensity;
    }
    if (Mesh)
    {
        InitialLocation = Mesh->GetRelativeLocation();
    }
    if (UGameInstance* Game = GetWorld()->GetGameInstance())
    {
        Midi = Game->GetSubsystem<UMadweweMidiSubsystem>();
        if (Midi)
        {
            Midi->OnMidiEvent.AddDynamic(this, &UMadweweMidiDemoBinding::HandleMidiEvent);
            Midi->OnConnectionChanged.AddDynamic(this, &UMadweweMidiDemoBinding::HandleConnectionChanged);
        }
    }
}

void UMadweweMidiDemoBinding::EndPlay(const EEndPlayReason::Type Reason)
{
    if (Midi)
    {
        Midi->OnMidiEvent.RemoveDynamic(this, &UMadweweMidiDemoBinding::HandleMidiEvent);
        Midi->OnConnectionChanged.RemoveDynamic(this, &UMadweweMidiDemoBinding::HandleConnectionChanged);
        Midi = nullptr;
    }
    RestoreTargets();
    Super::EndPlay(Reason);
}

float UMadweweMidiDemoBinding::MidiUnit(int32 Value)
{
    return FMath::Clamp(Value, 0, 127) / 127.0f;
}

void UMadweweMidiDemoBinding::HandleMidiEvent(const FMadweweMidiEvent& Event)
{
    if (Event.Channel != Channel)
    {
        return;
    }
    if (Light && Event.Data1 == PadNote)
    {
        if (Event.Kind == EMadweweMidiKind::NoteOn)
        {
            Light->SetIntensity(InitialIntensity + FMath::Max(0.0f, LightBoost) * MidiUnit(Event.Data2));
        }
        else if (Event.Kind == EMadweweMidiKind::NoteOff)
        {
            Light->SetIntensity(InitialIntensity);
        }
    }
    if (Mesh && Event.Kind == EMadweweMidiKind::ControlChange && Event.Data1 == HeightCC)
    {
        Mesh->SetRelativeLocation(InitialLocation + FVector(0.0f, 0.0f, HeightRange * MidiUnit(Event.Data2)));
    }
}

void UMadweweMidiDemoBinding::HandleConnectionChanged(bool bConnected)
{
    if (!bConnected)
    {
        RestoreTargets();
    }
}

void UMadweweMidiDemoBinding::RestoreTargets()
{
    if (IsValid(Light))
    {
        Light->SetIntensity(InitialIntensity);
    }
    if (IsValid(Mesh))
    {
        Mesh->SetRelativeLocation(InitialLocation);
    }
}
