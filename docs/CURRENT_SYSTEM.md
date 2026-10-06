# Šūtum: current system — evidence as of 2026-10-05

Artwork context and naming are documented in the [README](../README.md). The technical snapshot names identify components and versions, not separate artwork titles.

## Candidate baseline

The user recalls `Version 2025/Arduino Sketch/Hammurapi reads` and `ScriberLatestSimplified` as the last uploaded projects. This corroborates the project families, not the exact player variant. The Uno candidate's sketch timestamp is 2025-03-04; the player `HammurapireadsEncoded_fulltexts.ino` is dated 2025-03-29, later than `Encoded` (March 13) and `with_points` (March 21). Dates support selection but do not prove successful operation. The RP2040 board may be an Arduino Nano RP2040 Connect according to the user's recollection; **exact model and core remain unconfirmed**.

```text
TouchDesigner images / SOP traces / DAT generators
             | generated unsigned-short XY headers
             v
RP2040-family player (reported hardware; board unknown)
  Cuneiform.h + compiled choreography
  Serial1, 57600 baud  ---> Uno SoftwareSerial RX D8
  Serial1 RX           <--- Uno TX D9, text-completion byte AC
                                  |
                           correction / clipping /
                           interpolation / text
                                  |
                        SPI MOSI D11, SCK D13
                        CS D10, LDAC D7
                                  v
                               MCP4822
                                  |
                         existing analog circuitry
                                  |
                             X and Y galvos
Uno D5 --------------------------------> laser blanking
Uno D2/D4/D3 <--- encoder A/B/button
Uno D6 ----------> 12-pixel NeoPixel ring
```

The analog stage and ±15 V supply are reported in the supplied brief, not measured in this audit. Pin labels D11/D13 assume an Uno with its standard SPI mapping. Actual wiring and logic translators must be inspected.

## Startup and modes

The selected receiver initializes SoftwareSerial at 57600 and debug Serial at 19200, applies scale 0.99, and starts mode 0 (UART reception). Mode 1 draws a grid, mode 2 calls `laser.off()`, mode 3 draws the stone despite its misleading comment. `handleModes()` runs twice per loop. The older preserved receiver starts with scale 0.95 and mode 0 off; its UART reception is mode 2.

The player initializes debug Serial at 19200 and Serial1 at 57600. Setup draws the stone 15 times with subdivision 1 and delay 1 ms. Main choreography uses subdivision 5 / delay 6 ms for outlines of laws 196, 197 and 200, interspersed with text commands and pauses. Merely having fill arrays in Cuneiform.h does not mean the fulltexts loop plays them.

## Build status

No firmware build, upload, timing measurement or physical test was performed. `arduino-cli` was not found on PATH, and no usable configured board toolchain was established. Exact installed library/core versions are unknown; do not invent a reproducible version lock from this snapshot.

Uno dependencies: Arduino AVR core (SPI, SoftwareSerial, EEPROM headers), Adafruit_NeoPixel, and the vendored Laser/Drawing/DAC/Font/Basics files. The player depends on a core providing Arduino.h, Serial1, and compatible `avr/pgmspace.h` / `pgm_read_word`; this header spelling alone does not prove an AVR target. Serial1 pins are not selected in source and depend on the board/core.

Next validation: identify both boards and last Arduino board selection; record core/library versions and build each exact sketch separately, recording flash/RAM use and warnings. The receiver still declares functions removed from Drawing.cpp and retains an unused `runArduino()` caller; linking behavior depends on dead-code elimination. Treat this as a build question, not a proven compilation failure. Required `draw_*` names in uncommented player source are checked by the audit utility record; hardware operation is still unverified.
