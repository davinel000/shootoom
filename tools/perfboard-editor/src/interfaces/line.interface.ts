import {IDot} from "./dot.interface";

export interface ILine {
  net?: string;
  start: IDot,
  end: IDot,
  color?: string;
  width?: number;
}
