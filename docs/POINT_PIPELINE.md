# Point density and dwell

## Established stages

| Stage | Source and effect |
| --- | --- |
| Geometry generation | TD trace_all emits same-coordinate off/on starts, optional contour closure, then off at the start. Its threshold variant inserts additional state changes |
| Alternative TD interpolation | root script uses `ceil(distance/10)` steps, from i=1 through steps; first off/on commands share a frame, end includes a five-frame pause |
| Stored arrays | 16-bit X/Y pairs, bit 15 of X encodes on. Raw coordinate repeats must be distinguished from equal coordinate+state repeats |
| Master subdivision | LineSubdivider.cpp uses i=0 through steps: each segment emits steps+1 points, including both endpoints, even for zero/short segments. Integer rounding adds repeats |
| Symbol boundary | Subdiv sender starts off, then first segment resends its start; ends off at last coordinate. Non-subdiv helper instead sends off at (0,0) |
| Uno state | Drawing::processSinglePoint changes laser state BEFORE movement |
| Transforms | Optional 3D projection; nonlinear applyCorrection; fixed-point global scale and offset; Cohen–Sutherland line clipping; separate text scale/rotation/offset. Direct point commands reset offset but do not reset speed or 3D state |
| Second interpolation | Laser::sendtoRaw divides motion again based on `_quality`; default reciprocal quality 1/30, later `setSpeed(s)` sets 5/s |
| Output | X flip, DAC clamp to 12-bit, pair update and LDAC; end delay 5 us, each interpolation move 5 us, LDAC pulse 5 us in local driver |
| Blanking/dwell | 1500 us wait BEFORE actual on/off transitions. Off delay retains illumination at old point. Player delays (6 ms for normal outlines) also retain state between packets |

Text drawing changes speed persistently; subsequent point outlines inherit it. Thus local density depends on preceding text commands as well as source geometry. `sendSymbolAndFillEx` ignores steps_fill, streams fill without subdivision, and has no explicit final off-point.

## Measurements

`docs/audit/points-fulltexts.json` analyzes all 195 arrays, independently: 245,185 stored points, 139,107 on and 106,078 off; 53,810 consecutive same-coordinate pairs, of which only 78 have identical state and 53,732 are state transitions. All decoded coordinates are within 0–4095.

Uniform steps=5 modeling of every array yields 1,470,330 output points and 1,000,428 consecutive same-coordinate pairs. This is a controlled illustration, **not the actual playlist**: the selected loop uses only a subset, and the stone has different parameters/repeats. The tool reports per-array length quantiles, visit frequencies and a delay-weighted on-dwell proxy. It excludes float32 rounding differences, Uno transformations/interpolation, UART overhead, actual repeat counts, mechanical settling and optical power. It cannot estimate safe exposure or actual brightness.

At 57600 with assumed 8N1, an 8-byte point packet takes about 1.39 ms of wire time. Sender `delay(6)` is not the measured DAC dwell. Queueing, synchronous text, software serial interrupts and blanking delays need a logic-analyzer trace.

## Correctness before tuning

`sendtoRaw` calculates diffx with rounded fixed-point arithmetic, then divides by diffx without guarding zero. Same-coordinate and sufficiently short moves produce zero. With initial quality, a one-unit move evaluates `TO_INT(1*546)` to zero. This is a source-proven invalid division, not a measured hardware crash. `FROM_INT` also left-shifts potentially negative signed values; portability requires reviewing fixed-point arithmetic.

Do not blanket-deduplicate off/on pairs or change timing in this baseline. After preservation, first validate parser recovery/blanking and zero-distance handling on a separate fix branch, then choose one intentional interpolation stage and compare captured DAC/blanking traces on the same artwork.
