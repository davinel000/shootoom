"""Build the single English hardware guide from the reviewed pin maps.

Does not change either board project, firmware, or browser storage.
"""
from pathlib import Path
import csv
import html
import json
import shutil

ROOT = Path(__file__).resolve().parents[1]
BOARDS = ROOT / 'hardware/perfboard'
audit = json.loads((BOARDS / 'v0.8.4-physical-audit.json').read_text(encoding='utf8'))
pins = {p['pin']: p for p in audit['pins']}
encoder = [('J4.1','CLK','D4 / GPIO5'),('J4.2','DT','D5 / GPIO6'),
           ('J4.3','SW','D6 / GPIO43'),('J4.4','GND','Common ground'),
           ('J4.5','+ / VCC','3.3 V from XIAO')]
led = [('J5.1','GND','Common ground'),('J5.2','IN / DIN','D7 / GPIO44 → AHCT245 → R3 330 Ω'),
       ('J5.3','VCC / +','5 V rail')]
assert [(pins[k]['hole'],pins[k]['net']) for k,_,_ in encoder] == [
    ('J5','ENC_A'),('J6','ENC_B'),('J7','ENC_SW'),('J8','GND'),('J9','+3V3')]
assert [(pins[k]['hole'],pins[k]['net']) for k,_,_ in led] == [
    ('J19','GND'),('J20','LED_DATA'),('J21','+5V')]
analog = list(csv.DictReader((BOARDS/'shootoom-analog-v0.2.1-user-2026-10-29-connections.csv').open(encoding='utf-8-sig')))

def table(head, rows):
    return '<div class="scroll"><table><thead><tr>'+''.join('<th>'+html.escape(x)+'</th>' for x in head)+'</tr></thead><tbody>'+''.join('<tr>'+''.join('<td>'+html.escape(str(x))+'</td>' for x in row)+'</tr>' for row in rows)+'</tbody></table></div>'

encoder_table = table(['Board connector','Board hole','Connect to encoder label','Board route'],
                      [(k,pins[k]['hole'],label,route) for k,label,route in encoder])
led_table = table(['Board connector','Board hole','Connect to ring label','Board route'],
                  [(k,pins[k]['hole'],label,route) for k,label,route in led])
other_table = table(['Connector','Board contacts in order','Function'],[
    ('J1','Upper: GND; lower: +5 V','Regulated low-voltage input, column 3'),
    ('J3','Upper: GND; lower: +5 V','Fan supply, column 17'),
    ('J2','A24: +5 V; A25: switched laser return','Two-wire laser module'),
    ('J7','A20: Gate; A21: Drain; A22: Source/GND','External MOSFET cable; not a universal transistor pinout'),
    ('J6','A27: DAC A/X; A28: GND; A29: DAC B/Y','Signal cable to analog JB2')])
analog_table = table(['Analog connector','Hole','Signal'],[(r['ref']+'.'+r['pin'],r['hole'],r['net']) for r in analog if r['ref'].startswith('JB')])
all_pins = table(['Part / pin','Hole','Net'],[(p['pin'],p['hole'],p['net']) for p in audit['pins']])
all_wires = table(['Wire','Net','From','To'],[(w['wire'],w['net'],w['from'],w['to']) for w in audit['wires']])

connector_svg = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1100 300" role="img" aria-label="Installed connector order on the main board, component face">
<style>text{font:19px system-ui;fill:#172b35}.small{font-size:16px}.contact{fill:#d7b56d;stroke:#172b35;stroke-width:2}.body{fill:#e5e9ed;stroke:#728390;stroke-width:2}</style>
<text x="25" y="30">MAIN BOARD · component face · columns increase left → right</text>
<text x="25" y="68">J4 encoder</text><text x="675" y="68">J5 NeoPixel</text>
<rect class="body" x="20" y="85" width="610" height="95" rx="8"/><rect class="body" x="670" y="85" width="410" height="95" rx="8"/>
'''
for x,hole,label in [(85,'J5','CLK'),(205,'J6','DT'),(325,'J7','SW'),(445,'J8','GND'),(565,'J9','3.3 V'),(740,'J19','GND'),(875,'J20','DIN'),(1010,'J21','5 V')]:
    connector_svg+=f'<circle class="contact" cx="{x}" cy="112" r="11"/><text text-anchor="middle" x="{x}" y="158">{label}</text><text class="small" text-anchor="middle" x="{x}" y="210">{hole}</text>'
connector_svg+='''<text x="25" y="255">↓ Latch / split-wall side faces the lower edge (J edge) on this view.</text>
<text class="small" x="25" y="285">This is the installed board view, NOT the mating-face view of a detached cable plug.</text></svg>'''

text = '''<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Šūtum — Hardware & Firmware Guide</title><style>
:root{color-scheme:light}*{box-sizing:border-box}body{font:17px/1.55 system-ui,sans-serif;margin:0;color:#172b35;background:#f6f8fa}main{max-width:1180px;margin:auto;padding:32px 24px 80px}h1,h2,h3{line-height:1.2}h1{font-size:38px}h2{margin-top:48px;border-top:1px solid #ccd6dc;padding-top:24px}a{color:#005d85}nav{display:flex;gap:12px 24px;flex-wrap:wrap;padding:16px 0}nav a{font-weight:600}table{border-collapse:collapse;width:100%;font-size:15px;background:white}td,th{padding:10px 12px;text-align:left;border:1px solid #ccd6dc;vertical-align:top}th{background:#e7eef3}.scroll{overflow-x:auto}pre{background:#172b35;color:#f4f8fc;padding:18px;border-radius:6px;overflow:auto}code{font-family:ui-monospace,Consolas,monospace}img,svg{max-width:100%;height:auto}.note{border-left:5px solid #b57900;background:#fff2d4;padding:14px 18px}.good{border-left-color:#21704a;background:#e6f3ec}.small{font-size:14px;color:#435969}.card{background:white;border:1px solid #ccd6dc;border-radius:8px;padding:18px;margin:18px 0}summary{cursor:pointer;font-weight:600;padding:12px 0}figure{margin:20px 0}figcaption{font-size:14px;color:#435969}section{scroll-margin-top:16px}.button{display:inline-block;background:#005d85;color:white;padding:9px 14px;border-radius:5px;text-decoration:none;margin:5px 12px 5px 0}@media print{nav,.button{display:none}body{background:white}main{padding:0}details>table{display:table}tr{break-inside:avoid}h2{break-after:avoid}}
</style></head><body><main>
<p class="small">SHOOTOOM / ŠŪTUM · OWNER'S WORKING GUIDE · UPDATED 31 OCTOBER 2026</p>
<h1>Hardware & Firmware Guide</h1>
<p>One entry point for the controller board, analog amplifier, panel cables, firmware and current assembly notes. The board files remain separate because they describe two physically separate PCBs.</p>
<nav><a href="#connectors">Encoder & ring</a><a href="#main">Main board</a><a href="#analog">TL082 board</a><a href="#system">Signal chain</a><a href="#firmware">Firmware</a><a href="#case">Feet & mount</a><a href="#evidence">Evidence & files</a></nav>
<div class="note good"><strong>Current hardware:</strong> XIAO ESP32-S3 Plus + AHCT245 + MCP4822 on main board v0.8.4; one TL082 on the separate 17-column analog board. The owner reports a working square and full show on 0.1.0. Firmware 0.1.5 provides Show / Stone / Frame + F with common live settings and explicit saving. The agent has not uploaded or optically tested it.</div>

<section id="connectors"><h2>1. Encoder and NeoPixel cables</h2>
<p><strong>Use the MAIN board, not the TL082 board.</strong> Look at the component bodies: XIAO on the left with USB pointing left, AHCT245 in the middle, MCP4822 on the right. In this view <strong>A is at the top, J at the bottom; columns 1…30 run left to right</strong>. The white connector latch / split-wall side points down towards the J edge, matching the owner's description.</p>
<p><strong>Connector references and holes are different names:</strong> J4 is the encoder connector, while J5 is also the name of its leftmost board hole. J5 is the NeoPixel connector reference, but its holes are J19…J21.</p>
{{CONNECTOR_SVG}}
<h3>J4 · five-pin encoder · left to right</h3>{{ENCODER_TABLE}}
<p>The cable order is <strong>CLK — DT — SW — GND — +3.3 V</strong>. SW is the push-button contact (sometimes misread as SV). CLK and DT are the two rotation channels. Use the actual labels on the encoder module; the physical order on that module need not match the order on our board.</p>
<div class="note"><strong>The encoder supply is 3.3 V, not 5 V.</strong> Its three signal lines go directly to the XIAO inputs, without the AHCT level buffer. This is the board's intended supply for a compatible passive encoder module. The exact module model has not been documented; do not assume a module requiring 5 V is compatible.</div>
<h3>J5 · three-pin NeoPixel · left to right</h3>{{LED_TABLE}}
<p>The cable order is <strong>GND — DIN — +5 V</strong>. Connect to the ring's <strong>IN/DIN</strong> side, not OUT/DOUT. Ring data already passes through the AHCT245 and the existing R3 330 Ω on the main board. Do not add another resistor just because the cable is being assembled.</p>
<h3>Before applying power</h3>
<p>Disconnect external power and USB before re-pinning cables. With each cable plugged into the unpowered board, confirm continuity from the label on the peripheral to the relevant board strip. For accessible test pads use <strong>F5/CLK, F6/DT, F7/SW, F8/GND, F9/3.3 V</strong> and <strong>G19/GND, G20/DIN, G21/5 V</strong>. These pads share factory copper with the connector holes; do not push probes into occupied solder joints.</p>
<p>Reversing the way you look at a detached cable plug reverses apparent left/right. The small arrows/holes in the plastic do not prove electrical polarity. Trace the wires by continuity, not colour. This guide's drawing always shows the <em>installed connector on the component face</em>.</p>
<p><strong>Firmware 0.1.5: the same controls in all three programs.</strong> Short click cycles the menu; turn edits the selected value; hold 1.2 seconds and release to play/pause/resume; hold 3.5 seconds and release to save. The ring uses GRB, initially 12 pixels. See <a href="#firmware">operation below</a>.</p></section>

<section id="main"><h2>2. Main controller board · v0.8.4</h2>
<a class="button" href="./?project=shootoom-physical-v0.8.4.json">Open main board in editor</a>
<a class="button" href="shootoom-v0.8.4-guide.html">Detailed assembly guide</a>
<figure><a href="shootoom-v0.8.4-cases.png"><img loading="lazy" src="shootoom-v0.8.4-cases.png" alt="Main board component face: A top, J bottom, XIAO left"></a><figcaption>Component face: A top, J bottom. The original drawing's component labels are retained.</figcaption></figure>
<details><summary>Main board solder / printed face · J top, A bottom</summary><img loading="lazy" src="shootoom-v0.8.4-wiring.png" alt="Main board printed solder face: J top, A bottom"></details>
<p>Flip top-to-bottom around a horizontal axis to get the other face; left/right columns stay fixed under this specific flip. Do not transfer the row orientation from the amplifier board by memory.</p>
{{OTHER_TABLE}}
<h3>Main board parts already in the design</h3>
<p>XIAO ESP32-S3 Plus; AHCT245 DIP20; MCP4822 DIP8; R1 1 kΩ (laser gate series), R2 10 kΩ (control pull-down), R3 330 Ω (ring data series), R4 10 kΩ (external MOSFET gate-to-source); C1/C2 100 nF ceramic supply decoupling; D1 Schottky supply diode (owner fitted 1N5819). The diode's band faces B1 / XIAO 5V. The MOSFET is outside this board; its physical lead order must match its own part, not the connector's project labels.</p>
<details><summary>Complete main-board pin / passive list from the checked model</summary>{{ALL_PINS}}</details>
<details><summary>All 29 main-board jumpers from the checked model</summary><p>TOP/BOTTOM in rail endpoint names are legacy model IDs, not a change to the face orientation stated above. Use the board picture and named holes when locating physical pads.</p>{{ALL_WIRES}}</details></section>

<section id="analog"><h2>3. Separate analog board · one TL082</h2>
<a class="button" href="./?project=shootoom-analog-v0.2.1-user-2026-10-29.json">Open reviewed owner layout</a>
<a class="button" href="shootoom-analog-v0.2.1-user-2026-10-29-guide.html">Detailed analog guide</a>
<p><strong>17 columns, 1…17 left to right. Printed/component face: J at the TOP, A at the BOTTOM.</strong> Each column's J…F five pads are connected; E…A form a separate connected group. The gap is between F and E. TL082 notch points left.</p>
<figure><a href="shootoom-analog-v0.2.1-user-2026-10-29-cases.png"><img loading="lazy" src="shootoom-analog-v0.2.1-user-2026-10-29-cases.png" alt="Separate analog board, printed component face: J top, A bottom"></a><figcaption>Reviewed owner layout, 29 October. R6 is drawn as one nominal 2.7 kΩ part; the actual assembly uses a series pair, noted below.</figcaption></figure>
<details><summary>Analog solder face</summary><img loading="lazy" src="shootoom-analog-v0.2.1-user-2026-10-29-wiring.png" alt="Analog board opposite face"></details>
{{ANALOG_TABLE}}
<p>Main <strong>A27 → analog J7</strong>, main <strong>A28 → analog J8</strong>, main <strong>A29 → analog J9</strong>: DAC A/X, common ground, DAC B/Y. These are the design correspondences. The owner skipped the last requested cable continuity check before connecting and reported working output; do not record that skipped check as passed.</p>
<p>Analog parts: R1/R3 10 kΩ; R2/R4/R5 47 kΩ; R6 nominal 2.7 kΩ. <strong>Actual R6: 2.2 kΩ + 470 Ω in series (2.67 kΩ nominal, about 2.66 kΩ measured)</strong>, with the owner's isolated intermediate node in column 17. C1/C2/C3 are 100 nF ceramic. That is six resistor references, but seven physical resistors with the R6 pair.</p>
<h3>Electrical topology</h3>
<p>Each channel is an inverting amplifier: DAC input → 10 kΩ → inverting input; output → 47 kΩ feedback → inverting input. Both non-inverting inputs share VREF. VREF comes from +15 V through 47 kΩ, then R6 to 0 V. Nominal gain magnitude is 4.7, so <code>Vout = 5.7 × VREF − 4.7 × VDAC</code>. This is a fixed-resistor adaptation of the reference, not a claim that the burned old module had identical wiring.</p>
<details><summary>Analog circuit drawing · nominal R6 2.7 kΩ</summary><img loading="lazy" src="shootoom-analog-v0.2.1-user-2026-10-29-circuit.svg" alt="TL082 two-channel electrical schematic, nominal resistor values"><p>The existing schematic retains Russian labels and nominal R6; use the series-pair note above for the as-built value.</p></details>
<p>Owner-reported measurements: F10 +15 V, E13 −15 V relative to A15; VREF F13 +0.79 V; open signal-input outputs E10/F11 about +0.80 V. Main DAC at idle about 1.02 V on each output. These are individual observations, not a measured full-range calibration. The earlier burned/cracked TL082 incident remains in the repository; its exact cause was not established.</p></section>

<section id="system"><h2>4. Signal and power chain</h2>
<pre>Embedded show data → XIAO ESP32-S3 Plus
  SPI + paired LDAC → AHCT245 (5 V supply) → MCP4822
  MCP4822 A/B + GND → main J6 → analog JB2
  TL082 (±15 V supply) → analog JB1 → existing XY galvo driver

  GPIO1 / D0 → AHCT245 → R1 → external MOSFET Gate
  5 V → laser module → MOSFET Drain; Source → common GND

  Encoder CLK/DT/SW → XIAO GPIO5/6/43 (3.3 V logic)
  XIAO GPIO44 → AHCT245 → R3 330 Ω → ring DIN
  5 V rail → ring VCC and fan; 3.3 V → encoder supply</pre>
<p>The 5 V logic supply and the bipolar ±15 V amplifier supply are different rails. Their common signal reference is 0 V/GND; −15 V is not GND. Main J6 carries signal and ground only; it does not supply the amplifier. This guide does not specify or approve the internal 230 V mains wiring.</p>
<p>MCP4822 is driven at gain 1× in the current sketch: its nominal 12-bit range is 0…approximately 2.048 V, not 0…5 V despite its 5 V supply. Neither those voltage endpoints nor software clipping establish acceptable galvo travel.</p></section>

<section id="firmware"><h2>5. Autonomous firmware · 0.1.5</h2>
<p><strong>New candidate; no hardware upload or optical test by the agent.</strong> Open this version in Arduino IDE: XIAO ESP32-S3 Plus, USB CDC On Boot Enabled. Upload yourself.</p>
<pre>firmware/esp32-s3/0.1.5/ShootoomS3/ShootoomS3.ino</pre>
<h3>One set of controls everywhere</h3>
<table><tr><th>Gesture</th><th>Result in every program, running or paused</th></tr>
<tr><td>Short click and release</td><td>Next menu field; playback position retained</td></tr>
<tr><td>Turn</td><td>Edit that field live</td></tr>
<tr><td>Hold 1.2–3.5 s, release</td><td>Play / pause / resume</td></tr>
<tr><td>Hold at least 3.5 s, release</td><td>Save current settings + default program; preserve playing/paused state</td></tr></table>
<p><strong>To start or pause: hold about two seconds, then release.</strong> Holding beyond 3.5 seconds saves instead. These thresholds are unchanged; the cause of the reported unreliable start has not been established.</p>
<p>Button-down blanks and suspends playback. Actions happen <strong>on release</strong>, so a long Save hold cannot first trigger Play. There is no separate settings mode and no automatic save. A short click on a fault acknowledges it and stays paused.</p>
<table><tr><th>Menu colour, in order</th><th>Setting</th></tr>
<tr><td>White</td><td>Three adjacent markers: Show / Stone / Frame + F</td></tr>
<tr><td>Blue</td><td>Scale 5…100%, 1 percentage point per detent</td></tr>
<tr><td>Purple</td><td>Eight adjacent swap/reflection variants</td></tr>
<tr><td>Cyan</td><td>Angular rotation 0°, 45° … 315°, one 45° step per detent; eight adjacent cyan markers</td></tr>
<tr><td>Pink</td><td>Drawing speed 25…400%, steps of 25 percentage points</td></tr>
<tr><td>Amber</td><td>0…60 seconds between cycles of this program</td></tr>
<tr><td>Green</td><td>Autorun: positive turn = all green / ON; negative = one green / OFF</td></tr></table>
<p>The final LED is green while playing/waiting for repeat, amber while paused. Other LEDs show the current field; the Autorun field uses the whole ring. White flash confirms save; red blinking indicates a software fault. Exact values are available in USB <code>status</code>.</p>
<h3>Programs and live settings</h3>
<p><strong>Show</strong>: symbols, lettering and scripted pauses only; no stone contour at startup or at cycle repeats. <strong>Stone</strong>: only the stone outline, repeated. <strong>Frame + F</strong>: full digital square with the large F retained as an orientation mark. Separate small square/F modes are removed. Frame has no old 200 ms parking pause or calibration timeout; relocations between strokes are dark.</p>
<p>Scale/orientation/rotation/speed are shared. Each program has its own gap: initially Show 10 s, Stone 0 s, Frame 0 s. <strong>Gap 0 now means continuous repeat.</strong> Speed changes drawing/movement timing, including lettering, but not scripted scene pauses. Geometry changes blank and realign at the preceding vertex; pausing does not reset Show.</p>
<p>At angle 0°, scale 1.0, the frame spans codes 1…4095, as before. The eight rotation positions fit the square inside the same digital canvas: at 45°, requested scale 1.0 applies about 0.707 to avoid clipping corners; at 0°/90° it applies 1.0. <code>status</code> shows both scales. This new angular fitting does not explain size observations made on earlier firmware.</p>
<h3>Save and exhibition startup</h3>
<ol><li>Select the desired startup program on white and adjust its settings.</li><li>On green, turn positive for all green / autorun ON.</li><li>Hold at least 3.5 s until white, then release to save.</li><li>Next power-up: five-second amber countdown, then the saved program. Show starts immediately with symbols.</li></ol>
<p>Press/turn cancels the countdown; holding the button at power-on suppresses it. Set one green / OFF and save to disable future autorun. Saving writes NVS only while blanked and preserves playback state. Without saving, changes are RAM-only. On first import of an older profile, scale resets to 1.0 and angle to 0°; orientation and other settings are retained. After you save a 0.1.5 profile, your chosen scale/angle are restored normally. Old diagnostic selections from version-1 profiles still migrate to Frame with autorun OFF.</p>
<h3>USB monitor and size investigation</h3>
<p>Serial Monitor: 115200 / Newline. <code>status</code>, <code>monitor on/off</code>, <code>diag</code>. The last command gives commanded lit DAC coordinate bounds and nominal calculated voltage spans; it cannot measure actual analog voltage, light output or galvo position.</p>
<p>For USB playback send, separately: <code>stop</code>, <code>select stone</code>, <code>arm</code>, <code>stone</code> within ten seconds. Use <code>show</code> or <code>frame</code> for other programs. <code>pause</code> preserves position; <code>arm</code> then <code>resume</code> resumes. <code>stop</code> / <code>!</code> deliberately reset. Live commands: <code>scale</code>, <code>orient</code>, <code>angle</code>, <code>speed</code>, <code>gap</code>, <code>boot auto/manual</code>, <code>save</code>. Wi-Fi/Bluetooth are not enabled.</p>
<p>DAC gain remains 1×, matching the preserved Uno receiver. The present 47k/10k analog ratio is 4.7×; nominal full DAC span is 2.0475 Vpp, hence calculated analog span 9.623 Vpp. This is not a physical measurement or evidence of the old amplifier gain. The cause of the reported smaller projection remains unresolved until the current profile and a comparable Stone width are supplied.</p>
<p>Full instructions: <code>firmware/esp32-s3/0.1.5/README.md</code>; evidence: <code>firmware/esp32-s3/0.1.5/BUILD.md</code>; size investigation: <code>hardware/PROJECTION_SIZE_2026-10-30.md</code>.</p></section>

<section id="case"><h2>6. Feet and tripod attachment</h2>
<p><a href="https://www.amazon.de/dp/B0DVZPNF4R">3M SJ5017 adhesive feet on Amazon.de</a>: listing found for 40 pieces, Ø19 mm × 9.6 mm high. The actual dimensions are also stated by <a href="https://www.3mdeutschland.de/3M/de_DE/p/d/b5005035174/">3M</a>. Use four feet on clean, flat areas near the corners. Adhesion to this enclosure material has not been tested. The accessed Amazon session was set to the United States, so German delivery date and final price were not verified.</p>
<p><a href="https://www.amazon.de/dp/B0D5H9FTCF">SmallRig 4486 HawkLock / Arca kit on Amazon.de</a>: a compact candidate with two 1/4-20 screws on the top plate and a removable clamp. Listed kit dimensions are 54.6 × 41 × 23 mm; this is the <strong>complete kit</strong>, not the thickness of the top plate. The lower clamp can remain on the tripod. The clamp also has two underside 1/4-20 holes 18 mm apart. Current price and German delivery were not verified.</p>
<p>Two fasteners through the housing into a suitable internal backing plate or wide washers are the proposed anti-rotation attachment. Check body dimensions, clearances and the actual plate screw spacing before drilling; fastener length and enclosure load capacity are not established. Keep all internal hardware clear of wiring and boards. The complete mount arrangement depends on the existing tripod head, which has not been identified.</p>
<p><strong>Feet must extend below the plate left on the housing.</strong> A 9.6 mm foot is not enough to clear the whole 23 mm kit. Do not choose the feet solely by the catalog height without deciding which plate parts remain on the case.</p></section>

<section id="evidence"><h2>7. Files, evidence and status</h2>
<p>Run <code>Start Board Viewer.cmd</code> from the repository root to open this guide and start the local viewer if needed. The separate board editors use the same viewer. This guide does not edit browser local storage or overwrite board projects.</p>
<ul><li>Main source: <a href="shootoom-physical-v0.8.4.json">v0.8.4 JSON</a>; <a href="v0.8.4-physical-audit.json">checked physical pin/net map</a>.</li>
<li>Analog source: <a href="shootoom-analog-v0.2.1-user-2026-10-29.json">reviewed owner JSON</a>; <a href="shootoom-analog-v0.2.1-user-2026-10-29-connections.csv">connections CSV</a>; <a href="shootoom-analog-v0.2.1-user-2026-10-29-jumpers.csv">jumper CSV</a>.</li>
<li>Firmware instructions: <code>firmware/esp32-s3/0.1.5/README.md</code>; evidence: <code>firmware/esp32-s3/0.1.5/BUILD.md</code>.</li>
<li>Actual measurements and incident history: <code>hardware/ANALOG_BRINGUP_2026-10-29.md</code> and <code>hardware/INCIDENT_2026-10-27_TL082.md</code>.</li>
<li>Current encoder/LED connector tables are generated from the v0.8.4 checked pin map; fixed assertions check the expected holes and nets. Model verification is distinct from physical cable testing.</li></ul>
<p>Primary component references: <a href="https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/">Seeed XIAO S3/Plus</a> · <a href="https://raw.githubusercontent.com/espressif/arduino-esp32/3.3.7/variants/XIAO_ESP32S3/pins_arduino.h">ESP32 3.3.7 XIAO pin aliases (D4/5/6/7 = GPIO5/6/43/44)</a> · <a href="https://www.ti.com/lit/ds/symlink/sn74ahct245.pdf">TI AHCT245</a> · <a href="https://ww1.microchip.com/downloads/en/devicedoc/20002249b.pdf">Microchip MCP4822</a> · <a href="https://www.unisonic.com.tw/uploadfiles/836/part_no_pdf/TL082.pdf">UTC TL082</a> · <a href="https://www.instructables.com/Arduino-Laser-Show-With-Real-Galvos/">original build reference</a>.</p>
</section></main></body></html>'''
for key,value in dict(CONNECTOR_SVG=connector_svg,ENCODER_TABLE=encoder_table,LED_TABLE=led_table,
                      OTHER_TABLE=other_table,ANALOG_TABLE=analog_table,ALL_PINS=all_pins,ALL_WIRES=all_wires).items():
    text=text.replace('{{'+key+'}}',value)
assert '{{' not in text
while chr(92)*2 in text: text=text.replace(chr(92)*2,chr(92)) # Display Windows paths.
out=BOARDS/'shootoom-system-guide.html'
out.write_text(text,encoding='utf8')
linked_files=['shootoom-system-guide.html','v0.8.4-physical-audit.json',
              'shootoom-physical-v0.8.4.json','shootoom-v0.8.4-guide.html',
              'shootoom-v0.8.4-cases.png','shootoom-v0.8.4-wiring.png','shootoom-v0.8.4-r4-detail.svg',
              'shootoom-analog-v0.2.1-user-2026-10-29.json',
              'shootoom-analog-v0.2.1-user-2026-10-29-guide.html',
              'shootoom-analog-v0.2.1-user-2026-10-29-cases.png',
              'shootoom-analog-v0.2.1-user-2026-10-29-wiring.png',
              'shootoom-analog-v0.2.1-user-2026-10-29-circuit.svg',
              'shootoom-analog-v0.2.1-user-2026-10-29-connections.csv',
              'shootoom-analog-v0.2.1-user-2026-10-29-jumpers.csv']
for folder in (ROOT/'tools/perfboard-editor/assets',ROOT/'tools/perfboard-editor/dist'):
    if not folder.is_dir():
        continue
    for name in linked_files:
        shutil.copyfile(BOARDS/name,folder/name)
print('Built English system guide; verified 8 encoder/ring pin assignments; copied linked resources to viewer.')
