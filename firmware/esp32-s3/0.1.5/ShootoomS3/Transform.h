#pragma once
#include "Geometry.h"
#include "Settings.h"
struct ViewTransform {
  uint32_t scaleMilli=1000,orientation=0,rotation=0;
  float cosine=1,sine=0,effectiveScale=1;
  void set(const Profile& p) {
    scaleMilli=p.scaleMilli;orientation=p.orientation;rotation=p.rotation;
    const float radians=rotation*(3.14159265358979323846f/180.0f);
    cosine=cosf(radians);sine=sinf(radians);
    // Explicit fit-to-canvas rule: 100% means the largest rotated square
    // that fits the same DAC range. Never silently clip a rotated corner.
    const float fit=fabsf(cosine)+fabsf(sine);
    effectiveScale=(scaleMilli/1000.0f)/(fit>1?fit:1);
  }
  bool differs(const Profile& p) const {return scaleMilli!=p.scaleMilli||orientation!=p.orientation||rotation!=p.rotation;}
  XY apply(XY base) const {
    const float x=(int(base.x)-2048)*effectiveScale,y=(int(base.y)-2048)*effectiveScale;
    return oriented({boundCode(2048+x*cosine+y*sine),boundCode(2048-x*sine+y*cosine)},uint8_t(orientation));
  }
};
inline uint32_t paced(uint32_t us,uint32_t speedPct) {
  if(!us)return 0;const uint64_t n=uint64_t(us)*100/speedPct;
  return n<1?1:n>0x7fffffffu?0x7fffffffu:uint32_t(n);
}
