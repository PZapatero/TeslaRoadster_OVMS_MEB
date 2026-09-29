#include "roadster_ess_minmax.h"
#include <assert.h>
#include <cmath>
#include <iostream>
#include <string>

static void Put(uint8_t* d, int low, int high)
  {
  d[0] = low & 255; d[1] = (low >> 8) & 255;
  d[2] = high & 255; d[3] = (high >> 8) & 255;
  }

int main(int argc, char** argv)
  {
  RoadsterEssMinMax ess;
  uint8_t d[8] = {};
  float low = 0, high = 0;
  if (argc == 2 && std::string(argv[1]) == "--replay")
    {
    uint32_t now, id;
    unsigned byte, decoded = 0, voltage = 0, temperature = 0;
    while (std::cin >> now >> std::hex >> id >> std::dec)
      {
      for (auto& b : d) { std::cin >> byte; b = byte; }
      if (ess.Decode(id, d, 8, false, false, now)) ++decoded;
      if (ess.Voltage(now, low, high)) ++voltage;
      if (ess.Temperature(now, low, high)) ++temperature;
      }
    std::cout << "decoded=" << decoded << " full_voltage=" << voltage
              << " full_temperature=" << temperature << '\n';
    return decoded && voltage && temperature ? 0 : 1;
    }

  // No partial-pack extrema: voltage/temperature coverage is independent.
  assert(!ess.Voltage(0, low, high));
  for (unsigned sheet = 0; sheet < 11; ++sheet)
    {
    Put(d, 30000 - sheet * 10, 32000 + sheet * 10);
    assert(ess.Decode(0x08a + sheet * 8, d, 8, false, false, 100));
    assert(ess.Voltage(100, low, high) == (sheet == 10));
    }
  assert(std::fabs(low - 29900 / 8192.0f) < 0.00001f);
  assert(std::fabs(high - 32100 / 8192.0f) < 0.00001f);
  assert(!ess.Temperature(100, low, high));
  // Negative temperatures must retain their sign.
  for (unsigned sheet = 0; sheet < 11; ++sheet)
    {
    Put(d, -256 * int(sheet), 256 * int(sheet + 20));
    assert(ess.Decode(0x08c + sheet * 8, d, 8, false, false, 100));
    }
  assert(ess.Temperature(100, low, high));
  assert(low == -10 && high == 30);
  // A previous extreme must disappear when its sheet changes.
  Put(d, 0, 20 * 256);
  ess.Decode(0x0dc, d, 8, false, false, 101);
  assert(ess.Temperature(101, low, high));
  assert(low == -9 && high == 29);
  // Incomplete frames, other IDs and other frame formats are ignored.
  assert(!ess.Decode(0x08a, d, 4, false, false, 101));
  assert(!ess.Decode(0x08a, d, 8, true, false, 101));
  assert(!ess.Decode(0x08a, d, 8, false, true, 101));
  assert(!ess.Decode(0x088, d, 8, false, false, 101));
  assert(!ess.Decode(0x08b, d, 8, false, false, 101));
  assert(!ess.Decode(0x0e2, d, 8, false, false, 101));
  // Missing sheet data expires, even while other sheets keep transmitting.
  assert(ess.Voltage(129, low, high));
  assert(!ess.Voltage(130, low, high));
  assert(!ess.Temperature(130, low, high));
  // Invalid ordered extrema invalidate that sheet instead of hiding a fault.
  Put(d, 32000, 30000);
  ess.Decode(0x08a, d, 8, false, false, 101);
  assert(!ess.Voltage(101, low, high));
  // Freshness subtraction remains correct across uint32_t uptime wrap.
  RoadsterEssMinMax wrap;
  Put(d, 30000, 32000);
  for (unsigned i = 0; i < 11; ++i)
    wrap.Decode(0x08a + i * 8, d, 8, false, false, UINT32_MAX - 10);
  assert(wrap.Voltage(5, low, high));
  assert(!wrap.Voltage(20, low, high));
  std::cout << "ESS min/max tests passed\n";
  }
