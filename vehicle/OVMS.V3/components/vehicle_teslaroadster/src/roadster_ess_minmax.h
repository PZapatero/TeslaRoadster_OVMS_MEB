// Roadster_2_0_ESS_Bus.dbc: BMB_MinMaxAvgV/T, sheets 1..11.
// Keep only sheet extrema; never publish individual bricks or sensor indices.
#ifndef ROADSTER_ESS_MINMAX_H
#define ROADSTER_ESS_MINMAX_H

#include <stdint.h>
#include <stddef.h>

class RoadsterEssMinMax
  {
  public:
    static constexpr unsigned Sheets = 11;
    static constexpr uint32_t MaxAge = 30; // seconds; all sheets must be fresh

    bool Decode(uint32_t id, const uint8_t* data, size_t length,
                bool extended, bool remote, uint32_t now)
      {
      if (extended || remote || length != 8 || id < 0x08a || id > 0x0dc)
        return false;
      const unsigned offset = id - 0x08a;
      const unsigned kind = offset % 8;
      if (kind != 0 && kind != 2)
        return false;
      const unsigned sheet = offset / 8;
      if (sheet >= Sheets)
        return false;
      Pair& pair = kind == 0 ? m_voltage[sheet] : m_temperature[sheet];
      const uint16_t low = data[0] | (uint16_t(data[1]) << 8);
      const uint16_t high = data[2] | (uint16_t(data[3]) << 8);
      // Temperatures are signed little-endian, 1/256 degree Celsius.
      pair.min = kind == 0 ? low / 8192.0f : Signed(low) / 256.0f;
      pair.max = kind == 0 ? high / 8192.0f : Signed(high) / 256.0f;
      pair.time = now;
      pair.seen = pair.min <= pair.max;
      return true;
      }

    bool Voltage(uint32_t now, float& min, float& max) const
      { return Aggregate(m_voltage, now, min, max); }
    bool Temperature(uint32_t now, float& min, float& max) const
      { return Aggregate(m_temperature, now, min, max); }

  private:
    struct Pair
      {
      float min = 0, max = 0;
      uint32_t time = 0;
      bool seen = false;
      };
    Pair m_voltage[Sheets];
    Pair m_temperature[Sheets];

    static int32_t Signed(uint16_t value)
      { return value & 0x8000 ? int32_t(value) - 65536 : value; }

    static bool Aggregate(const Pair* pairs, uint32_t now, float& min, float& max)
      {
      for (unsigned i = 0; i < Sheets; ++i)
        if (!pairs[i].seen || uint32_t(now - pairs[i].time) >= MaxAge)
          return false;
      min = pairs[0].min;
      max = pairs[0].max;
      for (unsigned i = 1; i < Sheets; ++i)
        {
        if (pairs[i].min < min) min = pairs[i].min;
        if (pairs[i].max > max) max = pairs[i].max;
        }
      return true;
      }
  };

#endif
