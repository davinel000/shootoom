# Šūtum S3 0.1.5 — exhibition playback and 45° rotation

31 October 2026. [Open the sketch](ShootoomS3/ShootoomS3.ino). No upload or optical test by the agent.

## Changes from 0.1.4

- Rotation is **0°, 45°, 90°, 135°, 180°, 225°, 270°, 315°**, one detent per step, wrapping in either direction. The eight cyan markers occupy adjacent LEDs, like the purple orientation variants. USB `angle` accepts the same eight values.
- Initial scale is **1.0**, adjustable down to 0.05; initial angle is **0°**. When importing a valid older profile, scale and angle reset to these values. Orientation, speed, gaps, pixel count, program, blank timing and existing autorun policy otherwise retain their prior values. Old diagnostic selections from version-1 profiles still map to Frame with autorun off.
- After you explicitly save a 0.1.5 profile, your chosen reduced scale/angle are restored on subsequent boots. They are not reset every time. Until Save, an old profile imports at scale 1.0 / angle 0° again after reboot.
- **Show now contains only symbols, lettering and existing scene pauses.** No stone contour at startup or at any subsequent cycle. Stone remains a separate adjustment program.
- **Frame + F is retained unchanged**, including the large orientation F. Angular fitting is unchanged: 45° applies ~0.707 of requested scale; 0° applies the requested scale. `status` shows both.

## Upload yourself

Open `ShootoomS3/ShootoomS3.ino` **inside this 0.1.5 folder** in Arduino IDE. Select **XIAO ESP32-S3 Plus**, **USB CDC On Boot Enabled**, and the actual connected port. Upload. Serial Monitor: **115200 / Newline**; `status` should report **0.1.5**. First import should show `scale=1.000`, `angle=0deg`; the saved orientation is retained.

The existing autorun policy is retained during migration. If it was enabled, its five-second countdown uses the imported scale 1.0 / angle 0°; press/turn cancels the countdown.

## Same gestures in every program

| Gesture | Result |
| --- | --- |
| Short click and release | Next parameter |
| Turn | Edit it, including while drawing |
| Hold about **2 seconds**, then release | Play / pause / resume |
| Hold **at least 3.5 seconds**, then release | Save all settings and selected default program |

The exact play/pause interval is 1.2…less than 3.5 s; longer holds save instead. Actions happen on release. Button-down blanks/suspends playback. These gesture thresholds are unchanged from 0.1.4; this update does **not** establish the cause of the owner's unreliable-start report. A short click on a software fault acknowledges it and remains paused.

Menu order:

| Colour | Field |
| --- | --- |
| White | **Show / Stone / Frame + F**, three adjacent choices |
| Blue | Scale, 5…100%, 1 percentage point per detent |
| Purple | Eight orientation swap/reflection variants |
| Cyan | Eight 45° rotation positions, starting at 0° |
| Pink | Drawing speed, 25…400%, 25 percentage points per detent |
| Amber | Gap between cycles of the selected program, 0…60 s |
| Green | Autorun: positive turn = all green / ON; negative = one green / OFF |

The last LED is green for playback/repeat wait, amber for pause. Autorun uses the whole ring instead. White flash confirms a successful save; red blinking means a software fault. Exact values are available through `status`; ring colours do not measure optical output.

Geometry/speed settings are shared across programs; each program retains its own repeat gap. Defaults: Show 10 s, Stone 0 s, Frame 0 s. Gap 0 means continuous repetition. Drawing speed changes movement and stroke timing, including lettering; scripted scene pauses remain unchanged. Save is explicit only and writes NVS while blanked, preserving playing/paused state.

For exhibition operation: select **Show**, set the desired parameters, set all green / autorun ON, then hold at least 3.5 s and release to save. On subsequent power-up a five-second countdown precedes the saved Show, immediately starting the symbols. Holding the button at power-on suppresses startup.

## USB and verification

`status`, `diag`, `monitor on/off`, `stop`, `pause`, `clear`, `arm`, `show/stone/frame/resume`, `preview`, `calpreview`, `select show/stone/frame`, `scale .05..1`, `orient 0..7`, `angle 0/45/...315`, `speed 25..400`, `gap 0..60`, `pixels 1..24`, `boot auto/manual`, `save`. USB lit starts/resume require `arm` within ten seconds while paused. `stop` / `!` resets; pause retains position. Blank timing commands remain pause-only. Wi-Fi, laser power and wiring are unchanged.

After upload, check the version/defaults, all eight rotation positions in Frame + F, then switch to Show and confirm there is no stone outline. Save your settings and verify one power cycle. The agent has compiled/tested code, not the physical board; [build evidence](BUILD.md).
