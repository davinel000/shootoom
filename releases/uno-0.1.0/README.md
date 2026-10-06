# Uno 0.1.0 release files

Status: **compiled candidate for bench validation, not uploaded or hardware-tested**.

- `Sutum-Uno-0.1.0-source.zip`: complete versioned sketch folder and Russian instructions. Extract, open `Sutum-Uno-0.1.0/LaserShow/LaserShow.ino` in Arduino IDE. Select Arduino Uno; COM15 is the user-reported port, not independently verified here.
- `LaserShow.ino.hex`: application-only compiled binary for ATmega328P Arduino Uno. No bootloader replacement is needed. Arduino IDE's normal Upload builds from the source; this HEX is an archived build artifact.
- `SHA256.json`: exact source and artifact sizes/hashes.
- `build-output.txt`: final compile log including retained warnings.
- `validation.json`: exact build environment and limits of verification.

See [version notes](../../firmware/uno/0.1.0/README.md) and [changelog](../../firmware/uno/CHANGELOG.md). Legacy data and code hashes remain unchanged. Files with bootloader, ELF and EEPROM outputs generated locally by Arduino CLI are not published.

Rebuild from the repository root with `python tools/firmware-checks/build_uno.py --cli <arduino-cli-path> --config <IDE-config-yaml>`. Matching core/library versions are required; the script never opens COM ports or uploads. Rebuilding archives may change ZIP metadata/hashes; retain the published manifest for comparison. New behavior must be published under a new version rather than replacing this version.
