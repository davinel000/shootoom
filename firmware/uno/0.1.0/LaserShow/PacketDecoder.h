// Sutum Uno 0.1.0: nonblocking decoder; no hardware dependencies.
#pragma once
#include <stdint.h>
namespace Sutum {
// 68 bytes take ~11.8 ms at 57600/8N1. This is an inter-byte deadline.
constexpr uint32_t PACKET_TIMEOUT_MS = 100;
constexpr uint32_t POINT_IDLE_TIMEOUT_MS = 1000;
inline uint16_t read16(const uint8_t* p) { return (uint16_t(p[0]) << 8) | uint16_t(p[1]); }
inline bool validLength(uint8_t cmd, uint8_t n) {
  switch (cmd) {
    case 1: return n >= 10 && n <= 64;
    case 2: return n == 4;
    case 3: return n == 0 || n == 2;
    case 4: return n == 0;
    default: return false;
  }
}
inline bool validPayload(uint8_t cmd, const uint8_t* p, uint8_t len) {
  if (!validLength(cmd,len)) return false;
  if (cmd == 2) return (read16(p)&0x7fff) <= 4095 && read16(p+2) <= 4095;
  if (cmd == 3) return len == 0 || (p[0]>0 && p[1]>=5 && p[1]<=200);
  if (cmd == 4) return true;
  const uint8_t n=p[0];
  if (!n || n>55 || len!=uint16_t(n)+9) return false;
  for (uint8_t i=0;i<n;++i) if (p[1+i]<32 || p[1+i]>126) return false;
  return p[n+1]>0 && read16(p+n+3)<=4095 && read16(p+n+5)<=4095
      && p[n+7]>=5 && p[n+7]<=200 && p[n+8]>0;
}
class PacketDecoder {
public:
  enum Result { Pending, Complete, Invalid };
  uint8_t command=0, length=0, payload[64]={};
  void reset() { state=Start; index=0; }
  bool expire(uint32_t now) {
    if (state!=Start && uint32_t(now-lastByte)>=PACKET_TIMEOUT_MS) { reset(); return true; }
    return false;
  }
  Result feed(uint8_t value,uint32_t now) {
    lastByte=now;
    switch(state) {
      case Start: if(value==0xff) state=Command; break;
      case Command: command=value; state=Length; break;
      case Length:
        length=value; index=0;
        if(!validLength(command,length)) { reset(); return Invalid; }
        state=length?Payload:End; break;
      case Payload:
        payload[index++]=value;
        if(index==length) state=End;
        break;
      case End:
        reset();
        if(value==0xfe && validPayload(command,payload,length)) return Complete;
        if(value==0xff) state=Command;
        return Invalid;
    }
    return Pending;
  }
private:
  enum State { Start,Command,Length,Payload,End } state=Start;
  uint8_t index=0;
  uint32_t lastByte=0;
};
}
