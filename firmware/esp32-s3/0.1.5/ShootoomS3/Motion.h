#pragma once
#include "Geometry.h"

// Pure scheduling logic; IO is performed by App in the returned event order.
class Motion {
 public:
  enum Event:uint8_t {None=0, BeamOff=1, BeamOn=2, Write=4};
  XY position={2048,2048}; // Actual DAC coordinates, including orientation
  bool busy() const {return phase!=Phase::Idle;}
  bool lit() const {return beam;}
  uint32_t remainingHold(uint32_t now) const {return phase==Phase::Hold&&!due(now,nextAt)?nextAt-now:0;}
  bool holding() const {return phase==Phase::Hold;}
  void cancel(uint32_t now,uint32_t offUs) {
    turnOff(now,offUs);phase=Phase::Idle;
  }
  Event submit(XY target,bool light,uint32_t hold,uint32_t now,
               uint32_t offUs,uint32_t settleUs,uint16_t stride=Config::MAX_MOVE_CODES,uint32_t interval=Config::MOVE_INTERVAL_US) {
    origin=position;destination=target;index=0;
    const uint16_t dx=origin.x>destination.x?origin.x-destination.x:destination.x-origin.x;
    const uint16_t dy=origin.y>destination.y?origin.y-destination.y:destination.y-origin.y;
    const uint16_t distance=dx>dy?dx:dy;
    count=distance?(distance+stride-1)/stride:1;holdUs=hold;moveInterval=interval;
    if (!light) {
      turnOff(now,offUs);
      phase=Phase::Move;
      nextAt=now+offRemaining(now);
      return BeamOff; // App drives GPIO LOW before any motion can occur.
    }
    if (!beam) {
      const uint32_t remaining=offRemaining(now);
      phase=Phase::WaitOn;nextAt=now+(settleUs>remaining?settleUs:remaining);
    } else {phase=Phase::Move;nextAt=now;}
    return None;
  }
  Event poll(uint32_t now) {
    if (!busy()||!due(now,nextAt)) return None;
    if (phase==Phase::WaitOn) {
      beam=true;phase=Phase::Move;nextAt=now;
      return BeamOn; // Separate event: first lit DAC movement comes later.
    }
    if (phase==Phase::Hold) {phase=Phase::Idle;return None;}
    position=interpolate(origin,destination,++index,count);
    phase=index>=count?Phase::Hold:Phase::Move;
    afterWrite(now);
    return Write;
  }
  void afterWrite(uint32_t completedAt) {
    nextAt=completedAt+(phase==Phase::Hold?holdUs:moveInterval);
  }
 private:
  enum class Phase:uint8_t {Idle,WaitOn,Move,Hold};
  Phase phase=Phase::Idle;
  bool beam=false,offPending=false;
  XY origin={2048,2048},destination={2048,2048};
  uint16_t index=0,count=1;
  uint32_t nextAt=0,holdUs=0,offAt=0,offDuration=0,moveInterval=Config::MOVE_INTERVAL_US;
  uint32_t offRemaining(uint32_t now) {
    if (!offPending) return 0;
    const uint32_t elapsed=now-offAt;
    if (elapsed>=offDuration) {offPending=false;return 0;}
    return offDuration-elapsed;
  }
  void turnOff(uint32_t now,uint32_t offUs) {
    if (beam) {offAt=now;offDuration=offUs;offPending=true;}
    else (void)offRemaining(now);
    beam=false;
  }
};
