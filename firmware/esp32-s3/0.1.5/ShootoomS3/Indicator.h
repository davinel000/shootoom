#pragma once
#include <string.h>
#include "Settings.h"
#include "Config.h"
struct Indicator {
  uint8_t grb[Config::MAX_PIXELS*3]={};
  void pixel(uint8_t i,uint8_t r,uint8_t g,uint8_t b){if(i<Config::MAX_PIXELS){grb[3*i]=g;grb[3*i+1]=r;grb[3*i+2]=b;}}
  void build(const Profile& p,Field field,bool running,bool fault,bool boot,bool savedFlash,uint32_t held,uint32_t now){
    memset(grb,0,sizeof(grb));const uint8_t n=uint8_t(p.pixels),slots=n>1?n-1:1;
    if(fault||savedFlash||held>=3500){for(uint8_t i=0;i<n;++i)pixel(i,fault?((now/300)%2?10:1):6,fault?0:6,fault?0:6);return;}
    if(boot||held>=1200){pixel(uint8_t((now/200)%n),8,3,0);return;}
    if(field==Field::Autorun){for(uint8_t i=0;i<(p.autoBoot?n:1);++i)pixel(i,0,8,0);return;}
    uint8_t r=0,g=0,b=0,marker=0;bool bar=false;
    switch(field){
      case Field::Program:r=g=b=5;marker=uint8_t(p.program%slots);break;
      case Field::Scale:b=9;marker=uint8_t((p.scaleMilli-50)*(slots-1)/950);bar=true;break;
      case Field::Orientation:r=6;b=8;marker=uint8_t(p.orientation%slots);break;
      case Field::Rotation:g=7;b=7;marker=uint8_t((p.rotation/45)%slots);break;
      case Field::Speed:r=8;b=3;marker=uint8_t((p.speedPct-25)*(slots-1)/375);bar=true;break;
      case Field::Gap:r=8;g=3;marker=uint8_t(p.gap()*(slots-1)/60);bar=true;break;
      case Field::Autorun:break;
    }
    for(uint8_t i=0;i<slots;++i)if(bar?i<=marker:i==marker)pixel(i,r,g,b);
    if(n>1)pixel(n-1,running?0:5,running?5:2,0); // Last LED: green playing / amber paused.
  }
};
