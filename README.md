# Shootoom / Sutum / Hammurapi

Preservation snapshot of the existing galvo installation, audited on 2026-10-05.
Legacy firmware is copied byte-for-byte; this is **not yet a verified build or a confirmed dump of the installed firmware**.

Start with [the audit](docs/INVENTORY.md), [current system](docs/CURRENT_SYSTEM.md), and [known issues](docs/KNOWN_ISSUES.md).

| Location | Purpose |
| --- | --- |
| `firmware/legacy/uno-candidate/LaserShow/` | ScriberLatestSimplified receiver candidate |
| `firmware/legacy/rp2040-candidate/HammurapireadsEncoded_fulltexts/` | Latest dated player candidate, including exact headers |
| `firmware/legacy/alternates/2025-02-25/LaserShow/` | Earlier receiver; different startup modes |
| `touchdesigner/legacy/` | Unmodified Sutum2025.toe, primary artwork, selected fonts/licenses and choreography |
| `touchdesigner/extracted/` | Four readable DAT scripts extracted from that TOE, for review |
| `tools/legacy-generators/` | Original Python conversion experiments, unchanged |
| `tools/point-analysis/` | Read-only array/density analysis |
| `docs/audit/` | Source inventory, SHA-256 manifests, comparisons and measured point statistics |
| `reference/` | Upstream provenance, license and source differences |
| `hardware/`, `firmware/esp32-s3/` | Identification checklist and migration experiment specification |

## Reproduce the array audit

From the repository root, using Python 3 (no third-party packages):

```sh
python tools/point-analysis/analyze.py firmware/legacy/rp2040-candidate/HammurapireadsEncoded_fulltexts/Cuneiform.h --steps 5 --delay-ms 6 --output docs/audit/points-fulltexts.json
```

Open the `.ino` inside its same-named folder in Arduino IDE. Before compiling, confirm the board/core and library versions described in [CURRENT_SYSTEM](docs/CURRENT_SYSTEM.md). Do not upload these snapshots merely to identify what is currently installed.

The TouchDesigner file is an archival source, with unresolved old external paths and serial settings; see [its readme](touchdesigner/README.md). Firmware playback uses embedded arrays and does not require TouchDesigner at runtime.

No original cloud files have been moved, removed or rewritten. Personal documents, movies, backups, translation corpora, installers and PSD archives are excluded. Third-party licensing is recorded in [reference/upstream.md](reference/upstream.md); no new blanket license is asserted for the artwork or local contributions.
