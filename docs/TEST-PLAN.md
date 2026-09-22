# Testing and bug-fixing plan

## Verification layers

| Layer | What it proves | When |
|---|---|---|
| Pure logic tests | Normalization, ranges, curves, smoothing, thresholds, MIDI semantics | Every mapping-core change |
| Unreal automation | UObject lifetime, bindings, preset round trips, world lifecycle | Relevant PRs and every milestone |
| Functional demo tests | Real actor/material/Niagara changes from deterministic sources | Each target adapter and release |
| Packaged Windows tests | Runtime dependency/cooking correctness, persistence, device startup | Phase 2 onward; every release |
| MK3 hardware tests | Actual driver, port, control and reconnect behavior | First MIDI spike, input changes, releases |
| UX review | Learn, errors, first success, keyboard/DPI, recovery | Panel milestones and beta |
| Performance/soak | Timing, queue bounds, stability and memory trend | Alpha/beta gates and performance changes |

Use Unreal's [Automation Test Framework](https://dev.epicgames.com/documentation/unreal-engine/automation-test-framework-in-unreal-engine) for engine integration, and functional tests for scene behavior. Add Gauntlet when launch/package testing becomes repetitive. Keep small mathematical code independent enough to test without expensive scene initialization.

## High-value automated cases

- MIDI 0/127 bounds, velocity-zero Note On, channel filtering, duplicate notes, multiple held pads, CC flood, disconnect while a note is held, close/reopen, and queue overflow recovery.
- Absolute CC first. Relative encoders require an explicitly identified encoding, with fixtures for each supported mode. No claim that every MK3 template uses absolute CC.
- Zero input range, reversed output range, NaN/infinite values, clamp/invert ordering, threshold hysteresis, and an impulse through attack/release.
- Frame-rate-independent smoothing at 30/60/120 Hz with tolerance-based comparisons; a hitch should not cause uncontrolled output.
- Trigger fires once per event/edge, gate releases, and toggle resets correctly. Avoid executing one-shot actions on every frame a note remains held.
- Transform baseline does not drift over repeated updates. Material instances are created once, not each frame. Missing parameters are reported.
- Deleted actor, duplicate binding ID, spawned actor appearing late, renamed component, level unload, multiple PIE sessions, and PIE stop/restart.
- Preset round trip, missing device, incompatible schema, corrupted saved override and migration from a retained older fixture. User-facing error preserves the original file.
- No editor dependencies in a Shipping build. Ensure soft-referenced required demo assets are cooked.
- Silent input, known sine frequencies, impulses, clipping, source switching and supported sample rates (start with 44.1/48 kHz). Verify frequency-band response rather than relying only on attractive visuals.

## Maschine MK3 procedure

Native Instruments documents **SHIFT + CHANNEL (MIDI)** to enter MIDI mode. Mappings depend on the selected Controller Editor template; see the [official manual](https://docs.native-instruments.com/ni-tech-manuals/maschine-mk3-manual/en/basic-concepts).

1. Record OS, UE version, plugin commit, driver/Controller Editor versions, selected MIDI port, USB connection and template name.
2. Enter MIDI mode. If the port cannot open, check whether another application owns it. Record the actual port name.
3. In Madwewe's monitor, press each of the 16 pads softly and firmly; verify Note On, velocity and release. Record actual note/channel values.
4. Turn each of the eight knobs both ways. Identify absolute versus relative CC and actual controller numbers. Configure an absolute-CC test template for the first supported baseline.
5. Learn Pad 1 to a light gate and Pad 2 to a one-shot Blueprint action. Learn Knob 1 to intensity and Knob 2 to mesh height.
6. Hold pads together, turn knobs while tapping, and release everything. Verify no stuck gates or repeated triggers.
7. Save; stop/restart PIE; close/reopen the project; reload the preset. Check device identity restoration.
8. Repeat in Development and Shipping packaged builds with the editor closed.
9. Unplug while holding a pad, reconnect to the same and another USB port, and use Refresh/Reconnect. Expect recoverable status and released gates.
10. Run a one-hour mixed MIDI/audio session. Capture timing, queue high-water marks, dropped-event counters, memory trend and observed issues.

The user performs physical pad/knob actions and judges perceived response. Codex prepares the test, examines logs, fixes code, reruns automation and updates results. Hardware verification remains pending until actually performed; synthetic events do not substitute for it.

## Audio procedure

Start with generated/owned WAV test signals routed through an explicit submix. Verify overall level and low/mid/high signals against expected frequency regions. Then test the actual available microphone/input device, silence, permission denial, device loss and switching. Test monitoring muted so microphone input does not accidentally feed loudspeakers. Desktop audio and ASIO-only workflows are not supported claims without separate implementation and tests.

## Performance measurement

Record hardware and scene baseline with the plugin disabled, enabled without mappings, and with 1/10/100 active mappings. Use Unreal Insights for CPU timing and queue/thread investigation. Measure game-thread processing independently from rendering cost, DSP latency and input/display latency.

Measure callback-to-application time with timestamps in one clock domain. For physical pad-to-visible delay, use an external/high-frame-rate recording or appropriate instrumentation; document uncertainty. Headless functional checks cannot validate visible response latency. Run sustained-rate synthetic input separately from physical tests.

Check memory after warmup and at repeatable intervals during the soak. Investigate growth rather than masking it with garbage collection. Disable verbose event logging for the performance baseline and test the bounded monitor separately.

## Bug lifecycle

Issue → reproduce → smallest failing fixture → fix → regression test → targeted rerun → packaged/hardware rerun when affected → changelog → close with evidence.

Severity: P0 data loss/crash or release unusable; P1 core workflow broken/stuck outputs; P2 degraded behavior with workaround; P3 polish. P0/P1 block release. Reproduction records expected/actual behavior, engine/plugin version, device/template, minimal setup and relevant logs. Avoid public uploads of personal paths or unrelated device identifiers.

After two failed fixes to the same reproducible root problem, stop patching symptoms, write a compact diagnosis and request GPT-6 Astra high for focused investigation. After resolution, return normal implementation to Sol.

## Release gate

- Clean checkout builds and packages on the documented configuration.
- Required automated suites pass with report files and exact commit recorded.
- Hardware and audio matrix entries marked Pass/Fail/Not tested, never implied by another test.
- Presets/migrations and clean installation guide verified.
- No P0/P1 issues; P2 limitations are documented.
- Performance and soak evidence stored; demo assets and music have redistribution rights.
- Runtime package operates without an editor install; source-build instructions identify required Unreal install.
- Rollback available as the previous tag/artifact; saved-data compatibility documented.

Each test report records date, commit, engine/platform, configuration, suite, result, evidence location, tester and unresolved issues. Unreal test reports can be exported; see [Epic's report guide](https://dev.epicgames.com/documentation/unreal-engine/review-test-results-in-unreal-engine?lang=en-US).
