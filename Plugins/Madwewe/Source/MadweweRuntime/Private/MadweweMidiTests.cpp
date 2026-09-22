#if WITH_DEV_AUTOMATION_TESTS

#include "MadweweMidiSubsystem.h"
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
    return true;
}

#endif
