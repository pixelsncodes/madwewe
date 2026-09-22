# User experience and UI plan

## Product experience

Madwewe lives inside Unreal. The creator builds mappings in a dockable editor panel, watches results in Unreal's viewport, and saves the setup. The shipped experience loads those mappings through the runtime plugin. A small optional UMG panel handles controller/audio setup for end users.

Use native Slate for the editor panel and Details customizations where practical. Use UMG for the sample runtime controls. Do not add a browser/Electron application or a custom graph editor to v0.1. A design prototype is disposable; the native Unreal behavior is the acceptance target.

## Core journeys

### Audio creator: first success

1. Enable the plugin and open the included demo.
2. Open Madwewe and choose the included audio source. A meter immediately shows whether signal exists.
3. Select a light in the viewport; choose Add Mapping → Selected Actor → Intensity.
4. Choose Audio Level, then adjust Minimum, Maximum and Smoothness.
5. Start explicit Preview or Play. Save the preset.

No microphone access is needed for onboarding. The next step offers Live Input, lists supported devices, and explains device/permission failures in context.

### MIDI performer: first success

1. Connect the MK3 and enter MIDI mode; choose its available input port.
2. Select a target and click Learn. The panel says “Move a knob or press a pad.”
3. Learn captures an eligible Note or CC message, displaying device, channel, event and value.
4. Confirm the mapping. Notes suggest Trigger/Gate; CC suggests Continuous. Learn times out or cancels without changing a previous binding.
5. Set response range and play. Save the preset.

Ignore clock/active-sensing chatter during Learn. Never guess which identical device is intended. Expose raw note numbers alongside any note-name convention to avoid octave-label confusion.

### Game developer

Add a Madwewe binding component or implement the provided interface on a gameplay actor. Map a pad to an exposed action, and a knob to a named continuous value. Package the project. Runtime setup chooses the controller and preset; all actions function without the editor module.

### Recovery

If a port disappears, show “Controller disconnected,” clear held notes, restore configured baselines and offer Refresh/Reconnect. Preserve mappings. If the target is deleted, show that mapping as Invalid with “Choose target,” while other mappings keep working. If no signal arrives, provide a test pulse and distinguish device status from signal activity.

## Panel structure

```text
Madwewe                         Preset: Pulse Lights*   Save
Input: MK3 [Connected]           Audio: Demo Loop [Active]

[Mappings] [Inputs] [Diagnostics]

Mappings                                      [+ Add]
  On   Source          Target                 Activity
  ✓    Audio: Low      Key Light · Intensity   ▂▆▃▁
  ✓    MIDI: CC 21     Orb · Height            ━━━
  ✓    MIDI: Note 36   Scene · Burst           •

Selected mapping
  SOURCE     [Device / signal]    [Learn]
  TARGET     [Actor / component]  [Property]
  RESPONSE   [Continuous / Trigger / Gate / Toggle]
             Minimum [ ]  Maximum [ ]  Invert [ ]
             Attack [ ]   Release [ ]  Curve [ ]
             [Advanced: input range, threshold, channel]

[Preview: Off]   [Test input]   [Reset outputs]   Health: OK
```

At narrow widths, selected-mapping details stack below the list. At wide widths, place them beside it. The Unreal viewport remains the visual preview; do not render a duplicate scene inside the panel.

## UI states and behavior

| Area | Required states and actions |
|---|---|
| Device | Disconnected, connecting, connected, unavailable/in use, reconnecting; refresh and choose port |
| Learn | Idle, listening, captured, cancelled, timed out; previous binding retained until confirmation |
| Mapping | Enabled, disabled, invalid source, missing target, conflicting writer; specific repair action |
| Audio | Active, silent, unavailable; level meter and input source name; silence is not an error |
| Preset | Saved, unsaved, incompatible version, missing bindings; never silently overwrite |
| Preview | Off, active, paused; explicit restoration to captured baseline on stop |
| Runtime | Ready, missing device, missing preset, reconnect required; usable without an editor window |

Persistent edits support Undo/Redo. Transient input values and meters do not generate undo entries or mark assets dirty. Switching a dirty preset offers Save/Discard/Cancel. Runtime overrides are clearly separate from project defaults.

Reset Outputs clears gate/toggle state and returns managed properties to their captured/configured baseline. It does not destroy mappings. Stop Preview restores the editor scene. A gameplay project can explicitly opt out of restoration for particular event actions, which cannot generally be undone.

## Visual design

Follow Unreal's existing typography, spacing and native controls. Use charcoal surfaces, a restrained teal/cyan accent for active input, amber for attention, and red for invalid states. Pair every color with text/icon state. Avoid flashing UI indicators; activity is a small meter or dot. Keep theme colors in one style set.

Use 8-unit spacing increments, comfortably clickable controls, concise sentence-case labels, visible units, numeric entry alongside sliders, and keyboard focus outlines. Test at 100%, 150%, and 200% Windows scaling and at a narrow dock width. Tooltips explain musical/technical terms without being required to complete the workflow.

Default demo outputs avoid abrupt bright flashes and violent camera movement. Users can deliberately increase output ranges. Keep a visible Reset Outputs control.

## Information model

A mapping has a stable ID, label, enabled flag, source definition, response definition, target binding, baseline/failure policy, and schema version. Raw MIDI values remain visible in diagnostics while normalized values are used internally. Channel labels are 1–16 in UI; convert the backend convention explicitly.

Minimum viable response controls: source input minimum/maximum, output minimum/maximum, invert, clamp, attack/release time, curve, threshold and hysteresis for discrete conversion. Advanced controls stay collapsed. Do not expose controls irrelevant to the current response type.

Initially one writer owns a target property. A conflict shows the existing mapping and offers replacement or cancel. Do not hide non-deterministic “last writer wins” behavior.

## Usability acceptance

Use three short tasks with at least three people outside the implementation: make a light respond to audio; Learn a knob to move an object; recover after a controller disappears. Record time, errors, requested help, and confidence about what is saved. Success: all tasks completable, first mapping median under five minutes, and no lost setup. Fix repeated confusion before adding more controls.

Validate native Unreal keyboard navigation, undo and asset lifecycle in addition to any mockup review. Screenshots of a web prototype cannot certify the Slate implementation.
