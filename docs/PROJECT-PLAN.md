# Madwewe project plan

Planning baseline: September 22, 2026. All capabilities below are proposed unless explicitly marked verified.

## 1. Product goal

Let an Unreal creator make a scene respond to audio or a MIDI controller in minutes, then ship that behavior inside a game or experience.

The basic interaction is **choose a source → choose a target → tune the response → play**. A user should not need a custom Blueprint graph for common mappings.

Primary users are Unreal artists, musicians building interactive visuals, and small game/installation teams. The first release serves a single local creator on Windows. Multiplayer, touring-show infrastructure, and cross-platform certification are outside the first release.

Madwewe is defined by its own user journeys: simple mappings, inspectable source, reusable presets, and verified behavior in packaged Unreal experiences. Design and code decisions follow those requirements and the installed engine's documented APIs.

## 2. Verified starting point

- Workspace was empty when planning began. A minimal host project and Git repository were then initialized locally.
- Installed engine: UE 5.8.2, confirmed through Engine/Build/Build.version.
- Visual Studio 2022 Community with C++ tools and Windows SDK 10.0.26100.0 was detected. An editor build and a Win64 Development build/cook/archive succeeded on September 22, 2026.
- The installed MIDIDevice plugin declares a Runtime module, Beta status, and Win64/Mac support. We will promise Windows support first.
- AudioCapture and AudioMixer spectrum APIs are present locally. Capture-device behavior and packaging still need a practical test.
- Maschine MK3 availability is user-reported. It has not yet been connected or tested in this project.
- Git, Git LFS, and an authenticated GitHub CLI for `pixelsncodes` were detected. Remote publication has not been verified yet.

## 3. Scope and success criteria

### First end-to-end demo

A Maschine pad flashes a light; a knob controls the height of a mesh. A bundled audio loop controls another light's intensity. All three work in Play in Editor and a packaged Windows executable. Include a synthetic input mode so the demo and automated tests work without hardware.

### v0.1 public alpha

- MIDI device discovery, explicit connection, refresh/reconnect, event monitor, and Learn.
- MIDI Note On/Off, velocity, channel filtering, and absolute CC. Recognize velocity-zero Note On as Note Off.
- Unreal submix audio: overall level and low/mid/high frequency energy.
- Microphone/default input capture after the early capture spike passes; show the supported device-selection limits honestly.
- Mapping response: input/output ranges, clamp, invert, attack/release smoothing, and a small set of curves.
- Continuous mappings and discrete actions: gate, one-shot trigger, toggle.
- Targets: light intensity/color, material scalar/vector parameters, actor relative transform, Niagara exposed scalar parameters, and explicit Blueprint event/interface hooks.
- Named presets, project persistence, runtime device reassignment, validation, disable/reset controls.
- Small runtime UI for packaged demos; public Blueprint API for teams building their own UI.
- A small original demo scene and a reproducible hardware test guide.

### Explicitly later

Custom node graph; musical timeline; MIDI file import; MIDI Clock sync; beat/BPM estimation; stem separation; OSC; DMX/physical lighting; controller LED feedback; arbitrary property reflection; render-farm/Sequencer baking; cloud service; marketplace distribution; Mac/Linux support; multiplayer replication. Audio transients are not advertised as reliable beat detection.

MIDI controls virtual Unreal lights in v0.1. Controlling physical fixtures requires a later DMX output path.

### Measurable release targets

These are design targets, not achieved measurements:

- A new tester completes their first mapping in under five minutes after installation, without opening a Blueprint graph. Measure with at least three testers.
- A preset survives save/reopen, level reload, and packaged launch.
- A missing controller or deleted target produces an actionable state, without crashing or repeatedly flooding the log.
- One-hour hardware soak with no stuck gates, crash, or growing queue/memory trend.
- Benchmark 100 active simple mappings at 60 fps: target under 1 ms p95 added game-thread time on the reference PC; record PC specs and scene cost separately.
- Target p95 MIDI callback-to-target-application delay within two 60 Hz frames under the baseline load. This excludes device/USB and display latency. Measure end-to-end pad-to-visible response separately and publish the method.
- Zero known release-blocking or core-workflow bugs at a release gate.

## 4. Architecture

Build a C++ Unreal plugin with Blueprint-friendly types. Keep the runtime usable without the editor panel.

| Layer | Responsibility |
|---|---|
| Input adapters | MIDI, submix analysis, capture, and deterministic test signals |
| Signal processing | Normalize values, apply ranges/curves/envelopes, produce values or events |
| Mapping engine | Resolve inputs and targets, evaluate enabled routes, enforce conflict rules |
| Target adapters | Typed light/material/transform/Niagara setters and Blueprint hooks |
| Runtime session | World-local mappings and preset state; device connection through a shared device service |
| Editor UI | Create and inspect mappings, Learn, validate, preview, save assets |
| Runtime UI | Choose devices/presets, view health, reset output; optionally runtime Learn |

Suggested modules: MadweweRuntime and MadweweEditor. Start tests behind development-test guards; split a test module only if the build needs it. Store plugin source under Plugins/Madwewe and the host/demo project at repository root to avoid duplicate plugin copies.

### Decisions to settle in the first implementation spike

1. **Device ownership:** one process-level MIDI service opens ports; per-world sessions subscribe. This prevents multiple PIE worlds from fighting for a port. Retain controller UObject references and release delegates cleanly.
2. **Threading:** audio callbacks produce bounded analysis data; all actor/UObject mutation occurs on the game thread. Use a bounded queue for events; coalesce continuous values without dropping Note Off. Report overflow and release held gates on failure.
3. **Audio:** start from a dedicated analysis submix and reusable FFT/envelope analysis. Analyze each source once, then fan out. Never perform an FFT per mapping. External microphone monitoring is muted by default; prove that the selected analysis routing still receives signal. System/desktop loopback is a separate future feature.
4. **Lifecycle:** sessions survive intended runtime transitions but not destroyed worlds. Device disconnection clears gates and smoothly returns continuous controls to configured baselines. Manual Reconnect is the guaranteed recovery path; hot-plug automation is added only if reliable.
5. **Targets:** typed adapters first. Resolve stable binding IDs plus component/parameter identifiers, using explicit scene binding components for spawned actors. Missing/ambiguous bindings are errors. Check weak references before every use.
6. **Presets:** reusable configuration lives in versioned Data Assets. Scene bindings live with the host scene. User runtime overrides use versioned SaveGame data outside packaged content. Do not persist numeric MIDI device IDs as identity; prefer user-selected device names with duplicate-name disambiguation.
7. **Conflict policy:** reject two enabled mappings writing the same target property in v0.1. A future blend mode must make ownership explicit. Transform changes are offsets from a captured baseline, never frame-by-frame accumulation.
8. **Editor separation:** previews are explicitly enabled and reversible. Editor APIs are absent from shipping runtime dependencies. Runtime mutation does not silently dirty editor assets.

Epic documents streamed MIDI and controller lifetime requirements in its [MIDI guide](https://dev.epicgames.com/documentation/unreal-engine/midi-in-unreal-engine?lang=en-US). The initial audio path uses the locally verified AudioMixer APIs; offline analysis is not a substitute for live input.

## 5. Phase-by-phase delivery

Effort ranges are rough focused developer-day estimates, including iteration. They are not elapsed-time promises or Codex token predictions. Re-estimate after Phase 2; engine/toolchain and hardware issues can dominate the schedule.

| Phase | Deliverable | Exit gate | Lead model | Rough effort |
|---|---|---|---|---|
| 0: Foundations | Reproducible engine/toolchain baseline; repository publication package | Blank C++ project builds and packages; owner/license settled before GitHub publication | GPT-6 Sol, medium | 1–2 days |
| 1: UX validation | Mapping workflow and native panel design specification; prototype | User walks through first mapping, Learn, disconnect, and preset recovery | GPT-6 Sol, medium | 1–2 days |
| 2: Technical proof | Pad→light, knob→mesh, audio→light in a packaged build | Real hardware and synthetic input both work; teardown/restart succeeds | GPT-6 Astra, high for architecture; Sol for implementation | 3–5 days |
| 3: Runtime core | Deterministic mapping engine, typed targets, versioned presets | Automation tests pass; multiple worlds and missing bindings behave correctly | GPT-6 Sol, medium/high | 4–7 days |
| 4: Creator UI | Dockable editor panel, Learn, tuning, diagnostics, undo | First-mapping usability target met; saved setup reloads | GPT-6 Sol, medium | 4–7 days |
| 5: Public alpha | Packaged runtime controls, polished demo, quickstart, tagged source release | Full Windows matrix and MK3 checklist pass | Sol for fixes; Luna for docs | 2–4 days |
| 6: Beta hardening | External-test feedback, performance report, fixes and migrations | Soak/performance targets met; no core blockers; clean clone reproduced | Sol; Astra for persistent deep bugs | 3–6 days |
| 7: v1.0 | Stable documented API, compatibility policy, release artifacts | Release checklist signed off, fresh install passes, known limits documented | Astra review; Luna release docs | 1–3 days |

Total planning envelope: 19–36 focused developer-days plus scheduling for external testing. Aim for small public progress updates throughout, not a long silent build.

### Phase 0 tasks

Verify MSVC/SDK via Unreal's actual build; record engine version, GPU, CPU, audio device and driver versions in a local test report. Establish a minimal C++ host, runtime/editor plugin skeleton, clean build/package scripts, ignore rules, and an original test fixture. Create the public repository after owner/name and license are set. Publish the plan before claiming any features.

### Phase 1 tasks

Use the UX specification to validate four short journeys: audio beginner, MIDI performer, game developer, and disconnected-device recovery. Mock the workflow cheaply; a browser prototype may help but is not the delivered Unreal UI. Confirm native Slate feasibility and keyboard/DPI behavior. Freeze the small vocabulary: Source, Target, Response, Preset, Learn, Reset.

### Phase 2 tasks

Implement minimal MIDI connection and event handling; retain controller references. Confirm actual MK3 notes, CC values, channel numbering, and encoder mode rather than assuming factory settings. Add a synth/test source and one bundled audio source. Package immediately. Record failure cases and choose default buffer/FFT settings from measurement. Do not build a polished panel while packaging is still unresolved.

### Phase 3 tasks

Implement source identities, mapping types, deterministic smoothing, range validation, triggers, target adapters, binding resolution and persistence. Add tests for each invariant. Add event recording/replay for compact diagnostic fixtures, with bounded file sizes and opt-in capture. Runtime settings must not require an editor asset write.

### Phase 4 tasks

Build the dockable Slate panel and mapping details. Add Learn with timeout/cancel; live input/output meters; duplicate mapping; enable/disable; preset save; validation and a bounded event monitor. Support transactions for persistent edits and restoration after preview. Run the usability sessions and repair the largest friction points.

### Phase 5 tasks

Ship the narrow alpha feature set. Runtime UI must select an available controller when the saved one is absent and explain missing audio. Add demo presets: Pulse Lights, Play Motion, Trigger a Scene Event. Document a Blueprint interface example so a MIDI pad can trigger a game event beyond lighting. Capture a short honest demonstration and publish tested limitations.

### Phases 6–7 tasks

Triage external issues, reproduce them in the small demo, add regression coverage, and profile real load. Test the install guide on a fresh checkout. Freeze public types only after real usage; write schema migrations before changing saved data. Consider a second engine version only after the baseline is stable. Produce source release, permitted platform binaries, changelog, and benchmark evidence.

## 6. Decisions and change control

Defaults: Windows first, UE 5.8.2 pinned, local/offline runtime, no user account or OpenAI API needed in Madwewe itself, MIT for original code, no custom graph in v0.1. GPT models help develop the tool; they are not runtime dependencies.

Before publication: confirm GitHub owner, repository name availability, license holder/license choice, attribution wording, and packaging/distribution obligations. Before beta: settle minimum hardware from measurement. New feature requests enter a backlog; they do not silently expand the active milestone.

Use short architecture decision records for MIDI ownership, preset schema, conflict rules, editor preview semantics, and audio-source choice. A phase finishes only with its evidence and exit gate, not when generated code appears complete.
