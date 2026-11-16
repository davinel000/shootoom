/** Same physical holes and pin identities, viewed from the other face.
 * Follow the project's explicitly selected way of turning the board over.
 */
export function mirrorProjectView(source: any): any {
  const p=JSON.parse(JSON.stringify(source));
  const xs=p.dots.map((d:any)=>d.x);
  const width=Math.min(...xs)+Math.max(...xs);
  const ys=p.dots.map((d:any)=>d.y);
  const height=Math.min(...ys)+Math.max(...ys);
  const vertical=p.boardArtwork?.flipAxis==='vertical';
  const reflect=(pt:any)=>{if(vertical)pt.y=height-pt.y;else pt.x=width-pt.x;};
  for(const d of p.dots) reflect(d);
  for(const w of p.lines) for(const key of ['start','end']) reflect(w[key]);
  for(const ic of p.placedIcs||[]) {
    if(vertical)ic.topLeftDotY=height-ic.topLeftDotY-(ic.heightPin-1)*50;
    else ic.topLeftDotX=width-ic.topLeftDotX-(ic.widthPin-1)*50;
    ic.viewedFromBack=!ic.viewedFromBack;
    ic.backViewAxis=vertical?'vertical':'horizontal';
  }
  for(const c of p.placedStandardComponents||[]) {
    if(vertical){c.startDotY=height-c.startDotY;c.endDotY=height-c.endDotY;}
    else {c.startDotX=width-c.startDotX;c.endDotX=width-c.endDotX;}
  }
  for(const c of p.placedAdvancedComponents||[]) {
    if(vertical)c.anchorDotY=height-c.anchorDotY;
    else c.anchorDotX=width-c.anchorDotX;
    c.rotationAngle=(vertical?360-c.rotationAngle:180-c.rotationAngle+360)%360;
  }
  const a=p.boardArtwork;
  if(a) {
    a.viewSide=a.viewSide==='wiring'?'components':'wiring';
    for(const mark of a.connectorLatches||[]) {
      const swapped=vertical?{up:'down',down:'up'}:{left:'right',right:'left'};
      mark.direction=(swapped as any)[mark.direction]||mark.direction;
    }
    for(const track of a.copper) for(const pt of track.points) reflect(pt);
    for(const pt of a.cuts||[]) reflect(pt);
    for(const l of a.labels) {
      // Footer is outside the physical board and stays below either view.
      if(l.y<=height && l.y>=0)reflect(l);
      if(vertical && l.offsetY!==undefined)l.offsetY=-l.offsetY;
      if(!vertical && l.offsetX!==undefined)l.offsetX=-l.offsetX;
      if(l.textOnComponents && l.textOnWiring)l.text=a.viewSide==='wiring'?l.textOnWiring:l.textOnComponents;
    }
    for(const r of a.reservations||[]) {
      if(vertical)r.y=height-r.y-r.height;else r.x=width-r.x-r.width;
    }
  }
  return p;
}
