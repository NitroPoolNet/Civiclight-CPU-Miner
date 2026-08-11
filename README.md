# CivicLight Linux CPU Miner

An optimized Linux CPU miner for the **CivicLight** proof-of-work algorithm used by CivicNet.

This release includes six CPU-specific binaries and an automatic launcher that detects the host CPU and safely selects the best supported build.

- Linux x86_64
- Built on Ubuntu 22.04
- Automatic CPU detection
- Intel and AMD support
- Configurable thread count
- No developer fee

## Download

Download the latest release:

[**CivicLight Linux CPU Miner Releases**](https://github.com/NitroPoolNet/Civiclight-CPU-Miner/releases/latest)

The Linux release archive is:

```text
civiclight-miner-linux-x86_64.tar.gz
```

## Extract

```bash
tar -xzf civiclight-miner-linux-x86_64.tar.gz
cd civiclight-miner-linux-x86_64
```

## Start mining on NitroPool

Replace `YOUR_CIVIC_ADDRESS` with your CivicNet wallet address.

Adding `.worker` to the address is optional but makes individual miners easier to identify.

### Australia

```bash
./civiclight-miner \
  -a civiclight \
  -o stratum+tcp://au.nitropool.net:3032 \
  -u YOUR_CIVIC_ADDRESS.worker \
  -p x
```

### Europe

```bash
./civiclight-miner \
  -a civiclight \
  -o stratum+tcp://eu.nitropool.net:3032 \
  -u YOUR_CIVIC_ADDRESS.worker \
  -p x
```

### United States

```bash
./civiclight-miner \
  -a civiclight \
  -o stratum+tcp://us.nitropool.net:3032 \
  -u YOUR_CIVIC_ADDRESS.worker \
  -p x
```

## Set the number of threads

Use `-t NUMBER` to control the number of CPU mining threads.

Example using 10 threads:

```bash
./civiclight-miner \
  -a civiclight \
  -o stratum+tcp://au.nitropool.net:3032 \
  -u YOUR_CIVIC_ADDRESS.worker \
  -p x \
  -t 10
```

If `-t` is not specified, the miner will use its default thread configuration.

## Run in screen

Start a detached `screen` session using 10 threads:

```bash
screen -dmS civiclight ./civiclight-miner -a civiclight -o stratum+tcp://au.nitropool.net:3032 -u YOUR_CIVIC_ADDRESS.worker -p x -t 10
```

Attach to the miner:

```bash
screen -r civiclight
```

Detach without stopping the miner by pressing:

```text
Ctrl+A, then D
```

## Automatic CPU selection

Always start the miner using:

```bash
./civiclight-miner
```

The launcher detects the CPU features exposed by Linux and selects the fastest compatible binary from the included `bin` directory.

| Selected binary | CPU CPU capability |
|---|---|
| `cpuminer-core2` | Core2-class baseline |
| `cpuminer-avx` | AVX and AES |
| `cpuminer-avx2` | AVX2, AES, BMI and FMA |
| `cpuminer-avx2-sha` | AVX2 with SHA-NI |
| `cpuminer-avx512` | AVX-512 |
| `cpuminer-avx512-sha` | AVX-512 with SHA-NI |

The selected binary is displayed during startup:

```text
CivicLight CPU selection: cpuminer-avx2-sha
```

Do not run a binary directly from `bin/` unless you know the CPU supports every instruction required by that build. An incompatible binary may terminate with an `Illegal instruction` error.

## Included files

```text
civiclight-miner-linux-x86_64/
├── civiclight-miner
├── bin/
├── README.md
├── LICENSE
└── COPYING
```

## Tested hardware

| Processor | Threads | Selected binary | Observed live rate |
|---|---:|---|---:|
| Intel Core i7-8700K | 10 | `cpuminer-avx2` | approximately 3.4–3.5 kH/s |
| Intel Core i5-14400F | 12 | `cpuminer-avx2-sha` | approximately 5.7–5.9 kH/s |

Performance will vary depending on CPU model, thread count, power limits, cooling and other system activity.

> **Benchmark note:** CivicLight benchmark mode currently overstates the reported hashrate. Live mining output and pool-side statistics provide the meaningful performance measurement.

## Troubleshooting

### Permission denied

Restore executable permissions:

```bash
chmod +x civiclight-miner bin/cpuminer-*
```

### Illegal instruction

Make sure you are starting the automatic launcher:

```bash
./civiclight-miner
```

Do not manually select a binary from the `bin` directory.

If the automatic launcher still fails, open an issue and include the output of:

```bash
lscpu
```

### Miner reported as malware

Cryptocurrency mining software is frequently flagged by antivirus and endpoint-security products because it performs mining work.

Only download the miner from the official NitroPool GitHub Releases page.

## Credits

- Based on [cpuminer-opt](https://github.com/JayDDee/cpuminer-opt) 26.1 by JayDDee
- CivicLight algorithm and original miner work by the [CivicLight project](https://github.com/CivicLight/civiclight-miner-windows)
- Linux multi-CPU build, automatic launcher, packaging and release testing by [NitroPool](https://nitropool.net) / No-F8

## Licence

This miner is distributed under the **GNU General Public License version 2** in accordance with the upstream cpuminer-opt licence.

The release includes the applicable `LICENSE` and `COPYING` files. Copyright and attribution remain with their respective contributors.

## Links

- [Latest release](https://github.com/NitroPoolNet/Civiclight-CPU-Miner/releases/latest)
- [NitroPool](https://nitropool.net)
- [NitroPool Discord](https://discord.gg/guTygsnbx9)
- [CivicLight miner project](https://github.com/CivicLight/civiclight-miner-windows)

## Disclaimer

Mining places sustained load on the CPU and can increase power consumption and temperature. Ensure the system has adequate cooling and monitor it during initial testing.

This software is provided without warranty. Use it at your own risk.