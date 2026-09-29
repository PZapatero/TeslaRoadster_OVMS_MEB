# Roadster ESS min/max monitor

The Roadster module registers OVMS CAN3 in listen-only mode at 125 kbit/s
(OVT1 wiring: Roadster CAN2/ESS). It sends no requests on that bus.

Vehicle → Battery Min/Max (`/xtr/ess`) displays only:

| Metric | Meaning |
| --- | --- |
| `v.b.p.voltage.min` | Lowest reported brick voltage, V |
| `v.b.p.voltage.max` | Highest reported brick voltage, V |
| `v.b.p.temp.min` | Lowest reported cell temperature, °C |
| `v.b.p.temp.max` | Highest reported cell temperature, °C |

The same values appear in `stat`. No individual brick/sensor values or indices
are published. Existing CAN1 functionality is unchanged.

## Source and validity

Signal layout comes from the supplied `Roadster_2_0_ESS_Bus.dbc` reference:
sheet voltage extrema use IDs `0x08A + 8*n`, temperatures `0x08C + 8*n`,
where `n=0..10`. Both messages are standard CAN data frames with DLC 8.
Bytes 0–1 and 2–3 carry minimum and maximum, little-endian.
Voltage is unsigned at 1/8192 V; temperature is signed at 1/256 °C.

The decoder keeps only these two extrema per sheet, then aggregates all eleven
sheets. Voltage and temperature coverage are independent. A pair is unavailable
until every sheet has supplied valid ordered extrema in the last 30 seconds.
Missing/expired data clears the standard metrics; two internal validity flags
(`xtr.b.voltage.valid`, `xtr.b.temp.valid`) prevent the web interface from
displaying OVMS's JSON fallback zero as a real measurement. The page displays
`?` while unavailable. Sleeping vehicles or long report intervals may therefore
show `?`. Data from different sheets is asynchronous, with a maximum age of 30 s.

These are **Roadster-side reported values**. With an MEB translator they may
already be mapped/filtered. This does not add a raw MEB telemetry protocol or
decode a speculative `0x6F0` message. The translator must already publish the
native sheet summary messages for this monitor to display values.

## Host verification

From this component directory:

```sh
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -Isrc tests/ess_minmax.cpp -o /tmp/roadster-ess-tests
/tmp/roadster-ess-tests
```

Tests cover full-sheet coverage, signed temperatures, changing extrema,
malformed/unrelated frames, expiration and uptime wrap. `--replay` accepts
records containing decimal seconds, hexadecimal CAN ID and eight decimal
bytes. This validates the decoder independently of ESP-IDF; it does not replace
a complete firmware build or an on-vehicle test.
