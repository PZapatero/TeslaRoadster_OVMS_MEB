# Tesla Roadster OVMS MEB

This branch contains the modified Roadster module, tests, and a patch for voltage and temperature min/max monitoring.

**This GitHub repository was initialized separately and does not yet contain the complete OVMS source tree or upstream Git history. This branch is an overlay, not a buildable standalone firmware checkout.**

Base repository: https://github.com/openvehicles/Open-Vehicle-Monitoring-System-3

Base commit: `86dfa0afe4a8c5b2978ef069caedac661627b180`

To use the patch in a complete OVMS checkout:

```sh
git clone https://github.com/openvehicles/Open-Vehicle-Monitoring-System-3.git
cd Open-Vehicle-Monitoring-System-3
git checkout -b teslaroadster-meb 86dfa0afe4a8c5b2978ef069caedac661627b180
git apply /path/to/roadster-ess-minmax.patch
```

See `vehicle/OVMS.V3/components/vehicle_teslaroadster/README-MEB.md` for signal definitions and host tests. Decoder, log replay, and web display tests passed. A complete ESP-IDF firmware build and on-vehicle validation are still pending.
