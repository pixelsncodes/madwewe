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
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("SpawnRig", "Spawn test rig"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::SpawnRig) ]
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
                    [ SNew(SButton).Text(LOCTEXT("TestPadDown", "Pad 36 down"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::TestPadDown) ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)
                    [ SNew(SButton).Text(LOCTEXT("TestPadUp", "Pad 36 up"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::TestPadUp) ]
                    + SHorizontalBox::Slot().AutoWidth()
                    [ SNew(SButton).Text(LOCTEXT("TestKnob", "Test CC 21"))
                        .IsEnabled(this, &SMadweweMidiMonitor::HasPlayWorld)
                        .OnClicked(this, &SMadweweMidiMonitor::TestKnob) ]
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                [ SNew(STextBlock).Text(LOCTEXT("Columns", "Time  |  Port  |  Channel  |  Event  |  Number  |  Value")) ]
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
                ? TEXT("Connected. Press a pad or turn a control.")
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
            Service->EmitTestEvent(EMadweweMidiKind::NoteOn, 1, 36, 100);
        }
        return FReply::Handled();
    }

    FReply TestPadUp()
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            Service->EmitTestEvent(EMadweweMidiKind::NoteOff, 1, 36, 0);
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
            Message = TEXT("Test rig is already in this Play session.");
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
        Message = TestRig.IsValid() ? TEXT("Test rig spawned in front of the camera.")
            : TEXT("Could not spawn test rig in this Play world.");
        return FReply::Handled();
    }

    FReply TestKnob()
    {
        if (UMadweweMidiSubsystem* Service = GetService())
        {
            Service->EmitTestEvent(EMadweweMidiKind::ControlChange, 1, 21, 96);
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

    FText GetStatus() const
    {
        if (const UMadweweMidiSubsystem* Service = GetService())
        {
            if (Service->IsConnected())
            {
                return FText::FromString(FString::Printf(TEXT("Connected: %s"), *Service->GetConnectedDeviceName()));
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
                : Event.Kind == EMadweweMidiKind::ControlChange ? TEXT("CC") : TEXT("Other");
            Lines += FString::Printf(TEXT("%d  |  %s  |  %d  |  %s  |  %d  |  %d\n"),
                Event.Timestamp, *Event.DeviceName, Event.Channel, Kind, Event.Data1, Event.Data2);
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
