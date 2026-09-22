# Contributing to Madwewe

Madwewe is at the foundation stage. The documented plan is the current source of product scope; the plugin skeleton is not a working audio or MIDI tool yet.

## Before working on an issue

Read [the project plan](docs/PROJECT-PLAN.md), [UX plan](docs/UX-UI-PLAN.md), and [current status](docs/STATUS.md). If an issue changes the public API, saved data, or input-device ownership, describe the decision in the pull request before implementation.

Keep each contribution small enough to build and verify. Describe the user action it enables, the behavior expected in the editor and packaged game, and the evidence collected. Prefer original examples and assets with clear redistribution rights.

## Development baseline

- Unreal Engine 5.8.2 on Windows.
- Visual Studio 2022 with C++ tools.
- Open `MadweweDemo.uproject` or use Unreal's Windows build scripts.
- The repo holds a host project in `Source/` and the plugin in `Plugins/Madwewe/`.
- Generated `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, and packaged builds are ignored.

The verified compile/package commands are in the [README](README.md). Do not claim a feature is complete based only on an editor compile; runtime features need packaged validation.

## Pull requests

Include the linked issue, a short description of the behavior, screenshots or a short recording for visible UI changes, test commands/results, and hardware status where relevant. Mark tests as **not run** when they have not run. Do not include private logs, credentials, Engine source, or assets without permission to redistribute.

Report bugs with the engine version, commit, device/template where relevant, steps to reproduce, expected and actual behavior, and a narrow log excerpt. See [testing and bug fixing](docs/TEST-PLAN.md).

Community contributions are welcome once the public repo is published. A code of conduct, license and review policy will be included before accepting external pull requests.
