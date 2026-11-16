import {physicalPins} from "./physical-pins";
import {Canvas} from "../state/Canvas";
import {DotState} from "../state/DotState";
import {IcState} from "../state/IcState";
import {IDot} from "../interfaces/dot.interface";
import {ShortcutRegistry} from "./shortcut-keys";
import {Utils} from "../utils/utils";
import {redrawCanvas} from "./draw-canvas";
import {StandardComponentState} from "../state/StandardComponentState";
import {AdvancedComponentState} from "../state/AdvancedComponentState";
import {disarmWire} from "./wire";
import {findNearestDot} from "./dot-lookup";


export class Ic{
  static IC_CONTAINER: Ic[] = [];

  public id = Math.random() * 100;
  public isCustom?: boolean = false;
  public rotationAngle: number = 0; // 0, 90, 180, 270
  // A view through the PCB, never an instruction to flip a physical chip.
  public viewedFromBack = false;
  public backViewAxis: 'horizontal' | 'vertical' = 'horizontal';
  topLeftDot: IDot | null = null;

  constructor(
    public widthPin: number,
    public heightPin: number,
    public pinDescription: Record<number, string>,
    public name: string,
    isCustom: boolean = false
  ) {
    this.isCustom = isCustom;
  }

  static add(ic: Ic, saveToStorage: boolean = false){
    this.IC_CONTAINER.push(ic);
    if (saveToStorage) {
      this.saveCustomIcsToLocalStorage();
    }
    this.showICs();
  }

  static showICs(){
    Utils.getSafeHtmlElement("ic-items").innerHTML = Ic.IC_CONTAINER.map((item)=>{
      const editBtn = item.isCustom
        ? `<span onclick="event.stopPropagation(); editCustomIc('${item.id}')" title="Edit custom component" style="margin-left:6px;cursor:pointer;color:#38bdf8;font-weight:bold;">✎</span>`
        : '';
      const deleteBtn = item.isCustom
        ? `<span onclick="event.stopPropagation(); deleteCustomIc('${item.id}')" title="Delete custom component" style="margin-left:6px;cursor:pointer;color:#f87171;font-weight:bold;">✕</span>`
        : '';
      return `<button class="btn btn-accent" style="padding:0.3rem 0.6rem; font-size:0.75rem;" onclick='selectIc("${item.id}")'>📦 ${item.name}${editBtn}${deleteBtn}</button>`;
    }).join(" ");
  }

  static saveCustomIcsToLocalStorage() {
    try {
      const customIcs = Ic.IC_CONTAINER.filter(ic => ic.isCustom).map(ic => ({
        id: ic.id,
        name: ic.name,
        widthPin: ic.widthPin,
        heightPin: ic.heightPin,
        pinDescription: ic.pinDescription,
        isCustom: true
      }));
      localStorage.setItem('custom_ics', JSON.stringify(customIcs));
    } catch (e) {
      console.error("Failed to save custom ICs to localStorage", e);
    }
  }

  static loadCustomIcsFromLocalStorage() {
    try {
      const stored = localStorage.getItem('custom_ics');
      if (!stored) return;
      const customIcs = JSON.parse(stored) as Array<{
        id: number;
        name: string;
        widthPin: number;
        heightPin: number;
        pinDescription: Record<number, string>;
      }>;
      for (const data of customIcs) {
        if (!Ic.IC_CONTAINER.some(ic => String(ic.id) === String(data.id))) {
          const newIc = new Ic(data.widthPin, data.heightPin, data.pinDescription || {}, data.name, true);
          newIc.id = data.id;
          Ic.IC_CONTAINER.push(newIc);
        }
      }
      this.showICs();
    } catch (e) {
      console.error("Failed to load custom ICs from localStorage", e);
    }
  }

  updatePosition(x: number, y: number){
    this.topLeftDot = findNearestDot(x, y) ?? null;
  }

  drawBody(){
    if (!this.topLeftDot) return;
    Canvas.ctx.beginPath();
    const isSelected = this === IcState.selectedPlacedIc;

    Canvas.ctx.fillStyle = isSelected ? "rgba(30,58,138,0.9)" : this.viewedFromBack ? "rgba(17,24,39,0.3)" : "rgba(17,24,39,0.85)";
    Canvas.ctx.strokeStyle = isSelected ? "#38bdf8" : "#475569";
    Canvas.ctx.lineWidth = isSelected ? 3 : 2;

    const w = 50 * (this.widthPin - 1);
    const h = 50 * (this.heightPin - 1);
    Canvas.ctx.rect(this.topLeftDot.x, this.topLeftDot.y, w, h);
    Canvas.ctx.stroke();
    Canvas.ctx.fill();

    // Mark the same physical pin 1 used by labels and hit testing.
    const p1 = this.getRealPinPositions().find(p => p.pin === 1)!;
    const notchRadius = 6;
    const notch = this.rotationAngle === 0 ? [w/2,0] :
      this.rotationAngle === 90 ? [w,h/2] :
      this.rotationAngle === 180 ? [w/2,h] : [0,h/2];
    Canvas.ctx.beginPath();
    Canvas.ctx.arc(this.topLeftDot.x+(this.viewedFromBack && this.backViewAxis==='horizontal'?w-notch[0]:notch[0]), this.topLeftDot.y+(this.viewedFromBack && this.backViewAxis==='vertical'?h-notch[1]:notch[1]), notchRadius, 0, Math.PI*2);
    Canvas.ctx.fillStyle = "#38bdf8"; Canvas.ctx.fill();
    Canvas.ctx.beginPath();
    Canvas.ctx.arc(p1.x+(p1.x===this.topLeftDot.x?10:-10),p1.y+(p1.y===this.topLeftDot.y?10:-10),3,0,Math.PI*2);
    Canvas.ctx.fill();

    if (isSelected) {
      // Draw selection corner handles
      Canvas.ctx.fillStyle = "#38bdf8";
      Canvas.ctx.fillRect(this.topLeftDot.x - 4, this.topLeftDot.y - 4, 8, 8);
      Canvas.ctx.fillRect(this.topLeftDot.x + w - 4, this.topLeftDot.y - 4, 8, 8);
      Canvas.ctx.fillRect(this.topLeftDot.x - 4, this.topLeftDot.y + h - 4, 8, 8);
      Canvas.ctx.fillRect(this.topLeftDot.x + w - 4, this.topLeftDot.y + h - 4, 8, 8);
    }
  }

  drawPinLabels() {
    if (!this.topLeftDot) return;
    Canvas.ctx.save();
    Canvas.ctx.font = "600 9px monospace, sans-serif";
    Canvas.ctx.fillStyle = "#cbd5e1";
    const horizontal = this.rotationAngle === 90 || this.rotationAngle === 270;
    for (const p of this.getRealPinPositions()) {
      const desc = this.pinDescription[p.pin];
      const label = desc ? `${p.pin}:${desc}` : `${p.pin}`;
      Canvas.ctx.textAlign = horizontal ? "center" : p.x===this.topLeftDot.x ? "left" : "right";
      Canvas.ctx.fillText(label, p.x+(horizontal?0:p.x===this.topLeftDot.x?10:-10), p.y+(horizontal?p.y===this.topLeftDot.y?16:-10:3));
    }
    Canvas.ctx.restore();
  }

  getRealPinPositions(): {pin: number, x: number, y: number}[] {
    if (!this.topLeftDot) return [];
    const w=(this.widthPin-1)*50,h=(this.heightPin-1)*50;
    return physicalPins(this.widthPin,this.heightPin,this.rotationAngle).map(p=>({pin:p.pin,x:(this.viewedFromBack && this.backViewAxis==='horizontal'?w-p.x:p.x)+this.topLeftDot!.x,y:(this.viewedFromBack && this.backViewAxis==='vertical'?h-p.y:p.y)+this.topLeftDot!.y}));
  }

  drawPinMarkers() {
    const positions = this.getRealPinPositions();
    Canvas.ctx.save();
    Canvas.ctx.strokeStyle = "#38bdf8";
    Canvas.ctx.lineWidth = 1.5;
    for (const {x, y} of positions) {
      Canvas.ctx.beginPath();
      Canvas.ctx.arc(x, y, 5, 0, Math.PI * 2);
      Canvas.ctx.stroke();
    }
    Canvas.ctx.restore();
  }

  drawLabel(){
    if (!this.topLeftDot) return;
    const isSelected = this === IcState.selectedPlacedIc;
    const w = 50 * (this.widthPin - 1);
    const h = 50 * (this.heightPin - 1);
    const centerX = this.topLeftDot.x + (w / 2);
    const centerY = this.topLeftDot.y + (h / 2);

    Canvas.ctx.save();
    Canvas.ctx.font = "bold 15px Inter, Arial";
    const textWidth = Canvas.ctx.measureText(this.name).width;
    const badgeW = textWidth + 20;
    const badgeH = 26;

    // Draw background badge pill behind label
    Canvas.ctx.beginPath();
    Canvas.ctx.fillStyle = "#0f172a";
    Canvas.ctx.strokeStyle = isSelected ? "#38bdf8" : "#334155";
    Canvas.ctx.lineWidth = 1.5;
    const rectX = centerX - (badgeW / 2);
    const rectY = centerY - (badgeH / 2);
    Canvas.ctx.rect(rectX, rectY, badgeW, badgeH);
    Canvas.ctx.fill();
    Canvas.ctx.stroke();

    // Draw high-contrast text
    Canvas.ctx.fillStyle = isSelected ? "#38bdf8" : "#ffffff";
    Canvas.ctx.textAlign = "center";
    Canvas.ctx.textBaseline = "middle";
    Canvas.ctx.fillText(this.name, centerX, centerY + 1);
    Canvas.ctx.restore();

    this.drawPinMarkers();
    this.drawPinLabels();
  }

  draw(){
    this.drawBody();
    this.drawLabel();
  }

  containsPoint(x: number, y: number): boolean {
    if (!this.topLeftDot) return false;
    const w = 50 * (this.widthPin - 1);
    const h = 50 * (this.heightPin - 1);
    return (
      x >= this.topLeftDot.x &&
      x <= this.topLeftDot.x + w &&
      y >= this.topLeftDot.y &&
      y <= this.topLeftDot.y + h
    );
  }

  clone(): Ic {
    const copy = new Ic(this.widthPin, this.heightPin, { ...this.pinDescription }, this.name, this.isCustom);
    copy.viewedFromBack=this.viewedFromBack;
    copy.backViewAxis=this.backViewAxis;
    return copy;
  }

  getPinPositionOnIC(dot: IDot) {
    const p=this.getRealPinPositions().find(p=>p.x===dot.x && p.y===dot.y);
    return p ? {pin:p.pin,info:this.pinDescription[p.pin]} : null;
  }

  getPinNumber(dot: IDot){ return this.getPinPositionOnIC(dot); }

  rotate(){
    this.rotationAngle = ((this.rotationAngle + 90) % 360) as 0 | 90 | 180 | 270;
    const tmp = this.widthPin;
    this.widthPin = this.heightPin;
    this.heightPin = tmp;
  }
}

// Predefined IC components
Ic.add(new Ic(4, 4, {1: "GND", 2: "TRIG", 3: "OUT", 4: "RESET", 5: "CTRL", 6: "THRESH", 7: "DISCH", 8: "VCC"}, "NE555 Timer"));
Ic.add(new Ic(4, 7, {1: "1A", 2: "1B", 3: "1Y", 4: "2A", 5: "2B", 6: "2Y", 7: "GND", 14: "VCC"}, "DIP-14 Logic"));
Ic.add(new Ic(4, 8, {1: "EN", 2: "1D", 3: "1Q", 4: "2D", 5: "2Q", 8: "GND", 16: "VCC"}, "DIP-16 Logic"));
Ic.add(new Ic(4, 14, {1: "RESET", 2: "RX", 3: "TX", 7: "VCC", 8: "GND", 22: "GND", 20: "AVCC"}, "ATmega328P"));
// Arduino Nano: 0.6" between header rows, 15 pins per side. Labels as silkscreened on the board.
Ic.add(new Ic(7, 15, {
  1: "D13", 2: "3V3", 3: "REF", 4: "A0", 5: "A1", 6: "A2", 7: "A3", 8: "A4",
  9: "A5", 10: "A6", 11: "A7", 12: "5V", 13: "RST", 14: "GND", 15: "VIN",
  16: "TX1", 17: "RX0", 18: "RST", 19: "GND", 20: "D2", 21: "D3", 22: "D4",
  23: "D5", 24: "D6", 25: "D7", 26: "D8", 27: "D9", 28: "D10", 29: "D11", 30: "D12"
}, "Arduino Nano"));

// Load custom ICs from localStorage on load
Ic.loadCustomIcsFromLocalStorage();

export function deleteCustomIc(id: number | string) {
  const index = Ic.IC_CONTAINER.findIndex(ic => String(ic.id) === String(id));
  if (index > -1) {
    Ic.IC_CONTAINER.splice(index, 1);
    Ic.saveCustomIcsToLocalStorage();
    Ic.showICs();
  }
}

export function selectIc(id: number | string){
  const ic = Ic.IC_CONTAINER.find(ic => String(ic.id) === String(id));
  if (!ic){
    console.error(`Ic with id: ${id} not found`);
    return;
  }
  IcState.selectedIc = ic;
  StandardComponentState.armedDefinitionId = undefined;
  StandardComponentState.pendingStartDot = undefined;
  AdvancedComponentState.armedDefinitionId = undefined;
  AdvancedComponentState.pendingAnchorDot = undefined;
  disarmWire();
}

export function rotateSelectedIc() {
  if (IcState.selectedPlacedIc) {
    IcState.selectedPlacedIc.rotate();
    redrawCanvas();
    return;
  }
  if (AdvancedComponentState.selectedPlacedComponent) {
    AdvancedComponentState.selectedPlacedComponent.rotate();
    redrawCanvas();
    return;
  }
  if (IcState.selectedIc) {
    IcState.selectedIc.rotate();
    redrawCanvas();
    return;
  }
  if (AdvancedComponentState.armedDefinitionId) {
    AdvancedComponentState.armedRotation = ((AdvancedComponentState.armedRotation + 90) % 360) as 0 | 90 | 180 | 270;
    redrawCanvas();
    return;
  }
  if (DotState.hoverDot) {
    const hoveredIc = IcState.placedIcs.find(ic => ic.containsPoint(DotState.hoverDot!.x, DotState.hoverDot!.y));
    if (hoveredIc) {
      hoveredIc.rotate();
      IcState.selectedPlacedIc = hoveredIc;
      redrawCanvas();
      return;
    }
    const hoveredAdvancedComponent = AdvancedComponentState.placedComponents.find(c => c.containsPoint(DotState.hoverDot!.x, DotState.hoverDot!.y));
    if (hoveredAdvancedComponent) {
      hoveredAdvancedComponent.rotate();
      AdvancedComponentState.selectedPlacedComponent = hoveredAdvancedComponent;
      redrawCanvas();
      return;
    }
  }
}

ShortcutRegistry.add({
  key: "r",
  description: "Rotate selected IC component.",
  event: rotateSelectedIc
});

(window as any).selectIc = selectIc;
(window as any).deleteCustomIc = deleteCustomIc;
(window as any).rotateSelectedIc = rotateSelectedIc;
