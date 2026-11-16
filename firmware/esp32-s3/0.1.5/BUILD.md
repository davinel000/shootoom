# 0.1.5 verification — 31 October 2026

No device upload, serial command to the controller or optical test was performed by the agent.

## Firmware builds

Arduino CLI 1.3.1; FQBN `esp32:esp32:XIAO_ESP32S3_Plus:CDCOnBoot=cdc`.

| Core | Flash / 2097152 bytes | Global RAM / 327680 bytes | Result |
| --- | ---: | ---: | --- |
| Isolated stable 3.3.7 | 512396 | 26512 | PASS |
| Installed 4.0.0-alpha1 | 496469 | 26908 | PASS |

The installed IDE/core was not changed. The stable core ran in an isolated local Arduino CLI installation. Both builds' copied headers match the current release byte-for-byte after stripping the CLI-added `#line` directive. Source/binary hashes are in [build-verification.json](build-verification.json).

- Stable application SHA256: `9e324c2831efd088f865c185c410931c2d5c0efcf52240815e84563cca2b0370`
- Installed alpha SHA256: `447992456b67fe0eaa60ee0cc57e1b05e8f4550ee95f74769a07875dbfa2990d`

Reproduce in PowerShell from the repository root:

```powershell
$cli = 'C:/Program Files/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe'
& $cli --config-file "$env:LOCALAPPDATA/Temp/shootoom-arduino-3.3.7/arduino-cli.yaml" compile --fqbn 'esp32:esp32:XIAO_ESP32S3_Plus:CDCOnBoot=cdc' --build-path "$env:LOCALAPPDATA/Temp/shootoom-s3-0.1.5-stable-build" firmware/esp32-s3/0.1.5/ShootoomS3
& $cli --config-file "$env:USERPROFILE/.arduinoIDE/arduino-cli.yaml" compile --fqbn 'esp32:esp32:XIAO_ESP32S3_Plus:CDCOnBoot=cdc' --build-path "$env:LOCALAPPDATA/Temp/shootoom-s3-0.1.5-alpha-build" firmware/esp32-s3/0.1.5/ShootoomS3
python tools/check_s3_v015.py
```

## Native actual-logic checks

`tools/check_s3_v015.cpp` passed with MSVC 14.44.35207 / C++17 /O2 /W4. No Arduino/SPI/RMT/NVS substitutes are used. Synthetic time checks scheduling, not real hardware. The five inherited int-to-float warnings in ShowPlayer and one conversion warning in its native expected-coordinate assertion remain; there were no compiler errors.

Checks cover default scale 1.0 / angle 0°, old valid version-1/version-2 profile conversion, invalid checksum rejection, restored version-3 settings without resetting, rejection of angles outside the 45° grid, eight forward/reverse detents with wrap, adjacent cyan markers, fit at 45°, existing motion blanking/timing checks, common pause/resume, per-program gaps, boot cancellation and retained Frame + F.

Show starts at artwork step index 1, including after reset/repeat; Stone starts at contour index 0 and emits 626 targets. Full exhibition Show emits **1386626 targets**. During development the playback stream was enumerated with both 0.1.4 and 0.1.5. Comparing the full stream after the former contour intro gave **1386626 targets, FNV-1a64 `1f37a64b8575ded8` in both**. The contour was removed from Show without changing its remaining artwork sequence.

Reproduce from a Visual Studio Developer Command Prompt in the repository root:

```bat
cl /nologo /EHsc /std:c++17 /O2 /W4 tools\check_s3_v015.cpp /Fo:"%TEMP%\shootoom-check-v015.obj" /Fe:"%TEMP%\shootoom-check-v015.exe"
"%TEMP%\shootoom-check-v015.exe"
```

GPIO audit passed for all nine routes. ShowData, FontData, Geometry, Motion, Input, Frame, Transform, Ring and the provenance manifest are byte-identical to 0.1.4. Thus this version does not claim a fix for the reported unreliable start: button decoding and gesture thresholds are unchanged. Physical release timing/status still need owner observation if it persists.

The unified guide was regenerated and rendered from the local HTML because the localhost viewer was not running. Its 14 local linked/image resources exist; no horizontal overflow at 390 px. This verifies documentation, not the board.

Owner acceptance: install this version, read `status`, check first-import `scale=1.000` / `angle=0deg`, eight rotation steps, preserved F, no stone contour in Show, and explicit save/restore across a real power cycle. Real NVS, RMT signal quality, galvo/laser output and the earlier projection-size question remain physically unverified by the agent.
