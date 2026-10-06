// Tests of actual production logic; no Arduino/hardware mocks.
#include "../../firmware/uno/0.1.0/LaserShow/PacketDecoder.h"
#include "../../firmware/uno/0.1.0/LaserShow/MotionMath.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <regex>
#include <string>

using Sutum::PacketDecoder;
static PacketDecoder::Result frame(PacketDecoder& d, uint8_t cmd,
                                  const std::vector<uint8_t>& payload, uint32_t now=0) {
  d.feed(0xff,now); d.feed(cmd,now); d.feed(uint8_t(payload.size()),now);
  for(auto b:payload) d.feed(b,now);
  return d.feed(0xfe,now);
}
int main(int argc,char** argv) {
  PacketDecoder d;
  assert(frame(d,2,{0x81,0xb6,0x0a,0xba})==PacketDecoder::Complete);
  assert(frame(d,2,{0x8f,0xff,0x0f,0xfe})==PacketDecoder::Complete); // embedded delimiters
  assert(frame(d,3,{})==PacketDecoder::Complete);
  assert(frame(d,4,{})==PacketDecoder::Complete);
  assert(frame(d,3,{1,100})==PacketDecoder::Complete);
  assert(frame(d,2,{0x90,0,0,0})==PacketDecoder::Invalid);
  // Partial payload expires, then valid traffic recovers.
  d.feed(0xff,10); d.feed(2,10); d.feed(4,10); d.feed(0x80,10);
  assert(!d.expire(109)); assert(d.expire(110)); assert(!d.expire(111));
  assert(frame(d,2,{0,1,0,1},112)==PacketDecoder::Complete);
  // millis wrap-around must not disable expiration.
  d.feed(0xff,0xfffffff0u);
  assert(!d.expire(0x53u)); assert(d.expire(0x54u));
  // Missing terminator: consume the next FF as start of next frame.
  d.feed(0xff,0); d.feed(2,0); d.feed(4,0);
  for(int i=0;i<4;++i) d.feed(0,0);
  assert(d.feed(0xff,0)==PacketDecoder::Invalid);
  d.feed(4,0); d.feed(0,0); assert(d.feed(0xfe,0)==PacketDecoder::Complete);
  for(int n=1;n<=55;++n) {
    std::vector<uint8_t> p(1+n,'A'); p[0]=uint8_t(n);
    p.insert(p.end(),{12,0,5,20,12,228,20,30});
    assert(frame(d,1,p)==PacketDecoder::Complete);
    p[0]=255; assert(frame(d,1,p)==PacketDecoder::Invalid);
    p[0]=uint8_t(n); p.back()=0;
    assert(frame(d,1,p)==PacketDecoder::Invalid);
  }
  // Exercise every command/length against the same bounds validation used by firmware.
  uint8_t payload[64]={};
  for(int c=0;c<256;++c) for(int len=0;len<256;++len) {
    if(!Sutum::validLength(uint8_t(c),uint8_t(len)))
      assert(!Sutum::validPayload(uint8_t(c),payload,uint8_t(len)));
    else (void)Sutum::validPayload(uint8_t(c),payload,uint8_t(len));
  }
  int movementChecks=0;
  for(int speed=1;speed<=255;++speed) {
    const int32_t quality=int32_t((5.0f/speed)*16384);
    for(int32_t dx=-4095;dx<=4095;++dx) {
      const int32_t distance=dx<0?-dx:dx;
      const int32_t legacy=(distance*quality+8192)/16384;
      const int32_t fixed=Sutum::movementSteps(dx,-dx,quality);
      assert(fixed>=1 && fixed==(legacy>0?legacy:1));
      const int32_t increment=(dx*16384)/fixed;
      (void)increment; ++movementChecks;
    }
  }
  assert(Sutum::movementSteps(0,0,546)==1);
  assert(Sutum::movementSteps(1,0,546)==1);
  // Validate actual preserved point data, not invented artwork.
  assert(argc==2);
  std::ifstream input(argv[1]); assert(input);
  std::string text((std::istreambuf_iterator<char>(input)),std::istreambuf_iterator<char>());
  std::regex hex("0x[0-9A-Fa-f]+");
  std::vector<uint16_t> words;
  for(auto i=std::sregex_iterator(text.begin(),text.end(),hex);i!=std::sregex_iterator();++i)
    words.push_back(uint16_t(std::stoul(i->str(),nullptr,16)));
  assert(words.size()==490370);
  for(size_t i=0;i<words.size();i+=2) {
    const uint16_t x=words[i],y=words[i+1];
    assert(frame(d,2,{uint8_t(x>>8),uint8_t(x),uint8_t(y>>8),uint8_t(y)})==PacketDecoder::Complete);
  }
  std::cout << "PASS: framing, validation, timeout/recovery/wrap; " << movementChecks
            << " motion cases; " << words.size()/2 << " preserved points\n";
}
