#pragma once
#include "Frame.h"
#include "Motion.h"
#include "Transform.h"

// Hardware-independent playback. A pending segment survives pause/settings;
// changing geometry realigns dark at the previous vertex, never restarts Show.
class Renderer {
 public:
  enum class State:uint8_t {Paused,Playing,Gap,Boot};
  Motion motion;ViewTransform view;
  struct Bounds {
    uint16_t minX=4095,minY=4095,maxX=0,maxY=0;uint32_t writes=0;
    void add(XY p){if(p.x<minX)minX=p.x;if(p.y<minY)minY=p.y;if(p.x>maxX)maxX=p.x;if(p.y>maxY)maxY=p.y;++writes;}
  } bounds;
  void begin(const Profile& p){cfg=p;view.set(p);motion.position=view.apply({2048,2048});resetSource();}
  State state() const{return mode;}
  bool active() const{return mode==State::Playing&&!frozen;}
  bool running() const{return mode==State::Playing||mode==State::Gap;}
  bool suspended() const{return frozen;}
  bool beamAllowed() const{return allowBeam;}
  size_t sourcePosition() const{return player.position();}
  uint32_t cycles() const{return completedCycles;}
  void boot(uint32_t ms){mode=State::Boot;waitUntil=ms+5000;started=false;}
  void stop(uint32_t us){motion.cancel(us,cfg.offUs);mode=State::Paused;started=false;resetSource();}
  void start(uint32_t us,bool light=true){stop(us);allowBeam=light;started=true;mode=State::Playing;bounds=Bounds();completedCycles=0;}
  void pause(uint32_t us,uint32_t ms){
    if(!running())return;
    resumeMode=mode;remainingGap=mode==State::Gap&&!due(ms,waitUntil)?waitUntil-ms:0;
    cancelSegment(us);mode=State::Paused;
  }
  void resume(uint32_t us,uint32_t ms){
    if(mode!=State::Paused)return;
    if(!started){start(us,true);return;}
    mode=resumeMode;if(mode==State::Gap)waitUntil=ms+remainingGap;
  }
  void freeze(uint32_t us,uint32_t ms){if(frozen)return;frozen=true;frozenAt=ms;cancelSegment(us);}
  void thaw(uint32_t ms){if(!frozen)return;if(mode==State::Gap||mode==State::Boot)waitUntil+=ms-frozenAt;frozen=false;}
  void configure(const Profile& p,uint32_t us,uint32_t ms){
    const bool geometry=view.differs(p),program=p.program!=cfg.program,gap=p.gap()!=cfg.gap();
    if(geometry||program)cancelSegment(us);
    cfg=p;view.set(p);
    if(program){resetSource();bounds=Bounds();if(mode==State::Gap)mode=State::Playing;if(resumeMode==State::Gap)resumeMode=State::Playing;}
    else if(geometry){alignRequired=true;bounds=Bounds();}
    if(gap){remainingGap=cfg.gap()*1000;if(mode==State::Gap){waitUntil=ms+remainingGap;if(frozen)frozenAt=ms;}}
  }
  Motion::Event tick(uint32_t us,uint32_t ms){
    if(frozen)return Motion::None;
    if(mode==State::Boot&&due(ms,waitUntil)){start(us,true);}
    if(mode==State::Gap&&due(ms,waitUntil)){resetSource();mode=State::Playing;}
    if(mode!=State::Playing)return Motion::None;
    if(motion.busy()){
      const auto e=motion.poll(us);
      if((e&Motion::Write)&&motion.lit())bounds.add(motion.position);
      return e;
    }
    if(aligning)aligning=false;
    if(inFlight){anchor=pending.xy;hasPending=false;inFlight=false;holdActual=false;}
    if(alignRequired){alignRequired=false;aligning=true;return motion.submit(view.apply(anchor),false,0,us,cfg.offUs,cfg.settleUs);}
    if(!hasPending){
      if(!(cfg.program==2?frame.next(pending):player.next(pending,1.0f))){
        motion.cancel(us,cfg.offUs);++completedCycles;
        mode=State::Gap;waitUntil=ms+cfg.gap()*1000;return Motion::BeamOff;
      }
      hasPending=true;holdActual=false;
    }
    pendingPhysical=view.apply(pending.xy);inFlight=true;
    const uint32_t hold=holdActual||pending.scenePause?pending.holdUs:paced(pending.holdUs,cfg.speedPct);
    const bool fast=cfg.program==2;
    return motion.submit(pendingPhysical,pending.lit&&allowBeam,hold,us,cfg.offUs,cfg.settleUs,fast?32:16,paced(fast?25:100,cfg.speedPct));
  }
 private:
  Profile cfg;ShowPlayer player;Frame frame;Target pending={};XY pendingPhysical={2048,2048},anchor={2048,2048};
  State mode=State::Paused,resumeMode=State::Playing;
  bool started=false,allowBeam=true,frozen=false,hasPending=false,inFlight=false,alignRequired=false,aligning=false,holdActual=false;
  uint32_t waitUntil=0,remainingGap=0,frozenAt=0,completedCycles=0;
  void resetSource(){player.reset(cfg.program==1);frame.reset();hasPending=inFlight=aligning=alignRequired=holdActual=false;anchor={2048,2048};}
  void cancelSegment(uint32_t us){
    if(inFlight&&(motion.holding()||!motion.busy())){
      // Preserve an intentional dark scene wait; do not repeat a completed lit dwell.
      if(pending.scenePause){pending.holdUs=motion.remainingHold(us);holdActual=true;}
      else {anchor=pending.xy;hasPending=false;holdActual=false;}
    }
    if(aligning){alignRequired=true;aligning=false;}
    inFlight=false;motion.cancel(us,cfg.offUs);
  }
};
