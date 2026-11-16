import {ILine} from "./line.interface";
import {IDot} from "./dot.interface";
import {Ic} from "../features/ic";
import {BoardArtwork} from '../state/BoardState';

export interface IProjectSave {
  boardArtwork?: BoardArtwork;
  lines: ILine[];
  dots: IDot[];
  ICs: Ic[];
  placedIcs?: any[];
  placedStandardComponents?: any[];
  placedAdvancedComponents?: any[];
  canvas: {width: number, height: number};
}
