// Native checks of the actual pure classes, using synthetic timestamps.
// No Arduino/SPI/Preferences/RMT substitutes; this does not test physical IO.
#include <assert.h>
#include <stdio.h>
#include <initializer_list>
#include "../firmware/esp32-s3/0.1.5/ShootoomS3/Renderer.h"
#include "../firmware/esp32-s3/0.1.5/ShootoomS3/Input.h"
#include "../firmware/esp32-s3/0.1.5/ShootoomS3/Indicator.h"
static bool equal(XY a,XY b){return a.x==b.x&&a.y==b.y;}
static Motion::Event step(Renderer& r,uint32_t& us){auto e=r.tick(us,us/1000);if(e&Motion::Write)r.motion.afterWrite(us);us+=5;return e;}
static void untilCycle(Renderer& r,uint32_t& us){for(unsigned i=0;i<30000000;++i){step(r,us);if(r.cycles())return;}assert(false);}
int main(){
  Profile p;assert(p.valuesValid()&&p.scaleMilli==1000&&p.rotation==0&&!p.autoBoot);p.checksum=p.hash();assert(p.storedValid());p.speedPct=0;assert(!p.storedValid());
  Profile oldV2;oldV2.version=2;oldV2.scaleMilli=250;oldV2.rotation=37;oldV2.orientation=2;oldV2.speedPct=175;oldV2.autoBoot=1;oldV2.program=1;oldV2.checksum=oldV2.hash();
  assert(oldV2.previousValid()&&!oldV2.storedValid());p=oldV2.migratePrevious();
  assert(p.version==3&&p.scaleMilli==1000&&p.rotation==0&&p.orientation==2&&p.speedPct==175&&p.autoBoot==1&&p.program==1);
  ++oldV2.checksum;assert(!oldV2.previousValid());
  p.scaleMilli=800;p.rotation=90;p.checksum=p.hash();assert(p.storedValid()&&!p.previousValid());
  for(unsigned invalid:{1u,44u,46u,359u}){p.rotation=invalid;p.checksum=p.hash();assert(!p.storedValid());}
  LegacyProfile old={1,250,2,500,1500,12,0,12,1,0};
  const uint32_t words[]={1,250,2,500,1500,12,0,12,1};old.checksum=profileHash(words,9);assert(old.valid());
  p=old.migrate();assert(p.scaleMilli==1000&&p.orientation==2&&p.autoBoot&&p.program==0&&p.rotation==0&&p.speedPct==100&&p.showGap==12&&p.frameGap==0);
  for(uint32_t program=1;program<4;++program){old.program=program;const uint32_t w[]={old.version,old.scaleMilli,old.orientation,old.offUs,old.settleUs,old.gapSeconds,old.program,old.pixels,old.autoBoot};old.checksum=profileHash(w,9);assert(old.valid());p=old.migrate();assert(p.program==2&&!p.autoBoot);}
  p=Profile{};adjustProfile(p,Field::Program,-1);assert(p.program==2);adjustProfile(p,Field::Rotation,-1);assert(p.rotation==315);adjustProfile(p,Field::Rotation,1);assert(p.rotation==0);
  for(unsigned i=1;i<=8;++i){adjustProfile(p,Field::Rotation,1);assert(p.rotation==(i*45)%360&&p.valuesValid());}
  for(unsigned i=1;i<=8;++i){adjustProfile(p,Field::Rotation,-1);assert(p.rotation==(360-i*45)%360&&p.valuesValid());}
  for(int i=0;i<1000;++i)adjustProfile(p,Field::Speed,1);assert(p.speedPct==400);for(int i=0;i<1000;++i)adjustProfile(p,Field::Speed,-1);assert(p.speedPct==25);
  p.program=0;adjustProfile(p,Field::Gap,1);assert(p.showGap==11);p.program=1;assert(p.gap()==0);adjustProfile(p,Field::Gap,1);p.program=2;assert(p.gap()==0&&p.stoneGap==1);
  puts("PASS profile v3; old v1/v2 migrate to scale 1 / angle 0; other settings retained; saved v3 is not reset; eight 45-degree steps both ways");
  PanelInput input;input.begin(3,false,0);input.poll(1,false,1);input.poll(0,false,2);input.poll(2,false,3);assert(input.poll(3,false,4).turn==1);
  input.poll(2,false,5);input.poll(0,false,6);input.poll(1,false,7);assert(input.poll(3,false,8).turn==-1);
  input.poll(1,false,9);input.poll(3,false,10);input.poll(1,false,11);input.poll(0,false,12);input.poll(2,false,13);assert(input.poll(3,false,14).turn==1);
  input.poll(0,false,15);assert(!input.poll(3,false,16).turn);
  input.poll(3,true,100);assert(!input.poll(3,true,119).down);assert(input.poll(3,true,120).down);
  assert(!input.poll(3,true,1320).up);assert(!input.poll(3,true,3620).up); // No early toggle/save while held.
  input.poll(3,false,3700);auto e=input.poll(3,false,3720);assert(e.up&&classifyGesture(e.duration)==Gesture::Save);
  assert(classifyGesture(1199)==Gesture::Menu&&classifyGesture(1200)==Gesture::Toggle&&classifyGesture(3499)==Gesture::Toggle&&classifyGesture(3500)==Gesture::Save);
  input.begin(3,true,0);assert(!input.poll(3,true,6000).down);input.poll(3,false,6001);assert(!input.poll(3,false,6021).up);
  input.begin(3,false,0xffffff00u);input.poll(3,true,0xfffffff0u);assert(input.poll(3,true,4).down);input.poll(3,false,1204);e=input.poll(3,false,1224);assert(e.up&&classifyGesture(e.duration)==Gesture::Toggle);
  puts("PASS actual decoder directions/bounce; switch debounce, release-only gesture thresholds, held-at-boot and rollover");
  p=Profile{};ViewTransform view;view.set(p);assert(equal(view.apply({321,1234}),oriented({321,1234},0)));
  assert(dacWord(2048,false)==0x3800&&dacWord(2048,true)==0xb800);
  for(uint32_t scale:{50u,250u,1000u})for(uint32_t angle=0;angle<360;angle+=45)for(uint32_t ori=0;ori<8;++ori){
    p.scaleMilli=scale;p.rotation=angle;p.orientation=ori;view.set(p);
    for(XY xy: {XY{1,1},XY{4095,1},XY{4095,4095},XY{1,4095}}){const XY out=view.apply(xy);assert(out.x<=4095&&out.y<=4095);}
    assert(view.effectiveScale<=scale/1000.0f+.00001f);
  }
  p=Profile{};p.rotation=45;view.set(p);assert(fabsf(view.effectiveScale-.70710678f)<.00001f);
  assert(paced(1000,25)==4000&&paced(1000,100)==1000&&paced(1000,400)==250);
  puts("PASS geometry at eight 45-degree angles/8 orientations/3 scales; 45-degree fitting retained; DAC gain remains 1x");
  Motion motion;
  assert(motion.submit({2064,2048},true,200,100,500,1500)==Motion::None);
  assert(motion.poll(1599)==Motion::None&&!motion.lit());
  assert(motion.poll(1600)==Motion::BeamOn&&motion.position.x==2048);
  assert(motion.poll(1600)==Motion::Write&&motion.position.x==2064);motion.afterWrite(1650);
  assert(motion.poll(1849)==Motion::None&&motion.busy());motion.poll(1850);assert(!motion.busy());
  assert(motion.submit({2112,2048},false,0,2000,500,1500)==Motion::BeamOff);
  assert(motion.poll(2499)==Motion::None&&!motion.lit());assert(motion.poll(2500)==Motion::Write&&motion.position.x==2080);
  assert(motion.poll(2599)==Motion::None);assert(motion.poll(2600)==Motion::Write&&motion.position.x==2096);assert(motion.poll(2700)==Motion::Write&&motion.position.x==2112);motion.poll(2700);
  motion.submit({2144,2048},true,0,3000,500,1500);motion.cancel(3100,500);assert(!motion.busy()&&!motion.lit()&&motion.poll(9000)==Motion::None);
  Motion wrap;wrap.submit({2064,2048},true,0,0xffffff00u,500,1500);
  assert(wrap.poll(0xffffff00u+1499u)==Motion::None);assert(wrap.poll(0xffffff00u+1500u)==Motion::BeamOn);
  wrap.poll(0xffffff00u+1500u);wrap.poll(0xffffff00u+1500u);wrap.submit({2080,2048},false,0,0xfffffff0u,500,1500);
  assert(wrap.poll(483)==Motion::None&&wrap.poll(484)==Motion::Write);
  puts("PASS motion: beam-off before dark travel, fixed guard/settle, normal 16-code/100us bounds, actual-write dwell, cancel pending light and micros wrap");
  ShowPlayer stone,show;stone.reset(true);show.reset();Target t;
  size_t one=0,n=0;while(stone.next(t,1)){++one;assert(!t.scenePause);}assert(stone.finished()&&stone.position()==1&&one==626);
  assert(show.position()==1);assert(show.next(t,1));
  assert(equal(t.xy,showCoordinates(SHOW[1].data[0]&0x7fff,SHOW[1].data[1],1))&&!t.lit);
  show.reset();while(show.next(t,1)){assert(show.position()>=1);++n;}
  assert(n==1386626&&show.finished());show.reset();assert(show.position()==1);
  printf("PASS Show skips stone at start/repeat; %zu artwork targets; Stone remains separate (%zu targets)\n",n,one);
  Frame frame;unsigned points=0,litF=0;while(frame.next(t)){
    if(points<5)assert(t.xy.x==1||t.xy.x==4095||t.xy.y==1||t.xy.y==4095);
    else if(t.lit)++litF;
    assert(!equal(t.xy,{2048,2048})&&t.holdUs==0);++points;
  }assert(points==11&&litF==3);
  puts("PASS Frame: full square + preserved large F; dark relocations, no centre parking or 200 ms dwell");
  // Playback tests exercise Renderer directly, not copies of its scheduling logic.
  p=Profile{};p.program=2;Renderer r;r.begin(p);uint32_t us=0;r.start(us);untilCycle(r,us);const uint32_t normal=us;assert(r.bounds.writes&&r.state()==Renderer::State::Gap&&!r.motion.lit());
  p.speedPct=400;Renderer fast;fast.begin(p);us=0;fast.start(us);untilCycle(fast,us);assert(us<normal);
  p.speedPct=25;Renderer slow;slow.begin(p);us=0;slow.start(us);untilCycle(slow,us);assert(us>normal);
  p=Profile{};p.program=2;Renderer live;live.begin(p);us=0;live.start(us);
  for(unsigned i=0;i<100000;++i){step(live,us);if(live.motion.lit()&&live.motion.position.x<3000)break;}
  const XY physical=live.motion.position;live.freeze(us,us/1000);assert(!live.motion.lit()&&live.suspended());
  for(unsigned i=0;i<1000;++i)assert(step(live,us)==Motion::None);assert(equal(physical,live.motion.position));
  live.thaw(us/1000);live.pause(us,us/1000);assert(live.state()==Renderer::State::Paused);const size_t pos=live.sourcePosition();
  p.rotation=45;p.scaleMilli=800;live.configure(p,us,us/1000);assert(live.sourcePosition()==pos);live.resume(us,us/1000);
  bool darkWrites=false;for(unsigned i=0;i<100000;++i){auto event=step(live,us);if((event&Motion::Write)&&!live.motion.lit())darkWrites=true;if(event&Motion::BeamOn)break;}
  assert(darkWrites&&!live.suspended());
  puts("PASS Frame speed 25/100/400 changes scheduler duration; pause holds physical position; geometry resumes with dark realignment");
  p=Profile{};Renderer showLive;showLive.begin(p);us=0;showLive.start(us);
  assert(showLive.sourcePosition()==1); // No diagnostic contour on real playback.
  // Advance into artwork; hold/settings must not reset the actual Show cursor.
  for(unsigned i=0;i<10000000&&showLive.sourcePosition()<2;++i)step(showLive,us);
  const size_t showCursor=showLive.sourcePosition();assert(showCursor>=2);
  const XY before=showLive.motion.position;showLive.freeze(us,us/1000);assert(!showLive.motion.lit());showLive.thaw(us/1000);showLive.pause(us,us/1000);
  for(unsigned i=0;i<1000;++i)step(showLive,us);assert(equal(before,showLive.motion.position));
  p.speedPct=200;showLive.configure(p,us,us/1000);assert(showLive.sourcePosition()==showCursor);showLive.resume(us,us/1000);bool moved=false;
  for(unsigned i=0;i<100000;++i){step(showLive,us);if(!equal(before,showLive.motion.position)){moved=true;break;}}assert(moved);
  p=Profile{};p.program=2;p.frameGap=3;Renderer waits;waits.begin(p);us=0;waits.start(us);untilCycle(waits,us);uint32_t ms=us/1000;
  waits.freeze(us,ms);waits.thaw(ms+5000);assert(waits.tick(us+5000000,ms+5000)==Motion::None&&waits.state()==Renderer::State::Gap);
  waits.pause(us+5000000,ms+5000);waits.resume(us+10000000,ms+10000);waits.tick(us+11000000,ms+11000);assert(waits.state()==Renderer::State::Gap);
  waits.tick(us+14000000,ms+14000);assert(waits.state()==Renderer::State::Playing);
  Renderer boot;boot.begin(p);boot.boot(0xfffffff0u);boot.tick(0,4983);assert(boot.state()==Renderer::State::Boot);boot.tick(0,4984);assert(boot.state()==Renderer::State::Playing);
  boot.stop(0);boot.tick(10000000,10000);assert(boot.state()==Renderer::State::Paused&&!boot.motion.lit());
  puts("PASS Show pause/resume; gap countdown freezes through button/save holds; boot countdown wraps; stop cancels startup");
  Indicator led;p=Profile{};for(unsigned program=0;program<3;++program){p.program=program;led.build(p,Field::Program,false,false,false,false,0,0);assert(led.grb[program*3+1]==5&&led.grb[11*3+1]==5);}
  for(unsigned i=0;i<8;++i){p.rotation=i*45;led.build(p,Field::Rotation,false,false,false,false,0,0);assert(led.grb[3*i]==7&&led.grb[3*i+2]==7);for(unsigned j=0;j<11;++j)if(j!=i)assert(led.grb[j*3]==0&&led.grb[j*3+2]==0);}
  p.autoBoot=0;led.build(p,Field::Autorun,true,false,false,false,0,0);assert(led.grb[0]==8&&led.grb[3]==0);
  p.autoBoot=1;led.build(p,Field::Autorun,true,false,false,false,0,0);for(int i=0;i<12;++i)assert(led.grb[3*i]==8);
  led.build(p,Field::Scale,true,false,false,true,0,0);assert(led.grb[0]==6&&led.grb[1]==6&&led.grb[2]==6);
  puts("PASS ring program markers, paused status, autorun OFF one/ON all, saved flash");
  puts("ALL 0.1.5 NATIVE CHECKS PASSED (no hardware IO tested)");
}
