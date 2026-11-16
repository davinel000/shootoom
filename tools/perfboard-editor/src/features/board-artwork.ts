import {BoardState} from '../state/BoardState';
import {Canvas} from '../state/Canvas';
import {AdvancedComponentState} from '../state/AdvancedComponentState';

export function drawBoardArtwork() {
  const board = BoardState.artwork;
  if (!board) return;
  const ctx = Canvas.ctx;
  ctx.save();
  ctx.lineCap = 'round';
  ctx.strokeStyle = '#b59a59';
  ctx.lineWidth = 8;
  for (const track of board.copper) {
    ctx.beginPath();
    track.points.forEach((p, i) => i ? ctx.lineTo(p.x, p.y) : ctx.moveTo(p.x, p.y));
    ctx.stroke();
  }
  ctx.strokeStyle = '#ff4f64';
  ctx.lineWidth = 4;
  for (const cut of board.cuts || []) {
    ctx.beginPath();
    ctx.moveTo(cut.x - 8, cut.y - 8); ctx.lineTo(cut.x + 8, cut.y + 8);
    ctx.moveTo(cut.x - 8, cut.y + 8); ctx.lineTo(cut.x + 8, cut.y - 8);
    ctx.stroke();
  }
  ctx.font = '16px Arial';
  ctx.fillStyle = '#ffffff';
  ctx.textAlign = 'center';
  for (const label of board.labels) {
    if (label.headerId !== undefined) {
      const header = AdvancedComponentState.placedComponents.find(c => c.id === label.headerId);
      if (header) {
        const horizontal=header.rows===1 && header.cols>1 && (header.rotationAngle===0 || header.rotationAngle===180);
        const dy=board.flipAxis==='vertical' && horizontal ? -32 : (label.offsetY || 0);
        ctx.fillText(label.text, header.anchorDot.x + (label.offsetX || 0), header.anchorDot.y + dy);
      }
    } else ctx.fillText(label.text, label.x, label.y);
  }
  // Mechanical latch marks only: these are not wires or copper cuts.
  for(const mark of board.connectorLatches || []) {
    const header=AdvancedComponentState.placedComponents.find(h=>h.id===mark.headerId);
    if(!header)continue;
    const r=header.getBodyRect(),cx=r.x+r.w/2,cy=r.y+r.h/2;
    const dir=mark.direction;
    const nx=dir==='right'?1:dir==='left'?-1:0,ny=dir==='down'?1:dir==='up'?-1:0;
    const ex=nx===0?cx:nx>0?r.x+r.w:r.x,ey=ny===0?cy:ny>0?r.y+r.h:r.y;
    ctx.save();ctx.strokeStyle='#38bdf8';ctx.lineWidth=3;
    if(mark.source==='recommended')ctx.setLineDash([4,3]);else ctx.setLineDash([]);
    ctx.beginPath();
    for(const t of [-9,9]) {
      ctx.moveTo(ex+nx*3-ny*t,ey+ny*3+nx*t);
      ctx.lineTo(ex+nx*15-ny*t,ey+ny*15+nx*t);
    }
    ctx.stroke();ctx.restore();
  }
  ctx.setLineDash([8, 6]);
  ctx.strokeStyle = '#7dd3fc';
  for (const item of board.reservations) {
    ctx.strokeRect(item.x, item.y, item.width, item.height);
    ctx.fillText(item.text, item.x + item.width / 2, item.y + item.height / 2);
  }
  ctx.restore();
}
