import { AudioNode, BaseAudioContext } from "react-native-audio-api";
import {
  IAudioNode,
  IBaseAudioContext,
} from "react-native-audio-api/lib/typescript/interfaces";

export interface IMyOscillatorNode extends IAudioNode {
  frequency: number;
  volume: number;
}

export interface IMartigliNode extends IAudioNode {
  mf0: number;
  ma: number;
  mp0: number;
  mp1: number;
  md: number;
  inhaleDur: number;
  exhaleDur: number;
  waveformM: number;
  volume: number;
  panOsc: number;
  panOscPeriod: number;
  panOscTrans: number;
  animationValue: number;
  isPaused: boolean;
  shouldStart: boolean;
  shouldPause: boolean;
  shouldResume: boolean;
}

export class MyOscillatorNode extends AudioNode {
  constructor(context: BaseAudioContext, node: IMyOscillatorNode) {
    super(context, node);
  }

  public set frequency(value: number) {
    (this.node as IMyOscillatorNode).frequency = value;
  }

  public get frequency(): number {
    return (this.node as IMyOscillatorNode).frequency;
  }

  public set volume(value: number) {
    (this.node as IMyOscillatorNode).volume = value;
  }

  public get volume(): number {
    return (this.node as IMyOscillatorNode).volume;
  }
}

export class MartigliNode extends AudioNode {
  constructor(context: BaseAudioContext, node: IMartigliNode) {
    super(context, node);
  }

  public set mf0(value: number) {
    (this.node as IMartigliNode).mf0 = value;
  }

  public get mf0(): number {
    return (this.node as IMartigliNode).mf0;
  }

  public set ma(value: number) {
    (this.node as IMartigliNode).ma = value;
  }

  public get ma(): number {
    return (this.node as IMartigliNode).ma;
  }

  public set mp0(value: number) {
    (this.node as IMartigliNode).mp0 = value;
  }

  public get mp0(): number {
    return (this.node as IMartigliNode).mp0;
  }

  public set mp1(value: number) {
    (this.node as IMartigliNode).mp1 = value;
  }

  public get mp1(): number {
    return (this.node as IMartigliNode).mp1;
  }

  public set md(value: number) {
    (this.node as IMartigliNode).md = value;
  }

  public get md(): number {
    return (this.node as IMartigliNode).md;
  }

  public set inhaleDur(value: number) {
    (this.node as IMartigliNode).inhaleDur = value;
  }

  public get inhaleDur(): number {
    return (this.node as IMartigliNode).inhaleDur;
  }

  public set exhaleDur(value: number) {
    (this.node as IMartigliNode).exhaleDur = value;
  }

  public get exhaleDur(): number {
    return (this.node as IMartigliNode).exhaleDur;
  }

  public set waveformM(value: number) {
    (this.node as IMartigliNode).waveformM = value;
  }

  public get waveformM(): number {
    return (this.node as IMartigliNode).waveformM;
  }

  public set volume(value: number) {
    (this.node as IMartigliNode).volume = value;
  }

  public get volume(): number {
    return (this.node as IMartigliNode).volume;
  }

  public set panOsc(value: number) {
    (this.node as IMartigliNode).panOsc = value;
  }

  public get panOsc(): number {
    return (this.node as IMartigliNode).panOsc;
  }

  public set panOscPeriod(value: number) {
    (this.node as IMartigliNode).panOscPeriod = value;
  }

  public get panOscPeriod(): number {
    return (this.node as IMartigliNode).panOscPeriod;
  }

  public set panOscTrans(value: number) {
    (this.node as IMartigliNode).panOscTrans = value;
  }

  public get panOscTrans(): number {
    return (this.node as IMartigliNode).panOscTrans;
  }

  public get animationValue(): number {
    return (this.node as IMartigliNode).animationValue;
  }

  public get isPaused(): boolean {
    return (this.node as IMartigliNode).isPaused;
  }

  public start(): void {
    (this.node as IMartigliNode).shouldStart = true;
  }

  public pause(): void {
    (this.node as IMartigliNode).shouldPause = true;
  }

  public resume(): void {
    (this.node as IMartigliNode).shouldResume = true;
  }
}

declare global {
  var createMyOscillatorNode: (context: IBaseAudioContext) => IMyOscillatorNode;
  var createMartigliNode: (context: IBaseAudioContext) => IMartigliNode;
}
