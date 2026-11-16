#pragma once
#include <stdint.h>

namespace Config {
constexpr char VERSION[] = "0.1.5";
// GPIO numbers, matched to reviewed EPLZON v0.8.4 and the Plus variant header.
constexpr uint8_t LASER = 1;   // D0 -> HCT/AHCT A6 -> B6 -> R1 -> MOSFET gate
constexpr uint8_t DAC_CS = 2;  // D1 -> A3 -> MCP4822 /CS
constexpr uint8_t DAC_LDAC = 4;// D3 -> A4 -> MCP4822 /LDAC
constexpr uint8_t DAC_SCK = 7; // D8 -> A1 -> MCP4822 SCK
constexpr uint8_t DAC_MOSI = 9;// D10 -> A2 -> MCP4822 SDI
constexpr uint8_t RING = 44; // D7 -> AHCT A5/B5 -> existing 330 ohm -> DIN
constexpr uint8_t ENC_CLK = 5; // D4, main board J5
constexpr uint8_t ENC_DT = 6;  // D5, main board J6
constexpr uint8_t ENC_SW = 43; // D6, main board J7
constexpr uint8_t MAX_PIXELS = 24;
constexpr uint32_t SPI_HZ = 1000000;
constexpr uint32_t MOVE_INTERVAL_US = 100;
constexpr uint32_t BLANK_SETTLE_US = 1500;
constexpr uint32_t BLANK_OFF_US = 500; // Initial tuning value, not a measured module latency
constexpr uint32_t LOOP_GAP_LIMIT_US = 100000;
constexpr uint16_t MAX_MOVE_CODES = 16;
constexpr bool FLIP_X = true; // Preserved AVR output orientation
constexpr bool FLIP_Y = false;
constexpr bool LEGACY_CORRECTION = true;
// Panel defaults and the explicit saved boot policy live in Settings.h / Preferences.
}
