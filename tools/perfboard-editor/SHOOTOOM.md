# Šūtum board viewer

See the [installation instructions](../../docs/SHOOTOOM_GUIDE.md). The guide opens the main v0.8.4 board and the reviewed v0.2.1 TL082 board. The editor loads their JSON models and displays both board faces.

The editor is based on [PCB Perfboard Designer](https://github.com/MaurerKrisztian/PCB_Perfboard_designer) at commit `bb1340bce2e5a5d7fe67a553e0195a5c41acdd9f`. Upstream package metadata declares ISC. The upstream README and attribution are in this directory.

Source layouts are in `hardware/perfboard`. After updating them, run `python tools/build_system_guide.py` from the repository root to refresh the guide and bundled viewer copies.
