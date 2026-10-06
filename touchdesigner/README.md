# TouchDesigner preservation

`legacy/Sutum2025.toe` is the unmodified 356,092-byte snapshot dated 2025-03-13. It is byte-identical to Sutum2025.77.toe, so only one copy is included. It is small enough for ordinary Git; no LFS or large-file exception was needed.

The audit used installed `toeexpand.exe` on a temporary copy without opening/cooking the project. It produced the directory and TOC but returned exit code 1; existence and readable content were checked independently. It does not prove the project opens without errors. The original remains unchanged.

Included: three `IMG READY` images named by the main generator; choreography1.csv; the referenced Liberation, DejaVu and Noto fonts with supplied licenses. Relative folders are preserved beneath `legacy` where possible. `extracted/` contains four DAT script payloads with the 27-byte toeexpand container header removed and no source edits. See hashes/operator paths in `docs/audit/td-extracted.json`; these scripts require the original TD operator context and must not be executed as standalone generators.

## Unresolved dependencies

`docs/audit/td-references.json` lists parameter references. There are stale E:/P: paths to older project directories, a law_module external TOX, old image experiments, and a Background.png. The 16,524,701-byte background was not included in this minimal snapshot; neither were the 99,475,055-byte PSD, archives, old photos or hundreds of backups. Some externaltox/script expressions belong to embedded components and may be inactive; the inventory does not claim each string is a missing runtime dependency.

The modern generator's IMG READY files are included, but a full project-wide path repair was not performed. Main controls and older networks may need external-path reassignment. The exact embedded network already remains in the TOE; no substitute external component was invented. Before presentation, open with hardware disconnected, inspect missing-file/cook errors and active dependencies, then make a separate portable working copy after deciding which historical networks to retain.

Serial DAT is saved as COM13, baud 115200 with expression-controlled active state. The selected legacy Uno receives via SoftwareSerial at 57600 and does not parse USB Serial. Do not expect plugging in USB alone to restore direct streaming.
