# Madwewe

*Make Unreal respond to sound.*

Madwewe is an open-source Unreal Engine plugin project for mapping audio and MIDI to lights, materials, motion, particles, and gameplay events.

**Status: early implementation. MIDI input, a dockable monitor, synthetic test events, and a small light/motion demo rig now compile. Audio, general mappings, hardware validation, and a playable packaged demo are still pending.**

Initial development target: Windows, Unreal Engine 5.8.2, with a Maschine MK3 as the first hardware test controller. Other controllers should work through standard MIDI mappings; they will be listed as tested only after verification.

## MIDI input (first implementation)

In a Blueprint, use **Get Game Instance Subsystem → Madwewe Midi Subsystem**. Call **Refresh Devices**, choose an input port by its displayed name/ID, then call **Connect** with that ID. Bind **On Midi Event** to receive channel, raw type, normalized Note On/Off/Control Change kind, data bytes, and timestamp. **Get Recent Events** returns the most recent 128 events in arrival order. **Set Channel Filter** accepts 0 for all channels or 1–16 for one channel. Call **Disconnect** before ending a session; it also runs on game-instance teardown.

Refresh intentionally disconnects this subsystem's current port because Unreal reinitializes MIDI controllers during device enumeration. Connect uses the last refreshed list; refresh again after hardware changes. There is no automatic reconnect. Physical MK3 behavior is unverified.

### Try the editor monitor

1. Open `MadweweDemo.uproject` in Unreal and choose **Window → Tools → Madwewe MIDI Monitor**.
2. Start **Play in Editor**. The panel's controls are active only during Play.
3. Click **Spawn test rig**. It places a cube and point light in front of the Play camera.
4. Click **Pad 36 down** to raise the light, then **Pad 36 up** to restore it. Click **Test CC 21** to raise the cube. These events are labeled `Test input` in the monitor and do not require hardware.
5. For a real controller, put it in MIDI mode, click **Refresh ports**, choose its input port, then **Connect**. Press pads and turn knobs; the monitor shows raw numbers and values. The demo rig responds to Note 36 and CC 21 on channel 1 by default; the MK3's actual mapping may differ.

Stop Play to restore the test rig's starting state. The rig can also be placed in your own level as a `MadweweDemoRig` actor. The packaged host currently has no runtime device-selection UI or preplaced demo level, so packaging success is not yet an end-to-end playable-demo claim.

## Project documents

- [Project scope, architecture, and phased roadmap](docs/PROJECT-PLAN.md)
- [User experience and UI specification](docs/UX-UI-PLAN.md)
- [Testing, hardware validation, and bug fixing](docs/TEST-PLAN.md)
- [GitHub, public development, Codex models, and skills](docs/DELIVERY-PLAN.md)
- [Current status and next task](docs/STATUS.md)

## Build from source

Use Unreal Engine 5.8.2 and Visual Studio 2022 with C++ tools and the Windows SDK. From PowerShell in the repository root, run:

```powershell
$engineRoot = 'C:\Program Files\Epic Games\UE_5.8'
$projectPath = (Resolve-Path .\MadweweDemo.uproject).Path
& "$engineRoot\Engine\Build\BatchFiles\Build.bat" MadweweDemoEditor Win64 Development "-Project=$projectPath" -WaitMutex -NoHotReload
```

Adjust `$engineRoot` if Unreal is installed elsewhere. To build, cook and archive the Windows host project:

```powershell
$archivePath = Join-Path (Get-Location).Path 'Builds'
& "$engineRoot\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun "-project=$projectPath" -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive "-archivedirectory=$archivePath"
```

The engine location reflects the initial development PC. Build outputs under `Builds/` are ignored by Git. See [contributing](CONTRIBUTING.md) before opening a change.

The name comes from the Ojibwe word [madwewe](https://ojibwe.lib.umn.edu/main-entry/madwewe-vii), meaning that something makes noise, resounds, or reverberates. Pronunciation and appropriate attribution remain to be confirmed with a knowledgeable speaker. No community affiliation or endorsement is claimed. Trademark and domain clearance for Madwewe have not been completed.

Original Madwewe code is licensed under [MIT](LICENSE). Unreal Engine and third-party assets retain their own licenses; this repository does not grant rights to those materials.
