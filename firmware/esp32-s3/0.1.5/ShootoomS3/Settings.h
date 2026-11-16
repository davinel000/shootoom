#pragma once
#include <stdint.h>
#include <stddef.h>

enum class Program:uint8_t {Show,Stone,Frame};
enum class Field:uint8_t {Program,Scale,Orientation,Rotation,Speed,Gap,Autorun};
inline uint32_t profileHash(const uint32_t* words,size_t count) {
  uint32_t h=2166136261u;
  for(size_t i=0;i<count;++i) for(uint8_t b=0;b<4;++b) {h^=(words[i]>>(8*b))&255;h*=16777619u;}
  return h;
}
struct Profile {
  uint32_t version=3,scaleMilli=1000,orientation=0,rotation=0,speedPct=100;
  uint32_t showGap=10,stoneGap=0,frameGap=0,program=0,pixels=12,autoBoot=0,offUs=500,settleUs=1500,checksum=0;
  uint32_t hash() const {
    const uint32_t w[]={version,scaleMilli,orientation,rotation,speedPct,showGap,stoneGap,frameGap,program,pixels,autoBoot,offUs,settleUs};
    return profileHash(w,13);
  }
  bool commonValuesValid() const {return scaleMilli>=50&&scaleMilli<=1000&&orientation<8&&rotation<360&&speedPct>=25&&speedPct<=400&&showGap<=60&&stoneGap<=60&&frameGap<=60&&program<3&&pixels>=1&&pixels<=24&&autoBoot<=1&&offUs<=20000&&settleUs<=20000;}
  bool valuesValid() const {return version==3&&rotation%45==0&&commonValuesValid();}
  bool storedValid() const {return valuesValid()&&checksum==hash();}
  bool previousValid() const {return version==2&&commonValuesValid()&&checksum==hash();}
  Profile migratePrevious() const {
    Profile p=*this;p.version=3;p.scaleMilli=1000;p.rotation=0;p.checksum=0;return p;
  }
  uint32_t gap() const {return program==0?showGap:program==1?stoneGap:frameGap;}
  uint32_t& gapRef() {return program==0?showGap:program==1?stoneGap:frameGap;}
};
static_assert(sizeof(Profile)==56,"Version 3 retains fourteen uint32 words");
struct LegacyProfile {
  uint32_t version,scaleMilli,orientation,offUs,settleUs,gapSeconds,program,pixels,autoBoot,checksum;
  bool valid() const {
    const uint32_t w[]={version,scaleMilli,orientation,offUs,settleUs,gapSeconds,program,pixels,autoBoot};
    return version==1&&scaleMilli>=50&&scaleMilli<=1000&&orientation<8&&offUs<=20000&&settleUs<=20000&&gapSeconds<=60&&program<4&&pixels>=1&&pixels<=24&&autoBoot<=1&&checksum==profileHash(w,9);
  }
  Profile migrate() const {
    Profile p;p.orientation=orientation;p.offUs=offUs;p.settleUs=settleUs;
    p.showGap=gapSeconds;p.program=program==0?0:2;p.pixels=pixels;
    p.autoBoot=program==0?autoBoot:0; // Replaced diagnostic programs must not silently become continuous autorun.
    return p;
  }
};
static_assert(sizeof(LegacyProfile)==40,"Legacy profile layout preserved");
inline void adjustProfile(Profile& p,Field field,int direction) {
  if(field==Field::Program) p.program=(p.program+(direction>0?1:2))%3;
  if(field==Field::Orientation) p.orientation=(p.orientation+(direction>0?1:7))%8;
  if(field==Field::Rotation) p.rotation=(p.rotation+(direction>0?45:315))%360;
  if(field==Field::Autorun) p.autoBoot=direction>0?1:0;
  if(field==Field::Scale) {const int n=int(p.scaleMilli)+direction*10;p.scaleMilli=n<50?50:n>1000?1000:uint32_t(n);}
  if(field==Field::Speed) {const int n=int(p.speedPct)+direction*25;p.speedPct=n<25?25:n>400?400:uint32_t(n);}
  if(field==Field::Gap) {const int n=int(p.gap())+direction;p.gapRef()=n<0?0:n>60?60:uint32_t(n);}
}
