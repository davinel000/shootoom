# Findings and validation order

These are source findings; none is a claim that a particular physical failure was reproduced.

| Priority | Finding | Evidence / validation |
| --- | --- | --- |
| High | Missing UART payload bytes can block indefinitely, retaining output state | CommandParser.cpp processIncomingCommands inner wait; inspect/capture truncated packet with laser disabled |
| High | No communications-loss blanking; TD off command is rejected | Parser accepts only positive length; command 04 absent; disconnect after off-state bench setup |
| High | Division by zero on identical/short moves | Laser.cpp sendtoRaw; reproduce diffx calculation for a zero or one-unit move |
| High | Text length permits out-of-bounds reads | executeCommand 01 trusts N without validating N+9 against actual payload |
| Medium | Player symbol-end 03 collides with receiver grid 03 | CommandEncoder.h versus CommandParser.cpp; zero-length is currently discarded |
| Medium | Text timeout proceeds without recovery and late ACKs are ambiguous | sendTextCommand / waitForDrawCompletion, 9000 ms; measure worst-case text duration |
| Medium | Repeated segment endpoints plus two interpolation stages | LineSubdivider, SymbolSender, Laser::sendtoRaw; array report |
| Medium | Text speed affects later outlines | drawStringDetail -> setSpeed, direct-point path leaves speed unchanged |
| Medium | Button handling blocks; ISR and polling coexist | controller.update waits for switch release; selected sketch attaches ISRs but no longer consumes buttonFlag/encoderPos |
| Medium | handleModes runs twice; NeoPixel updates compete with time-sensitive reception | LaserShow.ino; actual interrupt interaction requires installed library source/version and trace |
| Medium | Transition wait occurs before blanking off | Laser::off retains HIGH for 1500 us; scope D5 vs LDAC |
| Medium | Fill helper may finish illuminated | sendSymbolAndFillEx lacks final off-point; not used in chosen fulltexts loop |
| Medium | TD serial route/settings mismatch | TOE serial1 is COM13/115200; candidate receives mySerial/57600, USB Serial is debug |
| Unknown | Last flashed binary, board/core versions, buildability and analog limits | Need physical labels, compile logs and measurements |

Receiver cleanup also removed drawString and other definitions while retaining declarations and an unused caller. Verify with the real linker; do not label it a confirmed build error from grep alone. DAC `beginTransaction` is opened once and never ended; dedicated SPI may work, but adding shared peripherals needs explicit transaction boundaries. Local driver `write` latches when autoLatch=false, a semantic change from upstream (current setup uses true).

Smallest next software work: compile the preserved pair with confirmed targets, then isolate nonblocking packet reception/timeout blanking, payload validation and zero-distance arithmetic fixes on a new branch. Preserve this snapshot for comparison. No timing constants, artwork, live firmware or hardware were changed during the audit.
