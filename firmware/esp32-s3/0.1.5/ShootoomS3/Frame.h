#pragma once
#include "ShowPlayer.h"
// Full-field square + large F orientation mark, as requested by the owner.
// Relocations between the outline and F are dark, with no central parking hold.
// Both use exactly the same scale/orientation/angle/speed controls as Show.
class Frame {
 public:
  void reset(){index=0;}
  bool next(Target& t) {
    static constexpr int16_t points[][3]={
      {-2047,-2047,0},{2047,-2047,1},{2047,2047,1},{-2047,2047,1},{-2047,-2047,1},
      {-1300,-900,0},{-1300,1400,1},{100,1400,1},{-1300,1400,0},{-1300,300,0},{-300,300,1}
    };
    if(index>=11)return false;const auto& p=points[index++];
    t={{uint16_t(2048+p[0]),uint16_t(2048+p[1])},p[2]!=0,0};return true;
  }
 private:uint8_t index=0;
};
