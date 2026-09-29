# Roadster firmware build

The `Build Roadster min-max firmware` workflow on this branch uses read-only repository access. It checks out upstream commit `362f8742592b8b6bb9fa4238bd6fe0983868eced` (installed firmware `3.3.006-172-g362f87425`), then applies `patches/roadster-ess-minmax.patch` in its temporary build directory. This branch contains the complete source tree pinned to that same installed-version base, plus the extension. The workflow independently fetches the fixed upstream base and applies the patch to verify the exact build inputs.

ESP-IDF is pinned to OVMS commit `9063c8662ca5d67b5490c1503bd4377b380feed3` and the compiler is the version referenced by upstream `.travis.yml`.

The candidate profile is `sdkconfig.default.hw31`: OVMS hardware 3.1 with PSRAM and 16 MB flash. Exact device hardware compatibility has not yet been confirmed. Successful compilation alone does not authorize or verify installation on the vehicle.

Successful workflow artifacts include the application binary, SDK configuration, exact base/extension commit IDs, applied patch, and SHA-256 digest. The build log is uploaded separately. Use only the application binary for an eventual verified OTA update; do not replace the bootloader or partition table.

Vehicle menu → Battery Min/Max shows voltage min/max and temperature min/max, using Roadster-side ESS summary messages. These may already be mapped or filtered by the MEB translator.
