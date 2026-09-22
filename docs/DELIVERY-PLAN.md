# Public delivery, Codex workflow and skills

## GitHub and build in public

Public repository: [pixelsncodes/madwewe](https://github.com/pixelsncodes/madwewe), with `main` as its default branch. The initial commit contains the plugin skeleton, demo host, plans and contribution guidance. Keep original plugin source, demo project, tests, docs and build scripts in this one repository.

Proposed layout after scaffolding:

```text
MadweweDemo.uproject
Config/
Source/MadweweDemo/
Content/MadweweDemo/
Plugins/Madwewe/
  Madwewe.uplugin
  Source/MadweweRuntime/
  Source/MadweweEditor/
  Content/
Tests/Fixtures/
Scripts/
docs/
.github/ISSUE_TEMPLATE/
.github/workflows/
AGENTS.md
README.md
LICENSE
CONTRIBUTING.md
CHANGELOG.md
```

Track original binary Unreal assets through Git LFS after verifying it is installed. Keep fixtures small. Ignore Binaries, Intermediate, Saved, DerivedDataCache, local IDE state and generated builds. Publish appropriate built artifacts through Releases rather than committing build output.

Before initial publication: verify the owner/name and authenticated GitHub access, review staged files, add project-specific ignore/LFS rules, and verify no private material or third-party restricted assets are included. The MIT license covers original Madwewe code only. Review Unreal's [current EULA](https://www.unrealengine.com/eula/unreal) for runtime versus editor-tool distribution before publishing binaries; do not redistribute Engine source or assume every editor artifact can ship through GitHub.

Use a protected main branch where account features permit, short feature branches and one coherent PR per ticket. Require a successful relevant check and a concise review; the user is release owner. Don't create fake approvals for a solo project. Milestones mirror Phases 0–7. Board columns: Backlog, Ready, In progress, Needs hardware test, Review, Done. Keep one main implementation ticket active at a time.

Label issues by area (midi/audio/runtime/editor/docs), severity and type. A Ready issue contains the user outcome, acceptance cases, non-goals, dependencies, expected test evidence and suggested model. Changes that affect saved data or public API require a short decision record.

Publish a brief devlog per milestone or meaningful demo: what works, evidence, what failed, next experiment. Show user-visible outcomes such as a pad moving a light. Keep the README accurate about prototype/alpha status, supported versions and limits. Use original/licensed music and visuals. Start with GitHub Issues/Discussions; a website and separate community server can wait until there is an alpha to try.

## CI that matches Unreal development

GitHub-hosted runners initially check Markdown, configuration/schema fixtures, repository hygiene and any independent logic tests. They do not automatically have a licensed/configured Unreal installation. Unreal compile, cook, package, automation and hardware tests run locally against the pinned engine, with commit-specific evidence attached to the PR/release process.

Do not connect this personal PC as an automatically triggered self-hosted runner for public pull requests. If remote Unreal builds are needed later, design an isolated, disposable trusted build environment with explicit execution controls. GitHub explains the public-fork risk in its [secure-use reference](https://docs.github.com/en/actions/reference/security/secure-use).

A green lightweight workflow is not proof of a successful Unreal build. PR evidence explicitly distinguishes automatic checks, local engine checks, and hardware checks. Inspect and pin external workflow dependencies when introducing them.

## Codex operating method

Codex drafts and edits C++, Slate, scripts, tests, docs and issues; executes builds; diagnoses logs; and prepares reviewable changes. The user supplies artistic direction, physical MK3 input, usability feedback and public release decisions. Native Unreal GUI control is not enabled in the current computer-control tool surface; command-line/editor scripting can cover many tasks. A live Unreal MCP connection may improve asset authoring after its setup is verified.

Before a ticket, read the status file, relevant specification and actual installed engine headers. Do not guess an API from another UE version. Define the smallest meaningful acceptance test, implement the change, run relevant checks, review the diff and record evidence. A generated file is not a completed feature.

At each task transition report: task completed, evidence, next task, recommended model/effort, whether a switch is needed. Request manual switching only at useful boundaries. Save a handoff before a switch so a smaller model does not have to reread the entire conversation.

Handoff fields: active ticket, intended outcome, files to read, decisions, commands/test evidence, known failure, next action, recommended model. Keep docs/STATUS.md concise and durable. Use short AGENTS.md guidance once actual build commands have been proven; don't bake untested commands into instructions.

## GPT-6 model routing

This Codex session advertises GPT-6 Astra, Sol and Luna. The app describes Luna as faster/affordable and Sol as a Codex model; treat the following as an initial workload policy to validate on this project. Public official docs retrieved here establish Astra's role, but do not establish exact Sol/Luna pricing or Codex subscription multipliers. No numerical token/cost savings are promised. See [official Astra documentation](https://developers.openai.com/api/docs/models/gpt-6-astra).

| Work | Model and effort | Why / switch condition |
|---|---|---|
| Architecture, audio/MIDI ownership, concurrency, preset schema | GPT-6 Astra high | Use for consequential decisions and unknown failure modes |
| Most C++/Slate implementation, build scripts, tests, normal fixes | GPT-6 Sol medium | Default implementation setting; use high for difficult bounded changes |
| README/devlogs, issue preparation, formatting, routine handoffs | GPT-6 Luna low or medium | Well-specified work with cheap verification |
| Persistent crash, race, leak, engine/plugin lifecycle defect | GPT-6 Astra high | Escalate with a reproduction and logs after repeated failed normal attempts |
| Release review and API/schema changes | GPT-6 Astra high, focused scope | Assess consequential risks and evidence, then return to Sol/Luna |

Example transition: “Phase 2 architecture is settled. Switch to GPT-6 Sol, medium, for ticket M-003: implement MIDI input and its lifecycle tests. Handoff is in docs/STATUS.md.”

Reduce consumption by keeping tickets bounded, using targeted searches, retaining decision/test summaries, batching independent reads and letting compilers/tests do verification. Avoid giant prompts, repeated full-repository reviews, automatic high reasoning for routine work, and speculative rewrites. Measure completed tickets and rework as well as usage; a stronger model can be cheaper overall on a hard bug.

No automatic model changes or parallel agents are planned. The user will switch manually when requested. No model switch is needed to finish this planning deliverable; Phase 0 implementation should start with GPT-6 Sol medium.

## Skills searched and recommendations

The official openai/skills curated catalog was fetched with Skill Installer on September 22, 2026. Its installed flags inspect the standalone skills directory, so they can say false even when an equivalent bundled skill is already available in this app. Nothing was installed during planning.

| Skill / source | Recommendation | Purpose and limitation |
|---|---|---|
| [gh-fix-ci](https://github.com/openai/skills/tree/main/skills/.curated/gh-fix-ci) | Install when GitHub checks exist | Inspect failing Actions runs; requires working GitHub CLI/auth. It does not perform Unreal hardware testing. |
| [gh-address-comments](https://github.com/openai/skills/tree/main/skills/.curated/gh-address-comments) | Install when PR review starts | Work through review feedback with traceable fixes. |
| [unreal-mcp, Epic Games](https://github.com/EpicGames/unreal-engine-skills-for-claude-code-plugin/tree/main/skills/unreal-mcp) | Evaluate in Phase 0/1 before installing | Epic-authored guidance for live editor operations. Packaged for Claude Code: verify Codex compatibility, required UE plugins and MCP setup first; do not copy Claude hooks blindly. |
| [unreal-cpp-gameplay, community](https://github.com/gamedev-skills/awesome-gamedev-agent-skills/tree/main/skills/unreal/unreal-cpp-gameplay) | Optional after source/license audit | UE reflection/module conventions. Its advertised version/toolchain assumptions need checking against installed 5.8.2; not a required dependency. |
| [unreal-editor-automation, community](https://github.com/adventuresincausality/game-development-constitution/tree/main/skills/unreal-editor-automation) | Optional after audit | Editor scripting and automation references. Use only selected relevant resources; verify commands locally. |
| [unreal-umg-commonui, community](https://github.com/adventuresincausality/game-development-constitution/tree/main/skills/unreal-umg-commonui) | Optional for runtime UI | Useful UMG context; CommonUI is not necessary for this small UI, and this does not replace Slate expertise. |
| [playwright](https://github.com/openai/skills/tree/main/skills/.curated/playwright) | Later, only for a web prototype/site | Browser testing; does not test a native Slate panel or packaged Unreal game. Existing browser tools may suffice. |
| [Figma skill catalog](https://github.com/openai/skills/tree/main/skills/.curated/figma) | Optional if the user chooses Figma | Current plan can be designed with native components and a small local prototype; no Figma installation is required. |
| openai-docs, skill-installer, skill-creator | Already available | Model guidance, selected installations, and creating a narrow local workflow skill later. |
| computer-use, visualize, imagegen | Already available | Optional supported UI/browser work, concept prototypes and original demo/promotional imagery; not an Unreal build toolchain. |

There is no Unreal-specific skill in the official curated catalog returned by this search. Community skills are discovered candidates, not audited endorsements. Pin a reviewed revision before installation. Skills are instructions, not new engine capabilities; an Unreal MCP connection is separate tooling.

Best initial additions: gh-fix-ci and gh-address-comments when the repository is active. Evaluate Epic's unreal-mcp separately because it could substantially help authoring assets. We can begin C++ work using local engine headers and build tools before that integration exists.

Once the build/package/hardware loop is proven, consider a small project skill covering the verified Madwewe workflow. Do not create a large generic Unreal instruction library before learning what is actually needed.

## Initial implementation tickets

| ID | Task | Acceptance | Depends on |
|---|---|---|---|
| M-001 | Verify toolchain and create minimal host/plugin | Clean build and packaged blank host; actual commands recorded | None |
| M-002 | Prepare/publish public repository | Owner/name/license set; clean contents; README, roadmap, contributing and issue templates | M-001 for technical baseline; owner decision |
| M-003 | MIDI device service and event monitor | MK3 events observed; lifecycle and velocity-zero semantics tested | M-001 |
| M-004 | Synthetic input and light/transform adapters | Deterministic input changes expected properties and resets | M-001 |
| M-005 | Package MIDI vertical slice | Pad→light and knob→mesh work with editor closed | M-003, M-004 |
| M-006 | Submix audio vertical slice | Test audio→light works in packaged build; frequency fixtures pass | M-004 |
| M-007 | Capture feasibility | Actual microphone input analyzed with monitoring muted; limitations recorded | M-006 |
| M-008 | Mapping and persistence core | Lifecycle, preset, range, smoothing and conflict tests pass | M-005, M-006 |
| M-009 | Native creator panel and Learn | Core UX journeys pass, including Undo and recovery | M-008, Phase 1 design |
| M-010 | Alpha release candidate | Test matrix, quickstart, original demo and release notes complete | M-007–M-009 |
