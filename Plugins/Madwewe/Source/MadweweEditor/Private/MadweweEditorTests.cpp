#if WITH_DEV_AUTOMATION_TESTS

#include "Framework/Docking/TabManager.h"
#include "Misc/AutomationTest.h"
#include "Widgets/Docking/SDockTab.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMadweweMonitorOpensTest,
    "Madwewe.Editor.MidiMonitorOpens",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMadweweMonitorOpensTest::RunTest(const FString& Parameters)
{
    const TSharedPtr<SDockTab> Tab = FGlobalTabmanager::Get()->TryInvokeTab(FName(TEXT("MadweweMidiMonitor")));
    TestTrue(TEXT("Monitor tab opens"), Tab.IsValid());
    if (Tab.IsValid())
    {
        Tab->RequestCloseTab();
    }
    return true;
}

#endif
