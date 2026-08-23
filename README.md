# CivicLight CPU Miner

CivicLight CPU Miner is the NitroPool Performance Edition CPU miner for the CivicLight proof-of-work algorithm used by CivicNet.

Developed and maintained by NitroPool / NoF8, it provides optimized Linux and Windows x86-64 releases with automatic CPU detection and no developer fee.

## Features

- Linux and Windows x86-64 support
- Automatic CPU detection
- Intel and AMD support
- Core2, AVX, AVX2, SHA-NI and AVX-512 builds
- Persistent per-thread CivicLight workspaces
- Automatic selection of the fastest compatible build
- No developer fee

## Download

Download the latest package from the [CivicLight CPU Miner releases](https://github.com/NitroPoolNet/Civiclight-CPU-Miner/releases/latest) page.

| Platform | Package |
|---|---|
| Linux x86-64 | `civiclight-miner-linux-x86_64.tar.gz` |
| Windows x86-64 | `civiclight-miner-windows-x86_64.zip` |

## Start mining on NitroPool

Replace `YOUR_CIVIC_ADDRESS` with your CivicNet wallet address. Adding a `.worker` suffix is optional.

### Linux

Download and extract the Linux release:

```bash
tar -xzf civiclight-miner-linux-x86_64.tar.gz
cd civiclight-miner-linux-x86_64
```

Start mining:

```bash
./civiclight-miner \
  -a civiclight \
  -o stratum+tcp://au.nitropool.net:3032 \
  -u YOUR_CIVIC_ADDRESS.worker \
  -p x
```

Use `-t NUMBER` to select the number of mining threads:

```bash
./civiclight-miner \
  -a civiclight \
  -o stratum+tcp://au.nitropool.net:3032 \
  -u YOUR_CIVIC_ADDRESS.worker \
  -p x \
  -t 10
```

### Windows

Download and extract the complete Windows ZIP file. Do not run the miner from inside the ZIP archive.

Edit `start-mining.bat` and replace `YOUR_CIVIC_ADDRESS` with your CivicNet wallet address:

- **Windows 10:** Right-click `start-mining.bat` and select **Edit**
- **Windows 11:** Right-click `start-mining.bat`, select **Show more options**, then select **Edit**

Save the file and double-click `start-mining.bat` to begin mining.

To change the number of mining threads, add `-t NUMBER` to the mining command inside `start-mining.bat`.

## NitroPool regions

- Australia: `stratum+tcp://au.nitropool.net:3032`
- Europe: `stratum+tcp://eu.nitropool.net:3032`
- United States: `stratum+tcp://us.nitropool.net:3032`

Change `au` to `eu` or `us` to use the closest NitroPool region.

## Automatic CPU selection

Always use the supplied launcher:

- **Linux:** Run `./civiclight-miner`
- **Windows:** Run `start-mining.bat`, which starts `civiclight-miner.exe`

The launcher detects the available CPU features and selects the fastest compatible optimized build.

Available builds include:

- Core2
- AVX
- AVX2
- AVX2 with SHA-NI
- AVX-512
- AVX-512 with SHA-NI

Do not run a binary from the bundled `bin` directory unless testing a known-compatible build.

## Verified live performance

Testing was performed through live CivicLight mining on NitroPool.

| CPU | Threads | Selected build | Previous rate | v2.1.0 rate |
|---|---:|---|---:|---:|
| Intel Core i7-8700K | 10 | AVX2 | 422–430 H/s | 5.68–5.90 kH/s |
| Intel Core i5-14400F | 12 | AVX2 with SHA-NI | 5.53 kH/s | 9.80–10.05 kH/s |

Updated live testing produced:

- Intel Core i7-8700K: 32 of 32 shares accepted and one block solved
- Intel Core i5-14400F: 60 of 60 shares accepted
- No stale or rejected shares during updated testing

Performance will vary depending on CPU architecture, thread count, memory performance, power limits, cooling and system configuration.

## Build the Linux release

Docker is the recommended reproducible build method:

```bash
./docker-build-linux-release.sh
```

The completed archive is written to:

```text
release/civiclight-miner-linux-x86_64.tar.gz
```

## Build the Windows release

Build the Windows package from a Linux Docker host:

```bash
./docker-build-windows-release.sh
```

The completed archive is written to:

```text
release/civiclight-miner-windows-x86_64.zip
```

The Windows builder uses MinGW-w64 and packages the native automatic launcher, six optimized miner binaries and the required runtime DLL files.

## Documentation

- [Linux release instructions](LINUX_RELEASE_README.md)
- [Windows release instructions](WINDOWS_RELEASE_README.md)
- [Windows release notes](WINDOWS_RELEASE_NOTES.md)
- [Project release notes](RELEASE_NOTES)

## Credits

- Based on [cpuminer-opt 26.1](https://github.com/JayDDee/cpuminer-opt) by JayDDee
- CivicLight algorithm and original miner work by the [CivicLight project](https://github.com/CivicLight/civiclight-miner-windows)
- CivicLight workspace optimization, multi-CPU builds, automatic launchers, packaging and release testing by [NitroPool](https://nitropool.net) / NoF8

## Licence

Distributed under the GNU General Public License version 2 in accordance with the upstream cpuminer-opt licence.

The applicable `LICENSE` and `COPYING` files are included. Copyright and attribution remain with their respective contributors.

## Disclaimer

Mining places sustained load on the CPU and can increase power consumption and temperature. Ensure the system has adequate cooling and monitor it during initial testing.

This software is provided without warranty. Use it at your own risk.
