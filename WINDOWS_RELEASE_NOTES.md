# CivicLight Windows CPU Miner v2.1.0

This NitroPool Performance Edition release delivers a major CivicLight v2 CPU
mining performance update.

## Release highlights

- Added persistent per-thread CivicLight workspaces, eliminating allocation and
  release of the memory-hard workspace for every nonce.
- Changed the miner identity to `cpuminer-nitro-2.1.0-x64W`.
- Added the NitroPool Performance Edition startup banner.
- Added a native Windows automatic CPU launcher.
- Added safe operating-system state checks for AVX, AVX2 and AVX-512.
- Added automatic fallback when a preferred optimized binary is missing.
- Added manual compatible-build selection for testing and troubleshooting.
- Added six optimized x86-64 builds: Core2, AVX, AVX2, AVX2 with SHA-NI,
  AVX-512 and AVX-512 with SHA-NI.
- Added a reproducible Linux-to-Windows MinGW release container.
- Added a Windows quick-start batch file and dedicated documentation.
- Corrected the miner banner so Windows builds identify themselves as Windows.
- No developer fee.

## Testing commands

Display detected capabilities and packaged binaries:

```powershell
.\civiclight-miner.exe --list-cpu-builds
```

Test automatic selection without starting the miner:

```powershell
.\civiclight-miner.exe --launcher-dry-run
```

Live mining and accepted-share testing should be completed on representative
Intel and AMD systems before publishing the final binary release.
