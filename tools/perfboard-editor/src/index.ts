import "./style.css";
import "./features/project/save-image";
import "./features/project/save-progress";
import "./features/project/save-project";
import "./features/select";
import "./features/project/load-project";
import "./features/hover";
import "./features/description";
import "./features/line";
import "./features/project/undo-redo";
import "./features/project/load-from-local-storage";
import "./features/shortcut-keys";
import "./features/dot";
import "./features/project/reset-project";
import "./features/ic-editor-modal";
import "./features/standard-components/standard-components-panel";
import "./features/advanced-components/advanced-components-panel";
import "./features/toolbar";
import "./features/wire";
import "./features/context-menu-actions";
import "./features/color-picker";
import "./features/view-controls";
import {resetCanvas} from "./features/reset-canvas";
import {createDotGrid, heightInput, widthInput} from "./features/project/resize-grid";
import {redrawCanvas} from "./features/draw-canvas";

createDotGrid(parseInt(widthInput.value || "10"), parseInt(heightInput.value || "10"));
resetCanvas();
redrawCanvas();

// Local integration: JSON is also the machine-readable interface to this editor.
import {loadProject} from './features/project/load-project';
import {getSaveJson} from './features/project/save-project';
import {fitToScreen} from './features/view-controls';
import {LineState} from './state/LineState';
import {IcState} from './state/IcState';
import {AdvancedComponentState} from './state/AdvancedComponentState';
import {mirrorProjectView} from './features/project/mirror-project-view';
const auditWarning = document.createElement('div');
auditWarning.id = 'shootoomHardwareHold';
auditWarning.style.cssText = 'position:fixed;left:0;right:0;top:68px;z-index:100;background:#8b0000;color:white;padding:12px 20px;font:bold 16px Arial;text-align:center;box-shadow:0 3px 8px #0008';
auditWarning.textContent = 'НЕ ВКЛЮЧАТЬ: SHOOTOOM v0.1–v0.7 отозваны. Физические ряды XIAO и DIP зеркальны. Поворот корпуса не исправляет схему. Старые номера не использовать для монтажа.';
document.body.append(auditWarning);
const sideButton=document.createElement('button');
sideButton.id='shootoomFlipSide';
sideButton.style.cssText='position:fixed;left:18px;bottom:18px;z-index:40;padding:12px;background:#e1eef2;color:#18334a;border:2px solid #28556b;border-radius:8px;font:bold 15px Arial';
sideButton.textContent='Перевернуть плату: другая сторона';
sideButton.addEventListener('click',()=>{
  loadProject(mirrorProjectView(getSaveJson())); fitToScreen(true);
});
document.body.append(sideButton);
window.addEventListener('shootoom-project-loaded', () => {
  const artwork = getSaveJson().boardArtwork;
  const name = artwork?.name || '';
  const wiring=artwork?.viewSide==='wiring';
  const ownerFrame=/^SHOOTOOM v0\.8\.[34] OWNER FIXED FRAME$/.test(name);
  const analogDraft=name==='SHOOTOOM ANALOG v0.1 DRAFT';
  const analogFixed=name==='SHOOTOOM ANALOG v0.2 FIXED';
  const analogPrinted=name==='SHOOTOOM ANALOG v0.2.1 PRINTED';
  const ownerVersion=name.includes('v0.8.4')?'v0.8.4':'v0.8.3';
  const vertical=name==='SHOOTOOM v0.8.2 VERTICAL TURNOVER';
  const revisedFaces=name==='SHOOTOOM v0.8.1 REVERSE FACTORY FACE';
  const revised = name === 'SHOOTOOM v0.8 PHYSICAL TOP VIEW';
  sideButton.hidden=!(revised || revisedFaces || vertical || ownerFrame || analogDraft || analogFixed || analogPrinted);
  sideButton.textContent=wiring?'Показать сторону корпусов':'Показать сторону перемычек';
  auditWarning.style.background = revised || revisedFaces || vertical || ownerFrame || analogDraft || analogFixed || analogPrinted ? '#694500' : '#8b0000';
  auditWarning.textContent = analogPrinted
    ? `АНАЛОГ v0.2.1 · ${wiring?'ОБРАТНАЯ СТОРОНА: A сверху / J снизу':'СТОРОНА С ПОДПИСЯМИ И КОРПУСАМИ: J сверху / A снизу'}. 1→17 слева направо. Детали и провода на прежних местах.`
    : analogFixed
    ? `АНАЛОГ v0.2 · ОДИН TL082 · ${wiring?'ПАЙКА: J сверху / A снизу; C1/C2 здесь':'КОРПУСА: A сверху / J снизу; выемка СЛЕВА'}. Кабели и допустимый вход драйвера ещё проверить.`
    : analogDraft
    ? `АНАЛОГ v0.1 · ПРОЕКТ: ${wiring?'ПАЙКА, J сверху / A снизу; резисторы и конденсаторы здесь':'КОРПУСА, A сверху / J снизу; U11 выемкой ВПРАВО, U12 ВЛЕВО'}. Питание ±15 В. Кабели и зазоры ещё проверить.`
    : ownerFrame
    ? wiring
      ? `${ownerVersion} · ПЕРЕМЫЧКИ: J1 слева / J30 справа сверху; A1 / A30 снизу. Левый и правый края сохраняются.`
      : `${ownerVersion} · КОРПУСА: XIAO СЛЕВА, HCT ЦЕНТР, MCP СПРАВА. Сверху A1…A30, снизу J1…J30. USB и выемки СЛЕВА.`
    : vertical
    ? wiring
      ? 'v0.8.2 · ПЕРЕМЫЧКИ, заводские надписи · 1→30, сверху J→A. Переворачивать СВЕРХУ ВНИЗ, сохраняя левый и правый края.'
      : 'v0.8.2 · ВСЕ КОРПУСА · 1→30, сверху A→J. USB и выемки СПРАВА на этом рисунке. Связи и места пайки сохранены.'
    : revisedFaces
    ? wiring
      ? 'v0.8.1 · ПЕРЕМЫЧКИ, заводская сторона с надписями · слева направо 1→30, сверху вниз J→A. Корпуса на другой стороне. Верх платы сохранять сверху.'
      : 'v0.8.1 · ВСЕ КОРПУСА, заводская обратная сторона · слева направо 30→1, сверху вниз J→A. USB и выемки СЛЕВА. Исходная модель проверена; изменения требуют проверки.'
    : revised
    ? 'v0.8 · ВИД СО СТОРОНЫ КОМПОНЕНТОВ · USB и обе выемки СЛЕВА. Модель проверена; пайку проверять по таблице до включения. Старые v0.1–v0.7 отозваны.'
    : 'НЕ ВКЛЮЧАТЬ: старые v0.1–v0.7 отозваны. Этот проект не является исправленной монтажной ревизией v0.8.';
});

const netPanel = document.createElement('label');
netPanel.style.cssText = 'position:fixed;right:18px;bottom:18px;z-index:30;background:#f0f6f8;color:#18334a;padding:12px;border-radius:8px;font:15px Arial;box-shadow:0 2px 12px #0005';
netPanel.textContent = 'Показать провода цепи: ';
const netSelect = document.createElement('select');
netSelect.id = 'shootoomNetSelect';
netSelect.style.cssText = 'font:15px Arial;padding:6px;max-width:240px';
netSelect.addEventListener('change', () => {LineState.visibleNet = netSelect.value; redrawCanvas();});
netPanel.append(netSelect); document.body.append(netPanel);
function refreshNetChoices() {
  netSelect.replaceChildren(new Option('Все цепи', ''));
  [...new Set(LineState.lines.map(line => line.net).filter(Boolean))].sort().forEach(net => netSelect.add(new Option(net!, net!)));
  LineState.visibleNet = ''; netPanel.hidden = netSelect.options.length === 1;
}
refreshNetChoices();
Object.assign(window, {shootoomPerfboard: {loadProject, getSaveJson, mirrorProjectView,
  advancedPinSnapshot:()=>AdvancedComponentState.placedComponents.flatMap(c=>c.getPinPositions().map((p,i)=>({...p,id:c.id,pin:i+1}))),
  physicalPinSnapshot:()=>IcState.placedIcs.flatMap(ic=>ic.getRealPinPositions().map(p=>({...p,ref:'U'+ic.id,name:ic.id===1?ic.pinDescription[p.pin]:String(p.pin)})))}});
const projectPath = new URLSearchParams(window.location.search).get('project');
if(projectPath==='shootoom-analog-v0.2.json' && !new URLSearchParams(window.location.search).has('archive')) {
  const current=new URL(window.location.href);current.searchParams.set('project','shootoom-analog-v0.2.1.json');window.location.replace(current.href);
}
if (projectPath && ['shootoom-physical-v0.8.json','shootoom-physical-v0.8.1.json','shootoom-physical-v0.8.2.json'].includes(projectPath) && !new URLSearchParams(window.location.search).has('archive')) {
  const current=new URL(window.location.href); current.searchParams.set('project','shootoom-physical-v0.8.3.json');
  window.location.replace(current.href);
}

if (projectPath) {
  const projectUrl = new URL(projectPath, window.location.href);
  if (projectUrl.origin === window.location.origin) {
    fetch(projectUrl).then(response => {
      if (!response.ok) throw new Error(`Project HTTP ${response.status}`);
      return response.json();
    }).then(project => {loadProject(project); refreshNetChoices(); fitToScreen(true);})
      .catch(error => {console.error(error); alert('Could not load project. Use Load Project to select its JSON file.');});
  }
}
