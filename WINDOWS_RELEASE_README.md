# CivicLight Windows CPU Miner

An optimized 64-bit Windows CPU miner for the **CivicLight** proof-of-work
algorithm used by CivicNet.

This release includes six CPU-specific miner binaries and a native automatic
launcher. The launcher checks the CPU instructions and the SIMD state enabled
by Windows before selecting the fastest safe build.

- Windows x86-64
- Native automatic CPU detection
- Intel and AMD support
- Configurable thread count
- No developer fee

## Download and extract

Download the latest Windows ZIP from:

[CivicLight CPU Miner releases](https://github.com/NitroPoolNet/Civiclight-CPU-Miner/releases/latest)

Extract the complete ZIP before running it. Do not run the miner from inside
the compressed archive, and keep the `bin` directory beside
`civiclight-miner.exe`.

## Quick start

1. Right-click `start-mining.bat` and select **Edit**.
2. Replace `YOUR_CIVIC_ADDRESS` with your CivicNet wallet address.
3. Save the file and double-click `start-mining.bat`.

The included file connects to the NitroPool Australia endpoint. Change `au` to
`eu` or `us` if another region is closer.

## Start from Command Prompt or PowerShell

Replace `YOUR_CIVIC_ADDRESS` with your CivicNet wallet address. Adding
`.worker` is optional but helps identify individual miners.

### Australia

```powershell
.\civiclight-miner.exe -a civiclight -o stratum+tcp://au.nitropool.net:3032 -u YOUR_CIVIC_ADDRESS.worker -p x
```

### Europe

```powershell
.\civiclight-miner.exe -a civiclight -o stratum+tcp://eu.nitropool.net:3032 -u YOUR_CIVIC_ADDRESS.worker -p x
```

### United States

```powershell
.\civiclight-miner.exe -a civiclight -o stratum+tcp://us.nitropool.net:3032 -u YOUR_CIVIC_ADDRESS.worker -p x
```

Use `-t NUMBER` to limit the number of mining threads. For example, add `-t 10`
to use ten threads.

## Automatic CPU selection

Always start mining through:

```powershell
.\civiclight-miner.exe
```

The launcher selects from these bundled builds:

| Build | Required capability |
|---|---|
| `core2` | Core 2-class SSE2 and SSSE3 baseline |
| `avx` | AVX and AES |
| `avx2` | AVX2, AES, BMI and FMA |
| `avx2-sha` | AVX2 with SHA-NI |
| `avx512` | AVX-512 |
| `avx512-sha` | AVX-512 with SHA-NI |

The launcher validates both CPU support and Windows operating-system support
for AVX register state. If the best compatible binary is missing, it safely
falls back to the next compatible bundled build.

To inspect detection without starting the miner:

```powershell
.\civiclight-miner.exe --list-cpu-builds
.\civiclight-miner.exe --launcher-dry-run
```

To select a compatible lower build manually:

```powershell
.\civiclight-miner.exe --cpu-build avx2 -a civiclight -o stratum+tcp://au.nitropool.net:3032 -u YOUR_CIVIC_ADDRESS.worker -p x
```

The launcher refuses to start a manually selected build if the required
instructions are unavailable. All other command-line options are passed to the
selected miner unchanged.

## Included files

```text
civiclight-miner-windows-x86_64/
|-- civiclight-miner.exe
|-- start-mining.bat
|-- README.md
|-- RELEASE_NOTES.md
|-- LICENSE
|-- COPYING
`-- bin/
    |-- civiclight-core2.exe
    |-- civiclight-avx.exe
    |-- civiclight-avx2.exe
    |-- civiclight-avx2-sha.exe
    |-- civiclight-avx512.exe
    |-- civiclight-avx512-sha.exe
    `-- required runtime DLL files
```

Do not move an individual miner executable out of `bin`. Run the top-level
launcher instead.

## Troubleshooting

### Windows Security or antivirus warning

Mining applications are frequently classified as potentially unwanted mining
software. Only download releases from the official NitroPool GitHub repository
and inspect or compile the published source if you do not trust a binary.

### Miner window closes immediately

Open Command Prompt or PowerShell in the extracted directory and run the miner
there so the error remains visible. You can also use `start-mining.bat`, which
pauses when the miner exits.

### Missing DLL or miner binary

Extract the complete release ZIP again. The launcher, `bin` directory, miner
binaries and DLL files must remain together in the packaged directory layout.

### Check which build is selected

```powershell
.\civiclight-miner.exe --list-cpu-builds
.\civiclight-miner.exe --launcher-dry-run
```

Include that output, the full miner command and the startup error when
reporting a problem.

## Credits

- Based on [cpuminer-opt](https://github.com/JayDDee/cpuminer-opt) 26.1 by JayDDee
- CivicLight algorithm and original miner work by the [CivicLight project](https://github.com/CivicLight/civiclight-miner-windows)
- CivicLight thread-local workspace performance optimization, Windows multi-CPU builds, automatic launcher, packaging and release testing by [NitroPool](https://nitropool.net) / NoF8

## Licence

Distributed under the GNU General Public License version 2 in accordance with
the upstream cpuminer-opt licence. Copyright and attribution remain with their
respective contributors.

## Disclaimer

Mining places sustained load on the CPU and can increase power consumption and
temperature. Ensure the system has adequate cooling and monitor it during
initial testing. This software is provided without warranty. Use it at your
own risk.
