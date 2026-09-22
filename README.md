# Madwewe

*Make Unreal respond to sound.*

Madwewe is an open-source Unreal Engine plugin project for mapping audio and MIDI to lights, materials, motion, particles, and gameplay events.

**Status: early implementation. A Blueprint-accessible MIDI input service now compiles and its event-normalization test passes. Audio, mappings, editor UI, hardware validation, and a playable demo are still pending.**

Initial development target: Windows, Unreal Engine 5.8.2, with a Maschine MK3 as the first hardware test controller. Other controllers should work through standard MIDI mappings; they will be listed as tested only after verification.

## MIDI input (first implementation)

In a Blueprint, use **Get Game Instance Subsystem → Madwewe Midi Subsystem**. Call **Refresh Devices**, choose an input port by its displayed name/ID, then call **Connect** with that ID. Bind **On Midi Event** to receive channel, raw type, normalized Note On/Off/Control Change kind, data bytes, and timestamp. **Get Recent Events** returns the most recent 128 events in arrival order. **Set Channel Filter** accepts 0 for all channels or 1–16 for one channel. Call **Disconnect** before ending a session; it also runs on game-instance teardown.

Refresh intentionally disconnects this subsystem's current port because Unreal reinitializes MIDI controllers during device enumeration. Connect uses the last refreshed list; refresh again after hardware changes. There is no automatic reconnect or visible monitor panel yet. Physical MK3 behavior is unverified.

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
