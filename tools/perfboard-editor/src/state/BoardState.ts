export interface BoardArtwork {
  name: string;
  viewSide?: 'components' | 'wiring';
  faceLayout?: string;
  factoryComponentFace?: 'printed' | 'reverse';
  flipAxis?: 'horizontal' | 'vertical';
  connectorLatches?: {headerId:number; direction:'up'|'down'|'left'|'right'; source:'owner'|'recommended'}[];
  copper: {points: {x: number; y: number}[]}[];
  cuts?: {x: number; y: number}[];
  labels: {x: number; y: number; text: string; headerId?: number; offsetX?: number; offsetY?: number}[];
  reservations: {x: number; y: number; width: number; height: number; text: string}[];
}

// Existing PCB copper is separate from editable hookup wires.
export class BoardState {
  static artwork: BoardArtwork | undefined;
}
