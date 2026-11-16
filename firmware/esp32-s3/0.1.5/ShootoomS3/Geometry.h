#pragma once
#include <math.h>
#include <stdint.h>
#include "Config.h"

struct XY { uint16_t x, y; };
// Apply to targets before motion interpolation. Runtime changes then cannot
// reinterpret the last physical DAC position and cause an immediate jump.
inline XY oriented(XY xy, uint8_t mode) {
  if (Config::FLIP_X) xy.x=4095-xy.x;
  if (Config::FLIP_Y) xy.y=4095-xy.y;
  if (mode&1) {const uint16_t oldX=xy.x;xy.x=xy.y;xy.y=oldX;}
  if (mode&2) xy.x=4095-xy.x;
  if (mode&4) xy.y=4095-xy.y;
  return xy;
}
inline uint16_t boundCode(float value) {
  if (!isfinite(value) || value <= 0) return 0;
  if (value >= 4095) return 4095;
  return static_cast<uint16_t>(lroundf(value));
}
inline uint16_t dacWord(uint16_t code, bool channelB) {
  // MCP4822: gain=1x, active=1. Same gain as the preserved AVR sketch.
  return (channelB ? 0xb000 : 0x3000) | (code > 4095 ? 4095 : code);
}
inline XY showCoordinates(float x, float y, float scale) {
  // Existing project distortion correction, then legacy 0.99 scaling.
  if (Config::LEGACY_CORRECTION) {
    const float correctedX = x / (1.2f - x / 4095.0f * 0.2f)
                           + 300.0f * (y / 4095.0f) * (1.0f - x / 4095.0f);
    const float correctedY = y / (1.0f + y / 4095.0f * 0.1f);
    x = correctedX; y = correctedY;
  }
  x = 2048 + (x * 0.99f - 2048) * scale;
  y = 2048 + (y * 0.99f - 2048) * scale;
  return {boundCode(x), boundCode(y)};
}
inline uint16_t movementSteps(XY from, XY to) {
  const uint16_t dx = from.x > to.x ? from.x-to.x : to.x-from.x;
  const uint16_t dy = from.y > to.y ? from.y-to.y : to.y-from.y;
  const uint16_t distance = dx > dy ? dx : dy;
  return distance == 0 ? 1 : (distance + Config::MAX_MOVE_CODES-1)/Config::MAX_MOVE_CODES;
}
inline XY interpolate(XY from, XY to, uint16_t index, uint16_t count) {
  return {static_cast<uint16_t>(int32_t(from.x)+(int32_t(to.x)-from.x)*index/count),
          static_cast<uint16_t>(int32_t(from.y)+(int32_t(to.y)-from.y)*index/count)};
}
inline bool due(uint32_t now, uint32_t deadline) { return int32_t(now-deadline) >= 0; }
