#pragma once
#include <stddef.h>
#include "Geometry.h"

enum class StepKind : uint8_t {Symbol, Text, Pause};
struct ShowStep {
  StepKind kind;
  const uint16_t* data;
  size_t words;
  const char* text;
  uint16_t repeats, subdivisions;
  uint32_t delayMs;
  uint16_t posX, posY, rotation;
  float localScale;
  uint16_t legacySpeed;
};
#include "ShowData.h"
#include "FontData.h"

struct Target { XY xy; bool lit; uint32_t holdUs; bool scenePause=false; };
class ShowPlayer {
 public:
  // Index 0 is the diagnostic stone contour; exhibition playback starts at 1.
  void reset(bool stoneOnly=false) {stepIndex=stoneOnly?0:1;limit=stoneOnly?1:SHOW_COUNT;last={2048,2048};clearStep();}
  size_t position() const {return stepIndex;}
  bool finished() const {return stepIndex>=limit;}
  bool next(Target& target, float scale) {
    if (finished()) return false;
    const ShowStep& s=SHOW[stepIndex];
    if (s.kind==StepKind::Pause) {
      target={last,false,s.delayMs*1000,true}; advance(); return true;
    }
    if (s.kind==StepKind::Symbol) return symbol(s,target,scale);
    return text(s,target,scale);
  }
 private:
  size_t stepIndex=1, word=0, character=0, glyphWords=0;
  size_t limit=SHOW_COUNT;
  uint16_t pass=0, sub=0;
  const uint16_t* glyph=nullptr;
  float penX=0;
  XY last={2048,2048};
  void clearStep() {word=0;character=0;glyphWords=0;pass=0;sub=0;glyph=nullptr;penX=0;}
  void advance() {++stepIndex;clearStep();}
  bool symbol(const ShowStep& s, Target& t, float scale) {
    if (word==0) {
      last=showCoordinates(s.data[0]&0x7fff,s.data[1],scale);
      t={last,false,s.delayMs*1000};word=2;return true;
    }
    if (word>=s.words) {
      t={last,false,0};word=0;sub=0;
      if (++pass>=(stepIndex==0?1:s.repeats)) advance();
      return true;
    }
    const float fraction=float(sub)/s.subdivisions;
    const float x0=s.data[word-2]&0x7fff, y0=s.data[word-1];
    const float x1=s.data[word]&0x7fff, y1=s.data[word+1];
    last=showCoordinates(x0+(x1-x0)*fraction,y0+(y1-y0)*fraction,scale);
    t={last,(s.data[word]&0x8000)!=0,s.delayMs*1000};
    if (++sub>s.subdivisions) {sub=0;word+=2;}
    return true;
  }
  void selectGlyph(char c) {
    switch(c) {
#define GLYPH(c,name) case c: glyph=draw_##name;glyphWords=sizeof(draw_##name)/sizeof(uint16_t);break
      GLYPH('A',A);GLYPH('B',B);GLYPH('C',C);GLYPH('D',D);GLYPH('E',E);
      GLYPH('F',F);GLYPH('G',G);GLYPH('H',H);GLYPH('I',I);GLYPH('J',J);
      GLYPH('K',K);GLYPH('L',L);GLYPH('M',M);GLYPH('N',N);GLYPH('O',O);
      GLYPH('P',P);GLYPH('Q',Q);GLYPH('R',R);GLYPH('S',S);GLYPH('T',T);
      GLYPH('U',U);GLYPH('V',V);GLYPH('W',W);GLYPH('X',X);GLYPH('Y',Y);GLYPH('Z',Z);
      GLYPH('0',0);GLYPH('1',1);GLYPH('2',2);GLYPH('3',3);GLYPH('4',4);
      GLYPH('5',5);GLYPH('6',6);GLYPH('7',7);GLYPH('8',8);GLYPH('9',9);
      GLYPH('!',exclam);GLYPH('?',question);GLYPH('.',dot);
#undef GLYPH
      default:glyph=nullptr;glyphWords=0;break;
    }
  }
  bool text(const ShowStep& s, Target& t, float scale) {
    while (!glyph) {
      const char c=s.text[character];
      if (!c) {
        t={last,false,0};
        if (++pass>=s.repeats) advance();
        else {character=0;penX=0;}
        return true;
      }
      selectGlyph(c);word=0;
      if (!glyph) {penX+=(c==' '?400:850)*s.localScale+5*s.localScale;++character;}
    }
    if (word>=glyphWords) {
      t={last,false,0};glyph=nullptr;
      penX+=855*s.localScale;++character;return true;
    }
    const float x=(glyph[word]&0x7fff)*s.localScale+penX;
    const float y=glyph[word+1]*s.localScale;
    const float radians=s.rotation*(3.14159265358979323846f/180);
    // Legacy AVR text used position-1950 + a 2048 3D translation (= position+98).
    const float tx=x*cosf(radians)-y*sinf(radians)+s.posX+98;
    const float ty=x*sinf(radians)+y*cosf(radians)+s.posY+98;
    last=showCoordinates(tx,ty,scale);
    t={last,word!=0 && (glyph[word]&0x8000)!=0,200};word+=2;return true;
  }
};
