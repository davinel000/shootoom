# Legacy UART protocol

Evidence: selected player's CommandEncoder.h/.cpp and SymbolSender.h; selected receiver's CommandParser.cpp and Drawing.cpp. Link: Serial1 ↔ SoftwareSerial(D8,D9), 57600 baud. Default framing assumed from unparameterized `begin`: verify with the eventual core/logic-analyzer capture. Debug Serial is separate, 19200.

Packet: `FF command length payload... FE`. Length is one byte and counts payload only. Multi-byte coordinates are big-endian. There is no CRC, sequence number or escaping; FF/FE inside a correctly length-delimited payload are legal. Parser buffer is 64 bytes; accepted lengths are 1–64.

| ID | Payload | Actual behavior |
| --- | --- | --- |
| 01 | `N text[N] repeat rotation Xhi Xlo Yhi Ylo scale speed` | N+9 bytes. Receiver draws text synchronously, then writes AC to mySerial |
| 02 | `Xhi Xlo Yhi Ylo` | Exactly 4 bytes; X bit 15 is laser-on, X & 7FFF is coordinate, Y is unsigned 16-bit; offset reset to zero |
| 03 receiver | `count scale` | Validates parameters but calls fixed grid parameters `(5,5,30,1,1)` |
| 03 sender | empty | Player's symbol-end marker. Zero length is rejected by receiver, so it has no symbol-end effect |
| 04 TD | empty | TD's laser-off command. Also rejected by receiver; no handler |

Text scale byte is approximately `round(scale*100)` in the player; receiver uses `((b-5)/195.0)*1.95+0.05`. Rotation, repeat and speed are bytes; positions are unsigned 16-bit. Receiver subtracts 1950 from text positions. With a 64-byte payload, text must be at most 55 bytes, but neither sender nor receiver adequately enforces the semantic length. The receiver trusts data[0] and indexes/copies without checking `length == N+9`, so malformed input can read beyond the buffer.

Only text commands are acknowledged. `sendTextCommand()` waits up to 9000 ms for AC; timeout logs a warning and proceeds, without retry or clearing ambiguous late acknowledgements. Points have only sender pacing. Zero-length symbol-end packets are not acknowledged. `sendSymbolSubdiv()` explicitly sends an off point at the last coordinate; that point, not its end marker, blanks the output. `sendSymbolAndFillEx()` ends fill with only the ineffective marker, so its last point can leave the laser on (helper not called by the selected fulltexts loop).

The receiver's payload loop waits indefinitely for missing bytes. There is no receive timeout or communications-loss blanking. A disconnected sender after an on-point can leave the last state active. A zero-length packet resets parsing before consuming FE as a terminator; the next byte is processed afresh.

Reproduce by source inspection first. For a future bench test with laser output disabled, capture a valid point packet and text acknowledgement, then truncate a payload and verify lack of recovery in the legacy implementation. Do not send deliberately malformed text to an illuminated installation.

TD snapshot is configured for COM13 / 115200 and sends to `serial1`; the selected Uno's USB/debug input is not parsed in its loop. Direct TD streaming is therefore **not configured to work with this receiver as preserved**, even though packet layouts partly match.
