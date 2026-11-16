# ESP32-S3 validation plan

This is the historical migration plan. See the [current S3 firmware](../firmware/esp32-s3/README.md) and [assembly guide](SHOOTOOM_GUIDE.md).

## Architecture decision — proposal updated 2026-10-07

The owner reports stable drawing with no current jitter. The reason for the original two-controller arrangement was the AVR receiver's limited memory. The owner now identifies the small receiver physically as a Nano, rather than an Uno; The AVR firmware targets Arduino Uno and is archived in `firmware/legacy/uno/0.1.0`. The RP2040 is outside the enclosure because the assembly is crowded. Prioritize compact permanent assembly, a working encoder, adjustable drawing/pauses, and software projection sizing.

For this workload, the recommended final architecture is **one controller with sufficient flash**, with XIAO ESP32-S3 Plus as the compact candidate. Two controllers are not a design requirement. The existing Nano RP2040 Connect, if that is the installed RP2040 board, is also technically capable of a single-controller implementation; the S3 choice is driven primarily by enclosure space. Actual board selection still needs a pin map and mechanical fit check.

One firmware contains the stored point arrays, choreography player, text/vector renderer, encoder/NeoPixel interface, and USB command interface. Eliminate the inter-board UART encoder/parser/ACK path for local playback. TouchDesigner can later send control commands through USB to the same parameter interface used by the encoder; autonomous playback must work with the computer disconnected. Direct external point streaming is a later requirement, not needed for the first permanent build. Start with flash-resident drawings rather than SD.

Keep software responsibilities separate within the controller: the renderer advances in bounded time steps; the player schedules scenes and pauses; the user interface edits parameters. Long drawing loops and multi-second `delay()` calls must become resumable operations so encoder, USB, and stop commands remain responsive. Use one interpolation stage with explicit timing. Do not port the current `setSpeed()` meaning unchanged: it changes `_quality` (intermediate point count), while the RP2040 sender separately subdivides and delays each transmitted point.

### Controls for photochromic presentation

| Parameter | Purpose | Current source evidence / proposed change |
|---|---|---|
| Drawing speed | Rate of travel along visible paths | Player uses `interp=5` and `pauseTime=6`; receiver `setSpeed()` changes interpolation quality. Introduce a defined path-speed parameter separately from geometric accuracy. |
| Scene pause | Time with emission off before the next section | Player has fixed `delay(3000/4000/7000)` calls. Make these baseline pauses adjustable by a separate multiplier. |
| Repeated passes | Exposure / strength of a written mark | Stone is repeated 15 times; text commands include repeat counts. Make repeat/exposure settings separate from path speed. |
| Global scale | Size of the whole choreography around a calibrated center | Receiver has `setScale()` and offsets, but the encoder's scale update is commented out. Apply a global transform to all objects and text positions, distinct from local text scales. |
| Center / rotation | Optional calibration | Use one global transform after scene geometry is produced, with bounded output. Introduce after pause/speed/scale controls. |

Recommended encoder UI: short press selects Pause / Drawing speed / Scale, rotation edits the selected parameter, long press toggles run/pause with emission off while paused. Use distinct NeoPixel colors for the parameter and a position/brightness cue for its value. A long press must not block encoder or rendering services. Keep the original stored choreography as the baseline.

At constant laser power, slowing a lit pass increases delivered exposure per path length. Therefore slowing the overall presentation by lengthening blanked pauses is different from slowing the actual lit stroke. Temperature-dependent mark persistence reported by the owner makes independent pause, speed, and pass-count controls useful; a profile's appearance must be tested on the actual pigment at the venue. No universal decay time or automatic temperature law is assumed.

### Permanent assembly

Target one mounted carrier/perfboard with a controller, DAC, appropriate 3.3 V → 5 V logic buffering, defined power distribution, and labeled connectors for encoder, ring, laser control, and X/Y output. The analog conditioning stage may share the board in a separate physical area; software scaling does not by itself replace voltage-range/offset conversion required by the galvo-driver interface. Keep its established transfer function until documented. Neither resistor values nor an op-amp model have been selected here.

Design the power connections and mechanical layout before soldering: record board/enclosure dimensions and mounting clearance, place power entry and larger terminals first, keep USB accessible, and route short digital/analog signal paths. Perfboard can improve layout using underside wiring, but mounting connectors and clearance determine actual size. A custom PCB is optional after the circuit is verified.

Original reference: [DeltaFlo LaserProjector](https://github.com/DeltaFlo/LaserProjector) links [Arduino Laser Show With Real Galvos](https://www.instructables.com/Arduino-Laser-Show-With-Real-Galvos/); its Step 7 is titled “Bipolar Signal With Opamps.” This establishes the reference analog stage's role, not the exact installed circuit or resistor values. See [current wiring guide](SHOOTOOM_GUIDE.md) for outstanding connector/power evidence.

Completion evidence for the first single-controller implementation: build with flash-resident data and record flash/RAM use; verify both DAC channels and blanking; verify encoder/USB responsiveness throughout a long scene and a long pause; measure point timing; confirm transforms respect calibrated limits; compare the original choreography at baseline settings and then assess independent speed/pause/pass-count adjustments on the actual surface.

ESP32-S3 is a candidate, not a tested replacement. Keep the existing analog stage until its wiring/transfer function is documented.

## Compact board candidate

The full ESP32-S3-DevKitC is a bench breakout, not the intended in-enclosure controller. For a compact first S3 prototype, evaluate the Seeed XIAO ESP32-S3 (21 × 17.8 mm, 8 MB flash, 8 MB PSRAM, 11 exposed GPIO). It is small enough to mount on a carrier/perfboard. The present controller signals need about nine GPIO (DAC SCK/MOSI/CS/LDAC, blanking, encoder A/B/button, NeoPixel data); an SPI microSD adds MISO and SD CS and would use the remaining two GPIO. Therefore start without SD and verify pin assignments/boot strapping before wiring. The audit records 245,185 point records across the preserved arrays; packed as 16-bit X/Y pairs this is 980,740 bytes before metadata, so internal flash is the first storage option to test. This estimate is not a compiled firmware size measurement.

If additional pins or an SD slot become necessary, compare the XIAO ESP32-S3 Plus or a custom carrier around an ESP32-S3 module. The Plus's additional underside castellated pads require a carrier-board connection and should not be counted as plug-in breadboard pins without checking the exact pinout. Board choice remains provisional until a pin map and physical fit check are recorded.

## First experiment

ESP32-S3 -> level buffer -> MCP4822 -> existing analog input -> small test pattern. Initially keep laser emission disabled and measure DAC A/B, LDAC and blanking behavior. Identify analog center/range before a bounded moving pattern. Later optical testing requires a contained beam path and confirmed blanking; do not begin with the full choreography or a stationary illuminated point.

Acceptance evidence: cold boot/reset keep blanking off; both DAC channels respond to known bounded codes; gain and center match the analog input; paired update timing is measured; frame cadence is stable; pattern terminates blanked. Record board variant, core version, GPIO mapping and oscilloscope/logic-analyzer captures. No board or timings are silently selected here.

## Minimal driver proposal

Use [Arduino-ESP32 SPI](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/spi.html): initialize the selected SPI bus/pins, put CS and LDAC in inactive state, open `SPI.beginTransaction(SPISettings(clock, MSBFIRST, SPI_MODE0))`, assert CS for each 16-bit channel word, send high and low bytes with `SPI.transfer`, deassert CS, pulse LDAC after both channel registers, then `SPI.endTransaction()`. Clock and pulse timing must meet the DAC datasheet and be validated on the actual wiring. No AVR register access.

MCP4822 words: A/B select in bit 15, gain in bit 13 (1 means 1x), active in bit 12, data in bits 11:0; bit 14 unused for MCP4822. At 1x, active words are `0x3000 | codeA` and `0xB000 | codeB`. Bound code to 0–4095 before encoding; do not wrap out-of-range values. Preserve the measured legacy gain rather than assuming a 5 V output span. This is a design specification, not tested firmware.

## Logic levels

[Microchip DS20002249B, page 6](https://ww1.microchip.com/downloads/en/DeviceDoc/20002249B.pdf) specifies VIH >= 0.7*VDD: at 5 V, that is 3.5 V, so a 3.3 V ESP32 output is not a guaranteed HIGH. The owner confirms the installed MCP4822 board is powered from 5 V. A unidirectional [SN74AHCT125](https://www.ti.com/lit/ds/symlink/sn74ahct125.pdf) powered from 5 V is therefore required in the S3-to-DAC prototype for SCK, MOSI, CS and LDAC; its VIH minimum is 2 V at the specified supply range. Verify loading/timing and ensure defined output-enable and boot states, shared ground and local decoupling. No DAC MISO is required. Laser blanking is a separate interface whose required level/polarity remains unknown. Do not connect a 5 V return signal directly to a 3.3 V controller.

## Blockers and later stages

- Exact ESP32-S3 module/pins and installed RP2040 model remain unknown.
- Existing analog transfer function, DAC gain/LDAC wiring and laser interface need measurement.
- Legacy arithmetic, SoftwareSerial, PROGMEM access and interrupt assumptions require targeted replacement; local GPIO DAC adaptations are not proof of portability.
- Confirm linked data footprint, available flash/RAM, point cadence and jitter. Header source size is not compiled memory consumption.
- Only after the DAC experiment: deterministic local playback and blanking, transformations, then encoder/ring/modes. Storage, USB and optional Wi-Fi follow measured requirements. Do not integrate Wi-Fi, SD or the giant arrays into the first experiment.
