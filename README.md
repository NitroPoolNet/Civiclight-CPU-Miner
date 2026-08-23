# CivicLight CPU Miner

The NitroPool Performance Edition CPU miner for the **CivicLight** proof-of-work
algorithm used by CivicNet, developed and maintained by NitroPool / NoF8.

The project provides multi-binary releases for Linux and Windows. Each release
includes an automatic launcher that detects the host CPU and selects the
fastest compatible build.

- Linux x86-64
- Windows x86-64
- Intel and AMD support
- Core2 through AVX-512 builds
- SHA-NI optimized builds
- No developer fee

## Releases

Download the latest package from the
[CivicLight CPU Miner releases](https://github.com/NitroPoolNet/Civiclight-CPU-Miner/releases/latest)
page.

| Platform | Package |
|---|---|
| Linux x86-64 | `civiclight-miner-linux-x86_64.tar.gz` |
| Windows x86-64 | `civiclight-miner-windows-x86_64.zip` |

## Start mining on NitroPool

Replace `YOUR_CIVIC_ADDRESS` with your CivicNet wallet address. Adding
`.worker` is optional.

### Linux

```bash
./civiclight-miner \
  -a civiclight \
  -o stratum+tcp://au.nitropool.net:3032 \
  -u YOUR_CIVIC_ADDRESS.worker \
  -p x
```

### Windows PowerShell

```powershell
.\civiclight-miner.exe -a civiclight -o stratum+tcp://au.nitropool.net:3032 -u YOUR_CIVIC_ADDRESS.worker -p x
```

NitroPool regions:

- Australia: `stratum+tcp://au.nitropool.net:3032`
- Europe: `stratum+tcp://eu.nitropool.net:3032`
- United States: `stratum+tcp://us.nitropool.net:3032`

Use `-t NUMBER` to control the number of CPU mining threads.

## Automatic CPU selection

Always start the top-level `civiclight-miner` launcher on Linux or
`civiclight-miner.exe` launcher on Windows. Do not directly run a binary from
the bundled `bin` directory unless testing a known-compatible build.

The launchers select from:

- Core2 baseline
- AVX
- AVX2
- AVX2 with SHA-NI
- AVX-512
- AVX-512 with SHA-NI

The native Windows launcher additionally checks that Windows has enabled the
required AVX or AVX-512 register state. It supports inspection and compatible
manual selection:

```powershell
.\civiclight-miner.exe --list-cpu-builds
.\civiclight-miner.exe --launcher-dry-run
.\civiclight-miner.exe --cpu-build avx2 [miner options]
```

## Build Linux release

Docker is the recommended reproducible build path:

```bash
./docker-build-linux-release.sh
```

Output:

```text
release/civiclight-miner-linux-x86_64.tar.gz
```

## Build Windows release

Build the Windows package from a Linux Docker host:

```bash
./docker-build-windows-release.sh
```

Output:

```text
release/civiclight-miner-windows-x86_64.zip
```

The Windows builder uses MinGW-w64 and packages the native launcher, six miner
binaries and their required runtime DLL files.

## Documentation

- `LINUX_RELEASE_README.md` — Linux package instructions
- `WINDOWS_RELEASE_README.md` — Windows package instructions
- `WINDOWS_RELEASE_NOTES.md` — Windows release summary and test commands

## Credits

- Based on [cpuminer-opt](https://github.com/JayDDee/cpuminer-opt) 26.1 by JayDDee
- CivicLight algorithm and original miner work by the [CivicLight project](https://github.com/CivicLight/civiclight-miner-windows)
- CivicLight thread-local workspace performance optimization, Linux and Windows multi-CPU builds, automatic launchers, packaging and release testing by [NitroPool](https://nitropool.net) / NoF8

## Licence

Distributed under the GNU General Public License version 2 in accordance with
the upstream cpuminer-opt licence. The applicable `LICENSE` and `COPYING` files
are included, and copyright remains with the respective contributors.

## Disclaimer

Mining places sustained load on the CPU and can increase power consumption and
temperature. Ensure the system has adequate cooling and monitor it during
initial testing. This software is provided without warranty. Use it at your
own risk.
