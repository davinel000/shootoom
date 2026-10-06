# Uno firmware versions

## 0.1.0 — 2026-10-06

First stabilization candidate based on ScriberLatestSimplified. Nonblocking framing, packet/point-idle timeouts, explicit error blanking, payload bounds, zero-distance arithmetic, legacy symbol-end support, nonblocking button handling and version/error counters. Built for Uno and checked on host; no upload or hardware validation yet. See [version notes](0.1.0/README.md).

Each published version has its own folder, embedded version string, build record and artifact hashes. Future behavior changes receive a new version; do not overwrite a published version's sources/binary. The legacy snapshot remains unchanged.
