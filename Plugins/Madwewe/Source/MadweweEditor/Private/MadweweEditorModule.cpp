#include "MadweweMidiSubsystem.h"
#include "MadweweDemoRig.h"

#include "Editor.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Docking/TabManager.h"
#include "Modules/ModuleManager.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MadweweMidiMonitor"

namespace
{
const FName MonitorTabId(TEXT("MadweweMidiMonitor"));

class SMadweweMidiMonitor final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMadweweMidiMonitor) {}
    SLATE_END_ARGS()

    void Construct(const FArguments&)
    {
        ChildSlot
        [
            SNew(SBorder)
            .Padding(12)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [ SNew(STextBlock).Text(LOCTEXT("Title", "MIDI input monitor")) ]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [ SNew(STextBlock).Text(this, &SMadweweMidiMonitor::GetStatus).AutoWrapText(true) ]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [ SNew(STextBlock).Text(this, &SMadweweMidiMonitor::GetMappingSummary).AutoWrapText(true) ]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("SpawnRig", "Spawn test rig"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::SpawnRig) ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("UseLastPad", "Use last pad"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::UseLastPad) ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("UseLastCC", "Use last CC"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::UseLastCC) ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("Refresh", "Refresh ports"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::Refresh) ]
                    + SHorizontalBox::Slot().FillWidth(1)
                    [
                        SAssignNew(DevicePicker, SComboBox<TSharedPtr<FMadweweMidiDevice>>)
                        .OptionsSource(&Devices)
                        .OnGenerateWidget(this, &SMadweweMidiMonitor::MakeDeviceOption)
                        .OnSelectionChanged(this, &SMadweweMidiMonitor::SelectDevice)
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        [ SNew(STextBlock).Text(this, &SMadweweMidiMonitor::GetSelectedDeviceText) ]
                    ]
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("Connect", "Connect"))
                        .IsEnabled(this, &SMadweweMidiMonitor::CanConnect)
                        .OnClicked(this, &SMadweweMidiMonitor::Connect) ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("Disconnect", "Disconnect"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::Disconnect) ]
                    + SHorizontalBox::Slot().AutoWidth()
                    [ SNew(SButton).Text(LOCTEXT("Clear", "Clear events"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::Clear) ]
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(this, &SMadweweMidiMonitor::GetTestPadDownLabel)
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::TestPadDown) ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(this, &SMadweweMidiMonitor::GetTestPadUpLabel)
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::TestPadUp) ]
                    + SHorizontalBox::Slot().AutoWidth()
                    [ SNew(SButton).Text(this, &SMadweweMidiMonitor::GetTestCCLowLabel)
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::TestCCLow) ]
                    + SHorizontalBox::Slot().AutoWidth()
                    [ SNew(SButton).Text(this, &SMadweweMidiMonitor::GetTestCCHighLabel)
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::TestCCHigh) ]
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [ SNew(STextBlock).Text(LOCTEXT("Columns", "Time  |  Port  |  Channel  |  Event  |  Raw  |  Number  |  Value")) ]
                + SVerticalBox::Slot().FillHeight(1)
                [ SNew(SScrollBox)
                    + SScrollBox::Slot()
                    [ SNew(STextBlock).Text(this, &SMadweweMidiMonitor::GetEvents).AutoWrapText(true) ] ]
            ]
        ];
    }

private:
    UMadweweMidiSubsystem* GetService() const
    {
        UWorld* World = GEditor ? GEditor->PlayWorld : nullptr;
        UGameInstance* Game = World ? World->GetGameInstance() : nullptr;
        return Game ? Game->GetSubsystem<UMadweweMidiSubsystem>() : nullptr;
    }

    bool HasPlayWorld() const { return GetService() != nullptr; }
    bool CanConnect() const { return HasPlayWorld() && SelectedDevice.IsValid(); }

    FReply Refresh()
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            SelectedDevice.Reset();
            DevicePicker->ClearSelection();
            Devices.Reset();
            for (const FMadweweMidiDevice& Device : Service->RefreshDevices())
            {
                Devices.Add(MakeShared<FMadweweMidiDevice>(Device));
            }
            DevicePicker->RefreshOptions();
            Message = Devices.IsEmpty() ? TEXT("No MIDI input ports found.") : TEXT("Select an input port.");
        }
        return FReply::Handled();
    }

    FReply Connect()
    {
        if (UMadweweMidiSubsystem* Service = GetService(); Service && SelectedDevice.IsValid())
        {
            Message = Service->Connect(SelectedDevice->Id)
                ? TEXT("Press a pad or turn a control.")
                : TEXT("Could not open this port. It may be in use or disconnected; refresh and try again.");
        }
        return FReply::Handled();
    }

    FReply Disconnect()
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            Service->Disconnect();
            Message = TEXT("Disconnected.");
        }
        return FReply::Handled();
    }

    FReply Clear()
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            Service->ClearRecentEvents();
        }
        return FReply::Handled();
    }

    FReply TestPadDown()
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            Service->EmitTestEvent(EMadweweMidiKind::NoteOn,
                TestRig.IsValid() ? TestRig->GetPadChannel() : 1,
                TestRig.IsValid() ? TestRig->GetPadNote() : 36, 100);
        }
        return FReply::Handled();
    }

    FReply TestPadUp()
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            Service->EmitTestEvent(EMadweweMidiKind::NoteOff,
                TestRig.IsValid() ? TestRig->GetPadChannel() : 1,
                TestRig.IsValid() ? TestRig->GetPadNote() : 36, 0);
        }
        return FReply::Handled();
    }

    FReply SpawnRig()
    {
        UWorld* World = GEditor ? GEditor->PlayWorld : nullptr;
        if (!World)
        {
            return FReply::Handled();
        }
        if (TestRig.IsValid())
        {
            if (APlayerController* Player = World->GetFirstPlayerController())
            {
                Player->SetViewTarget(TestRig.Get());
            }
            Message = TEXT("Framed the existing test rig.");
            return FReply::Handled();
        }
        FVector Location(350.0f, 0.0f, 100.0f);
        if (APlayerController* Player = World->GetFirstPlayerController())
        {
            FVector ViewLocation;
            FRotator ViewRotation;
            Player->GetPlayerViewPoint(ViewLocation, ViewRotation);
            Location = ViewLocation + ViewRotation.Vector() * 350.0f;
        }
        TestRig = World->SpawnActor<AMadweweDemoRig>(Location, FRotator::ZeroRotator);
        if (TestRig.IsValid())
        {
            if (APlayerController* Player = World->GetFirstPlayerController())
            {
                Player->SetViewTarget(TestRig.Get());
            }
        }
        Message = TestRig.IsValid() ? TEXT("Test rig framed. Press a pad, then use last pad to assign it.")
            : TEXT("Could not spawn test rig in this Play world.");
        return FReply::Handled();
    }

    FReply UseLastPad()
    {
        UMadweweMidiSubsystem* Service = GetService();
        if (!Service || !TestRig.IsValid())
        {
            Message = TEXT("Spawn the test rig first.");
            return FReply::Handled();
        }
        const TArray<FMadweweMidiEvent> Events = Service->GetRecentEvents();
        for (int32 Index = Events.Num() - 1; Index >= 0; --Index)
        {
            const FMadweweMidiEvent& Event = Events[Index];
            if (!Event.bSynthetic && Event.Kind == EMadweweMidiKind::NoteOn)
            {
                if (TestRig->UsePad(Event.Channel, Event.Data1))
                {
                    Message = FString::Printf(TEXT("Rig listens to note %d on channel %d."), Event.Data1, Event.Channel);
                }
                return FReply::Handled();
            }
        }
        Message = TEXT("No real pad press found yet. Press one, then try again.");
        return FReply::Handled();
    }

    FReply UseLastCC()
    {
        UMadweweMidiSubsystem* Service = GetService();
        if (!Service || !TestRig.IsValid())
        {
            Message = TEXT("Spawn the test rig first.");
            return FReply::Handled();
        }
        const TArray<FMadweweMidiEvent> Events = Service->GetRecentEvents();
        for (int32 Index = Events.Num() - 1; Index >= 0; --Index)
        {
            const FMadweweMidiEvent& Event = Events[Index];
            if (!Event.bSynthetic && Event.Kind == EMadweweMidiKind::ControlChange)
            {
                if (TestRig->UseHeightCC(Event.Channel, Event.Data1))
                {
                    Message = FString::Printf(TEXT("Cube height listens to CC %d on channel %d."), Event.Data1, Event.Channel);
                }
                return FReply::Handled();
            }
        }
        Message = TEXT("No real CC found yet. Turn a knob, then try again.");
        return FReply::Handled();
    }

    FReply TestCCLow()
    {
        return EmitTestCC(0);
    }

    FReply TestCCHigh()
    {
        return EmitTestCC(96);
    }

    FReply EmitTestCC(int32 Value)
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            Service->EmitTestEvent(EMadweweMidiKind::ControlChange,
                TestRig.IsValid() ? TestRig->GetHeightChannel() : 1,
                TestRig.IsValid() ? TestRig->GetHeightCC() : 21, Value);
        }
        return FReply::Handled();
    }

    void SelectDevice(TSharedPtr<FMadweweMidiDevice> Device, ESelectInfo::Type)
    {
        SelectedDevice = Device;
    }

    TSharedRef<SWidget> MakeDeviceOption(TSharedPtr<FMadweweMidiDevice> Device) const
    {
        return SNew(STextBlock).Text(FText::FromString(DescribeDevice(Device)));
    }

    static FString DescribeDevice(const TSharedPtr<FMadweweMidiDevice>& Device)
    {
        return Device.IsValid()
            ? FString::Printf(TEXT("%s (port %d)%s"), *Device->Name, Device->Id,
                Device->bInUse ? TEXT(" — in use") : TEXT(""))
            : TEXT("Select a MIDI input");
    }

    FText GetSelectedDeviceText() const { return FText::FromString(DescribeDevice(SelectedDevice)); }
    FText GetTestPadDownLabel() const
    {
        return FText::Format(LOCTEXT("PadDownFormat", "Pad {0} down"),
            FText::AsNumber(TestRig.IsValid() ? TestRig->GetPadNote() : 36));
    }
    FText GetTestPadUpLabel() const
    {
        return FText::Format(LOCTEXT("PadUpFormat", "Pad {0} up"),
            FText::AsNumber(TestRig.IsValid() ? TestRig->GetPadNote() : 36));
    }
    FText GetTestCCLowLabel() const
    {
        return FText::Format(LOCTEXT("CCLowFormat", "CC {0} low"),
            FText::AsNumber(TestRig.IsValid() ? TestRig->GetHeightCC() : 21));
    }
    FText GetTestCCHighLabel() const
    {
        return FText::Format(LOCTEXT("CCHighFormat", "CC {0} high"),
            FText::AsNumber(TestRig.IsValid() ? TestRig->GetHeightCC() : 21));
    }

    FText GetMappingSummary() const
    {
        if (!GetService())
        {
            return FText::GetEmpty();
        }
        if (!TestRig.IsValid())
        {
            return LOCTEXT("MappingBeforeRig", "Spawn test rig to see MIDI mappings. Test buttons create events; they do not toggle.");
        }
        return FText::FromString(FString::Printf(
            TEXT("Light: channel %d, note %d (down brightens; up resets). Cube height: channel %d, CC %d (low/high set absolute positions). Pad pressure is monitored but not mapped."),
            TestRig->GetPadChannel(), TestRig->GetPadNote(), TestRig->GetHeightChannel(), TestRig->GetHeightCC()));
    }

    FText GetStatus() const
    {
        if (const UMadweweMidiSubsystem* Service = GetService())
        {
            if (Service->IsConnected())
            {
                FString Connection = FString::Printf(TEXT("Connected: %s"), *Service->GetConnectedDeviceName());
                if (SelectedDevice.IsValid() && SelectedDevice->Id != Service->GetConnectedDeviceId())
                {
                    Connection += TEXT(" | Dropdown shows another port; click Connect to switch.");
                }
                return FText::FromString(Message.IsEmpty() ? Connection : Connection + TEXT(" | ") + Message);
            }
            return FText::FromString(Message.IsEmpty() ? TEXT("Refresh ports to begin.") : Message);
        }
        return LOCTEXT("PlayRequired", "Start Play in Editor, then refresh ports. Connections close when Play ends.");
    }

    FText GetEvents() const
    {
        const UMadweweMidiSubsystem* Service = GetService();
        if (!Service)
        {
            return FText::GetEmpty();
        }
        const TArray<FMadweweMidiEvent> Events = Service->GetRecentEvents();
        FString Lines;
        for (int32 Index = Events.Num() - 1; Index >= FMath::Max(0, Events.Num() - 32); --Index)
        {
            const FMadweweMidiEvent& Event = Events[Index];
            const TCHAR* Kind = Event.Kind == EMadweweMidiKind::NoteOn ? TEXT("Note On")
                : Event.Kind == EMadweweMidiKind::NoteOff ? TEXT("Note Off")
                : Event.Kind == EMadweweMidiKind::ControlChange ? TEXT("CC")
                : Event.RawType == 10 ? TEXT("Pad pressure") : TEXT("Other");
            Lines += FString::Printf(TEXT("%d  |  %s  |  %d  |  %s  |  %d  |  %d  |  %d\n"),
                Event.Timestamp, *Event.DeviceName, Event.Channel, Kind, Event.RawType, Event.Data1, Event.Data2);
        }
        return FText::FromString(Lines.IsEmpty() ? TEXT("No events yet.") : Lines);
    }

    TArray<TSharedPtr<FMadweweMidiDevice>> Devices;
    TSharedPtr<FMadweweMidiDevice> SelectedDevice;
    TSharedPtr<SComboBox<TSharedPtr<FMadweweMidiDevice>>> DevicePicker;
    FString Message;
    TWeakObjectPtr<AMadweweDemoRig> TestRig;
};
}

class FMadweweEditorModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MonitorTabId,
            FOnSpawnTab::CreateRaw(this, &FMadweweEditorModule::SpawnMonitorTab))
            .SetDisplayName(LOCTEXT("TabName", "Madwewe MIDI Monitor"))
            .SetTooltipText(LOCTEXT("TabTooltip", "Inspect MIDI input while playing in the editor."))
            .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
            .SetMenuType(ETabSpawnerMenuType::Enabled);
    }

    virtual void ShutdownModule() override
    {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MonitorTabId);
    }

private:
    TSharedRef<SDockTab> SpawnMonitorTab(const FSpawnTabArgs&)
    {
        return SNew(SDockTab).TabRole(ETabRole::NomadTab)
            [ SNew(SMadweweMidiMonitor) ];
    }
};

IMPLEMENT_MODULE(FMadweweEditorModule, MadweweEditor)

#undef LOCTEXT_NAMESPACE
