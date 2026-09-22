# Hardware observations

## Maschine MK3 — first input, September 22, 2026

Evidence: user-supplied Madwewe MIDI Monitor screenshot during Play in Editor. The MK3 was in MIDI mode. The active input reported **Maschine MK3 Ctrl MIDI**; the dropdown happened to display **Maschine MK3 EXT MIDI (port 1)**, which was selected but not active. The monitor UI now distinguishes active from selected ports.

One pressed pad produced channel **1**, Note On **48** with velocity **108**, Note Off **48**, and intervening `Other` messages for number 48 (sample values 15, 31, 5, and 0). Those are consistent with note-pressure messages, but the original monitor did not show the raw type, so that classification remains an inference. This is evidence for one pad in the current controller template, not a default mapping claim for every MK3.

The initial viewport was black even though the test rig appeared in the Outliner. The old rig put its light above the cube and relied on an unspecified player view; the binding also listened for Note 36. A dedicated preview camera, front-facing light, larger cube, and **Use last pad** assignment have been built but still need a visual retest. No knob CC number, second port behavior, disconnect behavior, packaged hardware response, or end-to-end scene response has been verified.
