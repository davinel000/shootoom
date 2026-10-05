# Hardware evidence

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

MCP4822 has an internal 2.048 V reference. The local driver replaces the `5000` initialization arguments with 2048 for the internal-reference model; 1x gain means nominal full-scale near 2.048 V, not 5 V. The inherited generic DAC header describes other device pinouts as well: do not use its VREF/pin comments as an MCP4822 wiring diagram. Consult [Microchip DS20002249B](https://ww1.microchip.com/downloads/en/DeviceDoc/20002249B.pdf).

Needed photographs: both controller markings and pin connections; DAC top marking and all eight connections; analog board markings/connectors; laser driver blanking connection; power supply labels and common-ground distribution. Until those are known, schematic component values, amplifier models and ESP32 GPIO assignments remain TODO.
