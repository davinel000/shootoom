# Inventory and selection

Audit date: 2026-10-05. Root: `2024-10 - SUTUM - carved-dissolut`. Source tree: 870 files, 1,062,734,408 bytes; 613 code/TouchDesigner files hashed. 19 `.ino` files found. No AGENTS.md was found within this source tree or the newly cloned repository. GitHub main initially contained only README.md.

## Selected scope

Preserved two likely current sketches and one earlier receiver; all copied bytes have SHA-256 evidence in [preserved-files.json](audit/preserved-files.json). User recollection supports the two families; the exact uploaded binaries and RP2040 board remain unknown. No claim of successful compilation or hardware validation is made.

Do not delete cloud duplicates: they retain alternate choreography, settings and historical data. Core/library versions and board identifiers were not recorded in the discovered sources. All target assignments below are inferred from code plus the user report.

## Candidates

### `LaserProjector-master/LaserProjector-master/LaserShow/LaserShow.ino`

Legacy LaserProjector demo lineage; AVR/Uno-oriented DAC/PROGMEM. Not the current encoded sender/receiver pair.

Sketch: 1,034 bytes; source modification time UTC: 2025-02-25T13:17:03+00:00. All builds unverified.

Files: `Basics.cpp` (3,326 B), `Basics.h` (1,018 B), `Cube.cpp` (5,069 B), `Cube.h` (108 B), `DAC_MCP4X.cpp` (7,687 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (11,024 B), `Drawing.h` (1,727 B), `Font.h` (8,413 B), `Laser.cpp` (6,891 B), `Laser.h` (3,624 B), `LaserShow.ino` (1,034 B), `Logo.h` (9,611 B), `Objects.h` (48,725 B).

Includes/libraries: `Arduino.h`, `Basics.h`, `Cube.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `Logo.h`, `Objects.h`, `SPI.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Laser laser(5);`.

Pin/DAC evidence: `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `LaserProjector-master/LaserProjector-master/LaserSpectrumAnalyzer/LaserSpectrumAnalyzer.ino`

FFT/ADC spectrum demo using Teensy Audio headers; alternate platform/demo, not the current UART pair.

Sketch: 11,989 bytes; source modification time UTC: 2017-02-19T01:39:50+00:00. All builds unverified.

Files: `Basics.cpp` (3,343 B), `Basics.h` (891 B), `DAC_MCP4X.cpp` (6,446 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (6,347 B), `Drawing.h` (1,593 B), `Font.h` (8,413 B), `Laser.cpp` (6,891 B), `Laser.h` (3,506 B), `LaserSpectrumAnalyzer.ino` (11,989 B).

Includes/libraries: `Arduino.h`, `Basics.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `SPI.h`, `analyze_fft1024.h`, `input_adc.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Laser laser(5);`.

Pin/DAC evidence: `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `LaserProjector-master/LaserShow/LaserShow.ino`

Legacy LaserProjector demo lineage; AVR/Uno-oriented DAC/PROGMEM. Not the current encoded sender/receiver pair.

Sketch: 9,884 bytes; source modification time UTC: 2017-02-19T11:39:50+00:00. All builds unverified.

Files: `Basics.cpp` (3,326 B), `Basics.h` (1,018 B), `Cube.cpp` (5,069 B), `Cube.h` (108 B), `DAC_MCP4X.cpp` (6,446 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (6,357 B), `Drawing.h` (1,593 B), `Font.h` (8,413 B), `Laser.cpp` (6,891 B), `Laser.h` (3,508 B), `LaserShow.ino` (9,884 B), `Logo.h` (9,611 B), `Objects.h` (8,436 B).

Includes/libraries: `Arduino.h`, `Basics.h`, `Cube.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `Logo.h`, `Objects.h`, `SPI.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Laser laser(5);`.

Pin/DAC evidence: `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `LaserProjector-master/LaserSpectrumAnalyzer/LaserSpectrumAnalyzer.ino`

FFT/ADC spectrum demo using Teensy Audio headers; alternate platform/demo, not the current UART pair.

Sketch: 11,989 bytes; source modification time UTC: 2017-02-19T11:39:50+00:00. All builds unverified.

Files: `Basics.cpp` (3,343 B), `Basics.h` (891 B), `DAC_MCP4X.cpp` (6,446 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (6,347 B), `Drawing.h` (1,593 B), `Font.h` (8,413 B), `Laser.cpp` (6,891 B), `Laser.h` (3,506 B), `LaserSpectrumAnalyzer.ino` (11,989 B).

Includes/libraries: `Arduino.h`, `Basics.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `SPI.h`, `analyze_fft1024.h`, `input_adc.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Laser laser(5);`.

Pin/DAC evidence: `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `New arduino sketch/LaserShow/LaserShow.ino`

Older direct Serial/USB receiver at 115200 with encoder/ring; conflicted sibling is incomplete outside its sketch folder.

Sketch: 16,337 bytes; source modification time UTC: 2025-02-07T15:26:12+00:00. All builds unverified.

Files: `Basics.cpp` (3,326 B), `Basics.h` (1,018 B), `DAC_MCP4X.cpp` (9,561 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (15,879 B), `Drawing.h` (2,383 B), `Font.h` (32,863 B), `font_cascadia_mono.h` (32,495 B), `font_space_mono.h` (34,159 B), `Laser.cpp` (10,653 B), `Laser.h` (4,048 B), `LaserShow.ino` (16,337 B), `NeopixelEncoderController.h` (5,708 B), `Objects.h` (7,114 B).

Includes/libraries: `Adafruit_NeoPixel.h`, `Arduino.h`, `Basics.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `NeopixelEncoderController.h`, `Objects.h`, `SPI.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Laser laser(5);`; `Serial.begin(115200);`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `#define ENCODER_CLK 4`; `#define ENCODER_DT 2`; `#define ENCODER_SW 3`; `extern Laser laser;`.

Pin/DAC evidence: `#define ENCODER_CLK 4`; `#define ENCODER_DT 2`; `#define ENCODER_SW 3`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `New arduino sketch/LaserShow (conflicted).ino`

Older direct Serial/USB receiver at 115200 with encoder/ring; conflicted sibling is incomplete outside its sketch folder.

Sketch: 11,101 bytes; source modification time UTC: 2024-09-22T09:12:31+00:00. All builds unverified.

Files: `Drawing (conflicted).cpp` (10,119 B), `LaserShow (conflicted).ino` (11,101 B).

Includes/libraries: `Cube.h`, `Drawing.h`, `Font.h`, `Laser.h`, `Logo.h`, `NeopixelEncoderController.h`, `Objects.h`.

Interface declarations: `Laser laser(5);`; `Serial.begin(115200);`.

Pin/DAC evidence: `Laser laser(5);`.

Missing local includes: Cube.h, Drawing.h, Font.h, Laser.h, Logo.h, NeopixelEncoderController.h, Objects.h.

### `original LP/LaserProjector-master/LaserShow/LaserShow.ino`

Legacy LaserProjector demo lineage; AVR/Uno-oriented DAC/PROGMEM. Not the current encoded sender/receiver pair.

Sketch: 9,884 bytes; source modification time UTC: 2017-02-19T11:39:50+00:00. All builds unverified.

Files: `Basics.cpp` (3,326 B), `Basics.h` (1,018 B), `Cube.cpp` (5,069 B), `Cube.h` (108 B), `DAC_MCP4X.cpp` (6,446 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (6,357 B), `Drawing.h` (1,593 B), `Font.h` (8,413 B), `Laser.cpp` (6,891 B), `Laser.h` (3,508 B), `LaserShow.ino` (9,884 B), `Logo.h` (9,611 B), `Objects.h` (8,436 B).

Includes/libraries: `Arduino.h`, `Basics.h`, `Cube.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `Logo.h`, `Objects.h`, `SPI.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Laser laser(5);`.

Pin/DAC evidence: `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `original LP/LaserProjector-master/LaserSpectrumAnalyzer/LaserSpectrumAnalyzer.ino`

FFT/ADC spectrum demo using Teensy Audio headers; alternate platform/demo, not the current UART pair.

Sketch: 11,989 bytes; source modification time UTC: 2017-02-19T11:39:50+00:00. All builds unverified.

Files: `Basics.cpp` (3,343 B), `Basics.h` (891 B), `DAC_MCP4X.cpp` (6,446 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (6,347 B), `Drawing.h` (1,593 B), `Font.h` (8,413 B), `Laser.cpp` (6,891 B), `Laser.h` (3,506 B), `LaserSpectrumAnalyzer.ino` (11,989 B).

Includes/libraries: `Arduino.h`, `Basics.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `SPI.h`, `analyze_fft1024.h`, `input_adc.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Laser laser(5);`.

Pin/DAC evidence: `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `Version 2025/Arduino Sketch/Hammurapi/Hammurapi/Hammurapi.ino`

Packet test sender through Serial, mixed with debug text; not the full player.

Sketch: 2,747 bytes; source modification time UTC: 2025-02-11T18:14:43+00:00. All builds unverified.

Files: `Hammurapi.ino` (2,747 B).

Includes/libraries: `Arduino.h`.

Interface declarations: `Serial.begin(BAUD_RATE);`.

Pin/DAC evidence: No explicit pin assignments; UART mapping depends on selected board/core..

### `Version 2025/Arduino Sketch/Hammurapi reads/Hammurapireads/Hammurapireads.ino`

Older frame-based hexadecimal choreography player; Serial1; dynamic String parsing, not current array playlist.

Sketch: 4,251 bytes; source modification time UTC: 2025-02-15T19:02:01+00:00. All builds unverified.

Files: `Choreography.h` (20,866 B), `CommandEncoder.cpp` (532 B), `CommandEncoder.h` (450 B), `Cuneiform.h` (188,176 B), `Hammurapireads.ino` (4,251 B).

Includes/libraries: `Arduino.h`, `Choreography.h`, `CommandEncoder.h`, `avr/pgmspace.h`.

Interface declarations: `Serial.begin(115200);`; `Serial1.begin(57600);`.

Pin/DAC evidence: No explicit pin assignments; UART mapping depends on selected board/core..

### `Version 2025/Arduino Sketch/Hammurapi reads/HammurapireadsEncoded/HammurapireadsEncoded.ino`

Earlier encoded/subdivided player; different smaller Cuneiform.h.

Sketch: 13,041 bytes; source modification time UTC: 2025-03-12T23:55:47+00:00. All builds unverified.

Files: `Choreography.h` (166 B), `CommandEncoder.cpp` (3,260 B), `CommandEncoder.h` (962 B), `Cuneiform.h` (614,269 B), `Cuneiform_stable.h` (818,209 B), `HammurapireadsEncoded.ino` (13,041 B), `LineSubdivider.cpp` (1,081 B), `LineSubdivider.h` (351 B), `SymbolSender.h` (2,619 B).

Includes/libraries: `Arduino.h`, `CommandEncoder.h`, `Cuneiform.h`, `LineSubdivider.h`, `SymbolSender.h`, `avr/pgmspace.h`, `ctype.h`.

Interface declarations: `Serial.begin(19200);`; `Serial1.begin(57600);`.

Pin/DAC evidence: No explicit pin assignments; UART mapping depends on selected board/core..

### `Version 2025/Arduino Sketch/Hammurapi reads/HammurapireadsEncoded_fulltexts/HammurapireadsEncoded_fulltexts.ino`

Selected player candidate; law outlines plus repeated full text, Serial1; RP2040 reported, exact core unknown.

Sketch: 10,486 bytes; source modification time UTC: 2025-03-29T17:02:59+00:00. All builds unverified.

Files: `Choreography.h` (166 B), `CommandEncoder.cpp` (3,260 B), `CommandEncoder.h` (962 B), `Cuneiform.h` (4,179,985 B), `Cuneiform_stable.h` (818,209 B), `HammurapireadsEncoded_fulltexts.ino` (10,486 B), `LineSubdivider.cpp` (1,081 B), `LineSubdivider.h` (351 B), `SymbolSender.h` (3,874 B).

Includes/libraries: `Arduino.h`, `CommandEncoder.h`, `Cuneiform.h`, `LineSubdivider.h`, `SymbolSender.h`, `avr/pgmspace.h`, `ctype.h`.

Interface declarations: `Serial.begin(19200);`; `Serial1.begin(57600);`.

Pin/DAC evidence: No explicit pin assignments; UART mapping depends on selected board/core..

### `Version 2025/Arduino Sketch/Hammurapi reads/HammurapireadsEncoded_with_points/HammurapireadsEncoded_with_points.ino`

Earlier player variant, includes fill/point choreography; same large Cuneiform.h as fulltexts.

Sketch: 26,930 bytes; source modification time UTC: 2025-03-21T16:18:21+00:00. All builds unverified.

Files: `Choreography.h` (166 B), `CommandEncoder.cpp` (3,260 B), `CommandEncoder.h` (962 B), `Cuneiform.h` (4,179,985 B), `Cuneiform_stable.h` (818,209 B), `HammurapireadsEncoded_with_points.ino` (26,930 B), `LineSubdivider.cpp` (1,081 B), `LineSubdivider.h` (351 B), `SymbolSender.h` (3,874 B).

Includes/libraries: `Arduino.h`, `CommandEncoder.h`, `Cuneiform.h`, `LineSubdivider.h`, `SymbolSender.h`, `avr/pgmspace.h`, `ctype.h`.

Interface declarations: `Serial.begin(19200);`; `Serial1.begin(57600);`.

Pin/DAC evidence: No explicit pin assignments; UART mapping depends on selected board/core..

### `Version 2025/Arduino Sketch/Hammurapisimple/Hammurapisimple/Hammurapisimple.ino`

Serial1 geometry test sender, not full choreography.

Sketch: 2,464 bytes; source modification time UTC: 2025-02-13T21:20:41+00:00. All builds unverified.

Files: `Hammurapisimple.ino` (2,464 B).

Includes/libraries: `Arduino.h`, `math.h`.

Interface declarations: `Serial.begin(115200);`; `Serial1.begin(57600);`.

Pin/DAC evidence: No explicit pin assignments; UART mapping depends on selected board/core..

### `Version 2025/Arduino Sketch/LaserShow/LaserShow.ino`

Earlier Uno receiver preserved as alternate: startup off mode 0, UART in mode 2, scale .95.

Sketch: 3,647 bytes; source modification time UTC: 2025-02-25T18:16:23+00:00. All builds unverified.

Files: `Basics.cpp` (3,417 B), `Basics.h` (1,054 B), `CommandParser.cpp` (5,964 B), `CommandParser.h` (686 B), `DAC_MCP4X.cpp` (9,865 B), `DAC_MCP4X.h` (4,823 B), `Drawing.cpp` (16,754 B), `Drawing.h` (2,470 B), `Font.h` (44,086 B), `Laser.cpp` (11,031 B), `Laser.h` (4,048 B), `LaserShow.ino` (3,647 B), `NeopixelEncoderController.h` (7,464 B), `Objects.h` (12,846 B).

Includes/libraries: `Adafruit_NeoPixel.h`, `Arduino.h`, `Basics.h`, `CommandParser.h`, `DAC_MCP4X.h`, `Drawing.h`, `EEPROM.h`, `Font.h`, `Laser.h`, `NeopixelEncoderController.h`, `Objects.h`, `SPI.h`, `SoftwareSerial.h`, `inttypes.h`.

Interface declarations: `Laser laser(5);`; `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `SoftwareSerial mySerial(8, 9);`; `Serial.begin(19200);`; `mySerial.begin(57600);`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `#define ENCODER_CLK 2`; `#define ENCODER_DT 4`; `#define ENCODER_SW 3`; `extern Laser laser;`.

Pin/DAC evidence: `#define ENCODER_CLK 2`; `#define ENCODER_DT 4`; `#define ENCODER_SW 3`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `Version 2025/Arduino Sketch/Mimimalistic Remake/Miminalistic/Miminalistic.ino`

Direct DAC step test, CS10/LDAC7, no active blanking setup; experimental.

Sketch: 3,318 bytes; source modification time UTC: 2025-02-07T16:52:05+00:00. All builds unverified.

Files: `Basics.cpp` (3,326 B), `Basics.h` (1,018 B), `Cube.cpp` (5,069 B), `Cube.h` (108 B), `DAC_MCP4X.cpp` (7,687 B), `DAC_MCP4X.h` (4,679 B), `Drawing.cpp` (11,024 B), `Drawing.h` (1,727 B), `Font.h` (8,413 B), `Laser.cpp` (6,891 B), `Laser.h` (3,624 B), `Logo.h` (9,611 B), `Miminalistic.ino` (3,318 B), `Objects.h` (48,725 B).

Includes/libraries: `Arduino.h`, `Basics.h`, `Cube.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `SPI.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `Serial.begin(115200);`; `dac.init(MCP4X_4822, 2048, 2048, DAC_CHIP_SELECT, DAC_LDAC_PIN, true);`.

Pin/DAC evidence: `const int DAC_CHIP_SELECT = 10;`; `const int DAC_LDAC_PIN = 7;`; `dac.init(MCP4X_4822, 2048, 2048, DAC_CHIP_SELECT, DAC_LDAC_PIN, true);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `Version 2025/Arduino Sketch/ScriberLatestSimplified/LaserShow/LaserShow.ino`

Selected Uno receiver candidate: startup UART mode 0, scale .99; user recollection and date support selection.

Sketch: 3,158 bytes; source modification time UTC: 2025-03-04T02:11:49+00:00. All builds unverified.

Files: `Basics.cpp` (3,417 B), `Basics.h` (1,054 B), `CommandParser.cpp` (5,964 B), `CommandParser.h` (686 B), `DAC_MCP4X.cpp` (9,865 B), `DAC_MCP4X.h` (4,823 B), `Drawing.cpp` (9,001 B), `Drawing.h` (2,470 B), `Font.h` (44,086 B), `Laser.cpp` (11,033 B), `Laser.h` (4,048 B), `LaserShow.ino` (3,158 B), `NeopixelEncoderController.h` (7,493 B), `Objects.h` (12,846 B).

Includes/libraries: `Adafruit_NeoPixel.h`, `Arduino.h`, `Basics.h`, `CommandParser.h`, `DAC_MCP4X.h`, `Drawing.h`, `EEPROM.h`, `Font.h`, `Laser.h`, `NeopixelEncoderController.h`, `Objects.h`, `SPI.h`, `SoftwareSerial.h`, `inttypes.h`.

Interface declarations: `Laser laser(5);`; `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `SoftwareSerial mySerial(8, 9);`; `Serial.begin(19200);`; `mySerial.begin(57600);`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `#define ENCODER_CLK 2`; `#define ENCODER_DT 4`; `#define ENCODER_SW 3`; `extern Laser laser;`.

Pin/DAC evidence: `#define ENCODER_CLK 2`; `#define ENCODER_DT 4`; `#define ENCODER_SW 3`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `Version 2025/Arduino Sketch/Scribersimple/Scribersimple.ino`

Earlier receiver; SoftwareSerial 9600 does not match 57600 players; text case unfinished.

Sketch: 6,282 bytes; source modification time UTC: 2025-02-12T18:49:32+00:00. All builds unverified.

Files: `Basics.cpp` (3,417 B), `Basics.h` (1,054 B), `DAC_MCP4X.cpp` (9,865 B), `DAC_MCP4X.h` (4,823 B), `Drawing.cpp` (16,243 B), `Drawing.h` (2,443 B), `Font.h` (34,925 B), `Laser.cpp` (11,031 B), `Laser.h` (4,048 B), `NeopixelEncoderController.h` (5,864 B), `Objects.h` (7,473 B), `Scribersimple.ino` (6,282 B).

Includes/libraries: `Adafruit_NeoPixel.h`, `Arduino.h`, `Basics.h`, `DAC_MCP4X.h`, `Drawing.h`, `Font.h`, `Laser.h`, `NeopixelEncoderController.h`, `Objects.h`, `SPI.h`, `SoftwareSerial.h`, `inttypes.h`.

Interface declarations: `extern Laser laser;`; `dac.init(MCP4X_4822, 5000, 5000,`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `#define ENCODER_CLK 4`; `#define ENCODER_DT 2`; `#define ENCODER_SW 3`; `extern Laser laser;`; `Laser laser(5);`; `SoftwareSerial mySS(8, 9);`; `Serial.begin(57600);`; `mySS.begin(9600);`.

Pin/DAC evidence: `#define ENCODER_CLK 4`; `#define ENCODER_DT 2`; `#define ENCODER_SW 3`; `#define NEOPIXEL_PIN 6`; `#define NUMPIXELS 12`; `Laser laser(5);`; `dac.init(MCP4X_4822, 5000, 5000, 10, 7, 1);`.

### `Version 2025/Arduino Sketch/SupermimalisticDAC/SupermimalisticDAC.ino`

Direct DAC test, CS10/LDAC7/laser5; drives laser HIGH during setup and holds stationary values; not a presentation baseline.

Sketch: 1,810 bytes; source modification time UTC: 2025-02-07T17:26:48+00:00. All builds unverified.

Files: `DAC_MCP4X.cpp` (7,687 B), `DAC_MCP4X.h` (4,679 B), `SupermimalisticDAC.ino` (1,810 B).

Includes/libraries: `Arduino.h`, `DAC_MCP4X.h`, `SPI.h`, `inttypes.h`.

Interface declarations: `Serial.begin(115200);`; `dac.init(MCP4X_4822, 2048, 2048, DAC_CS_PIN, DAC_LDAC_PIN, true);`.

Pin/DAC evidence: `const int DAC_CS_PIN = 10;`; `const int DAC_LDAC_PIN = 7;`; `const int LASER_PIN = 5;`; `dac.init(MCP4X_4822, 2048, 2048, DAC_CS_PIN, DAC_LDAC_PIN, true);`.

## Generated headers and duplicate evidence

Seven Cuneiform headers form four exact SHA-256 groups. `fulltexts/Cuneiform.h` and `with_points/Cuneiform.h` are identical (4,179,985 bytes). All three Cuneiform_stable.h copies are identical (818,209 bytes). Encoded/Cuneiform.h is 614,269 bytes and the older reads/Cuneiform.h is 188,176 bytes; both differ. See [key-duplicates.json](audit/key-duplicates.json) for full paths and hashes.

Sutum2025.toe and Sutum2025.77.toe are identical (356,092 bytes); retain one in Git. All other code/TD file hashes, including backups, are recorded in source-files.json. Same filenames and same sizes alone were not treated as duplicate proof.

## Exclusions and incomplete portability

Excluded: personal CV/documents, movies, translation corpora, software installers, PSD/archives, all Backup directories, obsolete full project copies. Background.png (16,524,701 bytes) remains a documented optional/older TD reference, not included. No original was removed. See [TouchDesigner status](../touchdesigner/README.md) before presenting from a fresh clone.

The selected player retains all sibling .ino/.h/.cpp files, including unused stable/choreography headers, so the folder snapshot stays intact. The earlier receiver is included for its materially different mode behavior. Older player variants are inventoried rather than duplicated in Git; they remain in the cloud source.

## Reproduction

Run `python tools/point-analysis/verify_preservation.py --source-root "<cloud project root>"` to compare every preserved file with the manifest and the cloud copy. Without the option it verifies the repository alone. Run the array analyzer using the root README command. `docs/audit/validation.json` records checks performed; it is not a build/test report for the hardware.
