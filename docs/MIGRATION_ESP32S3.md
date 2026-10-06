# ESP32-S3 validation plan

ESP32-S3 is a candidate, not a selected replacement. First establish the installed legacy build and wiring. Keep the existing analog stage; no evidence in this audit warrants redesigning its ±15 V section.

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
