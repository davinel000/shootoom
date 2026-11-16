#pragma once
#include <stdint.h>

// Full Gray-code cycle per detent, with the decoder unchanged from 0.1.3.
// Button actions are classified ON RELEASE so a Save hold cannot start/stop first.
class PanelInput {
 public:
  struct Events {int8_t turn=0;bool down=false,up=false;uint32_t duration=0;};
  void begin(uint8_t ab,bool pressed,uint32_t now) {
    previous=ab&3;raw=stable=pressed;changedAt=pressedAt=now;enabled=!pressed;quarter=0;
  }
  Events poll(uint8_t ab,bool pressed,uint32_t now) {
    Events e;static constexpr int8_t table[16]={0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    ab&=3;
    if(ab!=previous) {
      if((ab^previous)==3) quarter=0;else quarter+=table[(previous<<2)|ab];
      previous=ab;
      if(quarter>=4){e.turn=1;quarter=0;}if(quarter<=-4){e.turn=-1;quarter=0;}
    }
    if(pressed!=raw){raw=pressed;changedAt=now;}
    if(raw!=stable&&uint32_t(now-changedAt)>=20) {
      stable=raw;
      if(stable){pressedAt=now;if(enabled)e.down=true;}
      else {if(enabled){e.up=true;e.duration=now-pressedAt;}enabled=true;}
    }
    return e;
  }
  bool pressed() const {return stable;}
  uint32_t heldMs(uint32_t now) const {return stable&&enabled?now-pressedAt:0;}
 private:
  uint8_t previous=3;int8_t quarter=0;bool raw=false,stable=false,enabled=true;
  uint32_t changedAt=0,pressedAt=0;
};
enum class Gesture:uint8_t {Menu,Toggle,Save};
inline Gesture classifyGesture(uint32_t ms) {return ms>=3500?Gesture::Save:ms>=1200?Gesture::Toggle:Gesture::Menu;}
