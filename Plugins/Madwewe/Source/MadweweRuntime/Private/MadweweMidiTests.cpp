#if WITH_DEV_AUTOMATION_TESTS

#include "MadweweMidiSubsystem.h"
#include "MadweweMidiDemoBinding.h"
#include "MadweweDemoRig.h"
#include "Camera/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMadweweMidiNormalizationTest,
    "Madwewe.MIDI.NormalizeType",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMadweweMidiNormalizationTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Note On"), UMadweweMidiSubsystem::NormalizeType(9, 127), EMadweweMidiKind::NoteOn);
    TestEqual(TEXT("Zero-velocity Note On releases"), UMadweweMidiSubsystem::NormalizeType(9, 0), EMadweweMidiKind::NoteOff);
    TestEqual(TEXT("Note Off"), UMadweweMidiSubsystem::NormalizeType(8, 64), EMadweweMidiKind::NoteOff);
    TestEqual(TEXT("Control Change"), UMadweweMidiSubsystem::NormalizeType(11, 0), EMadweweMidiKind::ControlChange);
    TestEqual(TEXT("Other event"), UMadweweMidiSubsystem::NormalizeType(14, 1), EMadweweMidiKind::Other);
    UGameInstance* Game = NewObject<UGameInstance>();
    UMadweweMidiSubsystem* Service = NewObject<UMadweweMidiSubsystem>(Game);
    TestEqual(TEXT("No port connected initially"), Service->GetConnectedDeviceId(), INDEX_NONE);
    TestFalse(TEXT("Cannot connect an unlisted port"), Service->Connect(999));
    TestEqual(TEXT("Failed connection leaves no active port"), Service->GetConnectedDeviceId(), INDEX_NONE);
    TestFalse(TEXT("Reject invalid channel"), Service->EmitTestEvent(EMadweweMidiKind::NoteOn, 17, 36, 100));
    TestFalse(TEXT("Reject invalid value"), Service->EmitTestEvent(EMadweweMidiKind::ControlChange, 1, 21, 128));
    Service->SetChannelFilter(2);
    TestTrue(TEXT("Valid filtered test event accepted"), Service->EmitTestEvent(EMadweweMidiKind::NoteOn, 1, 36, 100));
    TestEqual(TEXT("Other channel not recorded"), Service->GetRecentEvents().Num(), 0);
    Service->SetChannelFilter(0);
    for (int32 Index = 0; Index < 130; ++Index)
    {
        Service->EmitTestEvent(EMadweweMidiKind::ControlChange, 1, 21, Index % 128);
    }
    const TArray<FMadweweMidiEvent> Events = Service->GetRecentEvents();
    TestEqual(TEXT("History is bounded"), Events.Num(), 128);
    TestTrue(TEXT("Synthetic events identified"), Events.Last().bSynthetic);
    TestEqual(TEXT("Oldest events evicted"), Events[0].Data2, 2);
    TestEqual(TEXT("MIDI minimum"), UMadweweMidiDemoBinding::MidiUnit(0), 0.0f);
    TestEqual(TEXT("MIDI maximum"), UMadweweMidiDemoBinding::MidiUnit(127), 1.0f);
    TestEqual(TEXT("Below range clamps"), UMadweweMidiDemoBinding::MidiUnit(-1), 0.0f);
    TestEqual(TEXT("Above range clamps"), UMadweweMidiDemoBinding::MidiUnit(128), 1.0f);
    const AMadweweDemoRig* RigDefaults = GetDefault<AMadweweDemoRig>();
    const UCameraComponent* Camera = RigDefaults->FindComponentByClass<UCameraComponent>();
    const UPointLightComponent* Light = RigDefaults->FindComponentByClass<UPointLightComponent>();
    const UStaticMeshComponent* Mesh = RigDefaults->FindComponentByClass<UStaticMeshComponent>();
    TestNotNull(TEXT("Demo rig has a camera"), Camera);
    TestNotNull(TEXT("Demo rig has a light"), Light);
    TestNotNull(TEXT("Demo rig has a mesh"), Mesh);
    if (Camera && Light && Mesh)
    {
        TestTrue(TEXT("Camera faces the lit side"), Camera->GetRelativeLocation().X < 0.0f && Light->GetRelativeLocation().X < 0.0f);
        TestNotNull(TEXT("Demo mesh asset loaded"), Mesh->GetStaticMesh().Get());
    }
    return true;
}

#endif
