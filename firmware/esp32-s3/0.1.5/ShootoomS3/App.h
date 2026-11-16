#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Preferences.h>
#include <stdlib.h>
#include <stdio.h>
#include "Renderer.h"
#include "Input.h"
#include "Indicator.h"
#include "Ring.h"

class ShootoomApp {
 public:
  void begin(){
    digitalWrite(Config::LASER,LOW);pinMode(Config::LASER,OUTPUT);
    digitalWrite(Config::DAC_CS,HIGH);pinMode(Config::DAC_CS,OUTPUT);
    digitalWrite(Config::DAC_LDAC,HIGH);pinMode(Config::DAC_LDAC,OUTPUT);
    pinMode(Config::ENC_CLK,INPUT_PULLUP);pinMode(Config::ENC_DT,INPUT_PULLUP);pinMode(Config::ENC_SW,INPUT_PULLUP);
    Serial.begin(115200); // Exhibition operation never waits for USB.
    storage=prefs.begin("shootoom-panel",false);
    if(storage){
      const size_t n=prefs.getBytesLength("profile");
      if(n==sizeof(Profile)){
        Profile p;if(prefs.getBytes("profile",&p,sizeof(p))==sizeof(p)){
          if(p.storedValid()){cfg=p;persisted=true;log("Loaded saved v3 profile.");}
          else if(p.previousValid()){cfg=p.migratePrevious();dirty=true;log("Migrated v2 in RAM: scale 1.00, angle 0; other settings retained. Save explicitly.");}
        }
      }else if(n==sizeof(LegacyProfile)){
        LegacyProfile old;if(prefs.getBytes("profile",&old,sizeof(old))==sizeof(old)&&old.valid()){cfg=old.migrate();dirty=true;log("Migrated v1 in RAM: scale 1.00, angle 0; orientation retained. Save explicitly.");}
      }
    }
    if(!persisted&&!dirty)log("Defaults: scale 1.00, angle 0, speed 100%, manual start.");
    renderer.begin(cfg);SPI.begin(Config::DAC_SCK,-1,Config::DAC_MOSI,-1);writeDac(renderer.motion.position);
    input.begin(readAB(),pressed(),millis());
    if(!ring.begin()){fault=true;log("RMT initialization failed; start blocked.");}
    if(cfg.autoBoot&&storage&&!pressed()&&!fault){renderer.boot(millis());log("Autostart in 5 s. Press/turn cancels.");}
    lastLoop=micros();status();
  }
  void tick(){
    const uint32_t now=micros(),gap=now-lastLoop;lastLoop=now;if(gap>maxLoopUs)maxLoopUs=gap;
    if(renderer.active()&&gap>Config::LOOP_GAP_LIMIT_US){stop();fault=true;log("Stopped: loop gap >100 ms. Click or clear to acknowledge.");}
    pollPanel();readCommands();
    if(armed&&due(millis(),armDeadline))armed=false;
    apply(renderer.tick(micros(),millis()));
    if(!renderer.motion.lit()){
      updateRing();drainLog();
      if(monitor&&Serial&&uint32_t(millis()-lastMonitor)>=1000){lastMonitor=millis();status();}
    }
    if(!renderer.active())delay(1);else delayMicroseconds(5);
  }
 private:
  Profile cfg;Preferences prefs;Renderer renderer;PanelInput input;Ring ring;Indicator indicator;Field field=Field::Program;
  bool storage=false,dirty=false,persisted=false,fault=false,armed=false,monitor=false,ignoreRelease=false;
  uint32_t lastLoop=0,maxLoopUs=0,armDeadline=0,lastRing=0,lastMonitor=0,savedUntil=0;
  int32_t encoderTurns=0;
  char command[64]={};uint8_t commandSize=0;bool overflow=false;
  char tx[1536]={};uint16_t txHead=0,txTail=0;uint32_t droppedLog=0;
  bool pressed()const{return digitalRead(Config::ENC_SW)==LOW;}
  uint8_t readAB()const{return (digitalRead(Config::ENC_CLK)?2:0)|(digitalRead(Config::ENC_DT)?1:0);}
  void log(const char* text){
    if(!Serial)return;
    const size_t n=strlen(text)+1,used=(txHead+sizeof(tx)-txTail)%sizeof(tx);
    if(n>=sizeof(tx)-used){++droppedLog;return;}
    for(const char* p=text;*p;++p){tx[txHead]=*p;txHead=(txHead+1)%sizeof(tx);}
    tx[txHead]='\n';txHead=(txHead+1)%sizeof(tx);
  }
  void drainLog(){
    if(!Serial){txHead=txTail=0;return;}
    int budget=Serial.availableForWrite();if(budget>32)budget=32;
    while(budget-->0&&txTail!=txHead){if(Serial.write(uint8_t(tx[txTail]))!=1)break;txTail=(txTail+1)%sizeof(tx);}
  }
  void stop(){digitalWrite(Config::LASER,LOW);renderer.stop(micros());armed=false;}
  void freeze(){digitalWrite(Config::LASER,LOW);renderer.freeze(micros(),millis());}
  void changed(){dirty=true;persisted=false;armed=false;digitalWrite(Config::LASER,LOW);renderer.configure(cfg,micros(),millis());digitalWrite(Config::LASER,renderer.motion.lit()?HIGH:LOW);}
  void editDone(){changed();status();}
  bool startAllowed(){if(fault||!ring.ok()){log("Start blocked: acknowledge fault / check RMT.");return false;}return true;}
  bool saveProfile(){
    // Explicit only. The caller blanks and freezes first, preserving playback.
    if(!storage){log("Save failed: storage unavailable; RAM settings retained.");return false;}
    cfg.checksum=cfg.hash();
    if(prefs.putBytes("profile",&cfg,sizeof(cfg))!=sizeof(cfg)){log("Save failed: flash write; RAM settings retained.");return false;}
    dirty=false;persisted=true;savedUntil=millis()+600;log("SAVED: current program, boot policy and all settings.");return true;
  }
  void pollPanel(){
    const auto e=input.poll(readAB(),pressed(),millis());
    if(e.turn){if(encoderTurns>1000000000||encoderTurns< -1000000000)encoderTurns=0;encoderTurns+=e.turn;}
    if(renderer.state()==Renderer::State::Boot&&(e.down||e.turn)){
      stop();ignoreRelease=e.down;log("Autostart cancelled; paused.");return;
    }
    if(e.down)freeze();
    if(e.up){
      renderer.thaw(millis());
      if(ignoreRelease){ignoreRelease=false;return;}
      if(fault){fault=false;log("Fault acknowledged; paused.");return;}
      switch(classifyGesture(e.duration)){
        case Gesture::Menu:field=Field((uint8_t(field)+1)%7);status();break;
        case Gesture::Toggle:
          if(renderer.running()){digitalWrite(Config::LASER,LOW);renderer.pause(micros(),millis());}
          else if(startAllowed())renderer.resume(micros(),millis());
          armed=false;status();break;
        case Gesture::Save:
          freeze();saveProfile();renderer.thaw(millis());lastLoop=micros();status();break;
      }
    }
    if(e.turn&&!input.pressed()&&!ignoreRelease){adjustProfile(cfg,field,e.turn);editDone();}
  }
  void writeWord(uint16_t word){
    digitalWrite(Config::DAC_CS,LOW);delayMicroseconds(1);SPI.transfer(uint8_t(word>>8));SPI.transfer(uint8_t(word));digitalWrite(Config::DAC_CS,HIGH);delayMicroseconds(1);
  }
  void writeDac(XY xy){
    SPI.beginTransaction(SPISettings(Config::SPI_HZ,MSBFIRST,SPI_MODE0));writeWord(dacWord(xy.x,false));writeWord(dacWord(xy.y,true));
    digitalWrite(Config::DAC_LDAC,LOW);delayMicroseconds(1);digitalWrite(Config::DAC_LDAC,HIGH);SPI.endTransaction();
  }
  void apply(Motion::Event e){
    if(e&Motion::BeamOff)digitalWrite(Config::LASER,LOW);
    if(e&Motion::BeamOn)digitalWrite(Config::LASER,HIGH);
    if(e&Motion::Write){writeDac(renderer.motion.position);renderer.motion.afterWrite(micros());}
  }
  void updateRing(){
    if(!ring.ok()){if(!fault){stop();fault=true;}return;}
    const uint32_t now=millis();if(uint32_t(now-lastRing)<30)return;lastRing=now;
    indicator.build(cfg,field,renderer.running(),fault,renderer.state()==Renderer::State::Boot,savedUntil&&!due(now,savedUntil),input.heldMs(now),now);
    if(!ring.paint(indicator.grb)){stop();fault=true;log("Ring RMT driver failed; stopped.");}
  }
  void status(){
    static const char* programs[]={"Show","Stone","Frame"};
    static const char* fields[]={"program","scale","orientation","rotation","speed","gap","autorun"};
    static const char* states[]={"paused","playing","gap","boot"};char line[260];
    snprintf(line,sizeof(line),"S3 %s program=%s state=%s field=%s step=%u/%u cycles=%u beam=%s",Config::VERSION,programs[cfg.program],states[uint8_t(renderer.state())],fields[uint8_t(field)],unsigned(renderer.sourcePosition()),unsigned(cfg.program==1?1:cfg.program==2?0:SHOW_COUNT),unsigned(renderer.cycles()),renderer.motion.lit()?"HIGH":"LOW");log(line);
    snprintf(line,sizeof(line),"scale=%.3f appliedScale=%.3f orient=%u angle=%udeg speed=%u%% gap=%us boot=%s saved=%u dirty=%u",cfg.scaleMilli/1000.0,renderer.view.effectiveScale,unsigned(cfg.orientation),unsigned(cfg.rotation),unsigned(cfg.speedPct),unsigned(cfg.gap()),cfg.autoBoot?"auto":"manual",persisted?1:0,dirty?1:0);log(line);
    snprintf(line,sizeof(line),"pixels=%u turns=%ld AB=%u SW=%u off=%uus settle=%uus nvs=%u rmt=%s fault=%u maxLoop=%uus dropped=%u",unsigned(cfg.pixels),long(encoderTurns),unsigned(readAB()),pressed()?1:0,unsigned(cfg.offUs),unsigned(cfg.settleUs),storage?1:0,ring.ok()?"ok":"failed",fault?1:0,unsigned(maxLoopUs),unsigned(droppedLog));log(line);
  }
  void diag(){
    status();const auto& b=renderer.bounds;char line[230];
    if(!b.writes){log("No lit DAC writes in this run/geometry yet; no measured span.");return;}
    snprintf(line,sizeof(line),"Commanded lit DAC bounds: X=%u..%u Y=%u..%u writes=%u; gain=1x (software only)",unsigned(b.minX),unsigned(b.maxX),unsigned(b.minY),unsigned(b.maxY),unsigned(b.writes));log(line);
    snprintf(line,sizeof(line),"Nominal DAC span X=%.3fVpp Y=%.3fVpp; calculated 4.7x analog span X=%.3fVpp Y=%.3fVpp (NOT measured)",(b.maxX-b.minX)*.0005,(b.maxY-b.minY)*.0005,(b.maxX-b.minX)*.00235,(b.maxY-b.minY)*.00235);log(line);
  }
  bool parseProgram(const char* s,uint32_t& p){if(!strcmp(s,"show"))p=0;else if(!strcmp(s,"stone"))p=1;else if(!strcmp(s,"frame"))p=2;else return false;return true;}
  void execute(){
    if(!strcmp(command,"status")){status();return;}
    if(!strcmp(command,"diag")){diag();return;}
    if(!strcmp(command,"stop")){stop();log("Stopped; next start begins selected program.");return;}
    if(!strcmp(command,"pause")){digitalWrite(Config::LASER,LOW);renderer.pause(micros(),millis());status();return;}
    if(!strcmp(command,"clear")){if(!renderer.running()){fault=false;maxLoopUs=0;}return;}
    if(!strcmp(command,"monitor on")){monitor=true;status();return;}
    if(!strcmp(command,"monitor off")){monitor=false;return;}
    if(!strcmp(command,"arm")){
      if(renderer.running()||renderer.state()==Renderer::State::Boot||fault){log("Pause/stop and clear fault first.");return;}
      armed=true;armDeadline=millis()+10000;log("Armed 10 s: show / stone / frame / resume.");return;
    }
    uint32_t selected=cfg.program;
    const bool resumeCommand=!strcmp(command,"resume");
    const bool lightProgram=parseProgram(command,selected)||!strcmp(command,"calibrate");
    if(lightProgram||resumeCommand){
      if(!armed||due(millis(),armDeadline)||input.pressed()||!startAllowed()){armed=false;log("Pause/stop, then arm first (10 s).");return;}
      armed=false;
      if(resumeCommand)renderer.resume(micros(),millis());
      else {if(!strcmp(command,"calibrate"))selected=2;cfg.program=selected;changed();digitalWrite(Config::LASER,LOW);renderer.start(micros(),true);}
      status();return;
    }
    if(!strcmp(command,"preview")||!strcmp(command,"calpreview")){
      if(!startAllowed()||input.pressed())return;
      if(!strcmp(command,"calpreview")){cfg.program=2;changed();}
      digitalWrite(Config::LASER,LOW);renderer.start(micros(),false);status();return;
    }
    if(!strcmp(command,"save")){
      const bool held=renderer.suspended();freeze();saveProfile();if(!held)renderer.thaw(millis());lastLoop=micros();status();return;
    }
    if(!strncmp(command,"select ",7)){
      if(!parseProgram(command+7,selected)){log("select show / stone / frame");return;}
      cfg.program=selected;editDone();return;
    }
    if(!strcmp(command,"boot auto")||!strcmp(command,"boot manual")){cfg.autoBoot=!strcmp(command,"boot auto");editDone();return;}
    if(!strncmp(command,"scale ",6)){
      char* end=nullptr;const float value=strtof(command+6,&end);
      if(end==command+6||*end||!isfinite(value)||value<.05f||value>1){log("scale .05..1");return;}
      cfg.scaleMilli=uint32_t(lroundf(value*1000));editDone();return;
    }
    struct Option {const char* prefix;uint32_t* value;uint32_t min,max;bool pausedOnly;};
    Option options[]={{"orient ",&cfg.orientation,0,7,false},{"angle ",&cfg.rotation,0,315,false},{"speed ",&cfg.speedPct,25,400,false},{"gap ",&cfg.gapRef(),0,60,false},{"pixels ",&cfg.pixels,1,Config::MAX_PIXELS,false},{"blankoff ",&cfg.offUs,0,20000,true},{"settle ",&cfg.settleUs,0,20000,true}};
    for(auto& o:options)if(!strncmp(command,o.prefix,strlen(o.prefix))){
      const char* number=command+strlen(o.prefix);char* end=nullptr;const long value=strtol(number,&end,10);
      if(end==number||*end||value<long(o.min)||value>long(o.max)||(o.value==&cfg.rotation&&value%45)||(o.pausedOnly&&(renderer.running()||renderer.state()==Renderer::State::Boot))){log("Value out of range / angle must be 0,45,...315 / blank timing requires pause.");return;}
      *o.value=uint32_t(value);editDone();return;
    }
    log("status, diag, monitor on/off, stop, pause, clear, arm, show/stone/frame/resume, preview, select show/stone/frame, scale .05..1, orient 0..7, angle 0/45/...315, speed 25..400, gap 0..60, boot auto/manual, save, pixels 1..24");
  }
  void readCommands(){
    for(uint8_t budget=0;budget<32&&Serial.available();++budget){
      const char c=char(Serial.read());
      if(c=='!'){stop();commandSize=0;overflow=false;log("Stopped.");continue;}
      if(c=='\r'||c=='\n'){if(overflow){stop();log("Command too long; stopped.");}else if(commandSize){command[commandSize]=0;execute();}commandSize=0;overflow=false;}
      else if(commandSize<sizeof(command)-1&&!overflow)command[commandSize++]=c;else overflow=true;
    }
  }
};
