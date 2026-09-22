# Hardware observations

## Maschine MK3 — first input, September 22, 2026

Evidence: user-supplied Madwewe MIDI Monitor screenshot during Play in Editor. The MK3 was in MIDI mode. The active input reported **Maschine MK3 Ctrl MIDI**; the dropdown happened to display **Maschine MK3 EXT MIDI (port 1)**, which was selected but not active. The monitor UI now distinguishes active from selected ports.

One pressed pad produced channel **1**, Note On **48** with velocity **108**, Note Off **48**, and intervening messages for number 48 (sample values 15, 31, 5, and 0). Later screenshots with the raw type column confirmed these are type **10**, polyphonic pad pressure. Another pad sent Note **49**. These observations describe the current controller template, not a default mapping claim for every MK3.

The initial viewport was black even though the test rig appeared in the Outliner. The dedicated preview camera, front-facing light, and larger cube made it visible. **Use last pad** assigned channel 1 Note 48, confirmed by the monitor. The synthetic CC 21 test visibly moved the cube upward, but nearly out of frame; camera distance has now been increased and needs a visual retest. The MK3 light response has not yet been clearly confirmed by the user. No knob CC number, disconnect behavior, packaged hardware response, or end-to-end scene response has been verified. Both EXT and Ctrl ports were available, with Ctrl supplying the observed pad messages.
