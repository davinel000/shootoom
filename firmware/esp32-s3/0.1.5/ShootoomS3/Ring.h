#pragma once
#include <Arduino.h>
#include <esp32-hal-rmt.h>
#include "Config.h"

class Ring {
 public:
  bool begin() {
    digitalWrite(Config::RING,LOW);pinMode(Config::RING,OUTPUT);
    healthy=rmtInit(Config::RING,RMT_TX_MODE,RMT_MEM_NUM_BLOCKS_2,10000000);
    if (healthy) healthy=rmtSetEOT(Config::RING,LOW);
    return healthy;
  }
  // Send only when previous transfer and reset have completed; never rewrite
  // symbols while the RMT peripheral can still be using this persistent buffer.
  bool paint(const uint8_t* frame) {
    if (!healthy) return false;
    if (busy) {
      if (!rmtTransmitCompleted(Config::RING)) return true;
      busy=false;readyAt=micros()+300;return true;
    }
    if (int32_t(micros()-readyAt)<0) return true;
    bool changed=!sent;
    for (size_t i=0;i<sizeof(previous);++i) if (frame[i]!=previous[i]) changed=true;
    if (!changed) return true;
    size_t n=0;
    for (size_t i=0;i<sizeof(previous);++i) for (uint8_t bit=0;bit<8;++bit) {
      const uint8_t byte=frame[i];
      const bool one=byte&(0x80>>bit);
      symbols[n].level0=1;symbols[n].duration0=one?8:4;
      symbols[n].level1=0;symbols[n].duration1=one?4:8;++n;
    }
    // Includes black pixels beyond configured count to clear a previous larger setting.
    healthy=rmtWriteAsync(Config::RING,symbols,n);
    if (healthy) {memcpy(previous,frame,sizeof(previous));sent=true;busy=true;}
    return healthy;
  }
  bool ok() const {return healthy;}
 private:
  rmt_data_t symbols[Config::MAX_PIXELS*24]={};
  uint8_t previous[Config::MAX_PIXELS*3]={};
  bool healthy=false,busy=false,sent=false;
  uint32_t readyAt=0;
};
