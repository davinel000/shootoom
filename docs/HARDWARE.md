# Hardware evidence

The [artist's description of Šūtum](https://www.slavaromanov.art/2024/shootoom) specifies a focused 405 nm laser and photochromic pigments. Wavelength is reported project context, not a measurement of the currently installed module; exact optical power and module identification remain unknown.

| Part/signal | Established from code | Physical confirmation needed |
| --- | --- | --- |
| Uno receiver | SoftwareSerial, Uno pin layout in DAC lineage | Board marking/revision and installed firmware |
| RP2040 player | User report; code uses Serial1, large PROGMEM arrays | Exact board, core, flash capacity, UART pin mapping |
| DAC | `Laser::init`: MCP4X_4822, CS=10, LDAC=7, autoLatch=true, gain 1x both channels | Read chip marking, VDD and actual LDAC wiring |
| SPI | Local driver: 4 MHz, MSB first, mode 0 | Scope SCK/SDI/CS/LDAC; Uno MOSI D11, SCK D13 |
| Laser blanking | `Laser laser(5)`; on HIGH, off LOW | Driver input circuit, voltage, polarity and boot state |
| UART | Uno RX D8 / TX D9, 57600 | Cross-connection, common ground and 5 V to 3.3 V translation on return path |
| Encoder | CLK D2, DT D4, switch D3 | Actual wiring and contact behavior |
| Ring | D6, 12 pixels, GRB, 800 kHz | Exact LED model, supply and wiring |
| Galvos/driver | X/Y values emitted through dual DAC | Model, input range, calibrated center, scan limits |
| Supplies | 5 V breadboard and ±15 V analog rails reported in brief | Measured rails, grounding, connectors, decoupling, strain relief |

Owner confirms that the MCP4822 DAC board is powered from 5 V. The migration prototype therefore needs 5 V AHCT logic buffering on S3 → DAC SCK, MOSI, CS and LDAC; exact wiring, output-enable defaults and DAC marking still need bench confirmation. A compact first-board candidate is Seeed XIAO ESP32-S3 (21 × 17.8 mm, 8 MB flash + 8 MB PSRAM, 11 GPIO); keep microSD optional and map all signals before selecting pins. See [migration plan](MIGRATION_ESP32S3.md).

MCP4822 has an internal 2.048 V reference. The local driver replaces the `5000` initialization arguments with 2048 for the internal-reference model; 1x gain means nominal full-scale near 2.048 V, not 5 V. The inherited generic DAC header describes other device pinouts as well: do not use its VREF/pin comments as an MCP4822 wiring diagram. Consult [Microchip DS20002249B](https://ww1.microchip.com/downloads/en/DeviceDoc/20002249B.pdf).

Needed photographs: both controller markings and pin connections; DAC top marking and all eight connections; analog board markings/connectors; laser driver blanking connection; power supply labels and common-ground distribution. Until those are known, schematic component values, amplifier models and ESP32 GPIO assignments remain TODO.
