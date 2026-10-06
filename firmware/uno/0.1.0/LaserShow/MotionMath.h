#pragma once
#include <stdint.h>
namespace Sutum {
// Legacy rounded Q14 calculation with a nonzero divisor.
// Inputs: clipped 12-bit coordinate differences and validated quality.
inline int32_t movementSteps(int32_t dx,int32_t dy,int32_t quality) {
  const int32_t ax=dx<0?-dx:dx, ay=dy<0?-dy:dy;
  const int32_t steps=((ax>ay?ax:ay)*quality+8192)/16384;
  return steps>0?steps:1;
}
}
