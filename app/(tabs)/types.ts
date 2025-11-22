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
  isOn: boolean;
  currentInhaleDur: number;
  currentExhaleDur: number;
  currentPeriod: number;
  shouldStart: boolean;
  shouldPause: boolean;
  shouldResume: boolean;
  shouldStop: boolean;
}

export interface IBinauralNode extends IAudioNode {
  fl: number;
  fr: number;
  waveformL: number;
  waveformR: number;
  volume: number;
  panOsc: number;
  panOscPeriod: number;
  panOscTrans: number;
  martigliAnimationValue: number;
  isPaused: boolean;
  shouldStart: boolean;
  shouldPause: boolean;
  shouldResume: boolean;
  shouldStop: boolean;
  frameCount: number;
}

export interface ISymmetryNode extends IAudioNode {
  f0: number;
  noctaves: number;
  nnotes: number;
  d: number;
  waveform: number;
  permfunc: number;
  volume: number;
  shouldStart: boolean;
  shouldPause: boolean;
  shouldResume: boolean;
  shouldStop: boolean;
  frameCount: number;
}

export interface IMartigliBinauralNode extends IAudioNode {
  fl: number;
  fr: number;
  waveformL: number;
  waveformR: number;
  ma: number;
  mp0: number;
  mp1: number;
  md: number;
  inhaleDur: number;
  exhaleDur: number;
  volume: number;
  panOsc: number;
  panOscPeriod: number;
  panOscTrans: number;
  isOn: boolean;
  isPaused: boolean;
  animationValue: number;
  currentInhaleDur: number;
  currentExhaleDur: number;
  currentPeriod: number;
  shouldStart: boolean;
  shouldPause: boolean;
  shouldResume: boolean;
  shouldStop: boolean;
}

// Helper to create property accessors
const createProp = (node: IAudioNode, name: string) => ({
  get: () => (node as any)[name],
  set: (v: any) => ((node as any)[name] = v),
});

export class MyOscillatorNode extends AudioNode {
  constructor(context: BaseAudioContext, node: IMyOscillatorNode) {
    super(context, node);
  }
  get frequency() {
    return (this.node as IMyOscillatorNode).frequency;
  }
  set frequency(v: number) {
    (this.node as IMyOscillatorNode).frequency = v;
  }
  get volume() {
    return (this.node as IMyOscillatorNode).volume;
  }
  set volume(v: number) {
    (this.node as IMyOscillatorNode).volume = v;
  }
}

export class MartigliNode extends AudioNode {
  private n: IMartigliNode;

  constructor(context: BaseAudioContext, node: IMartigliNode) {
    super(context, node);
    this.n = node;
  }

  get mf0() {
    return this.n.mf0;
  }
  set mf0(v: number) {
    this.n.mf0 = v;
  }
  get ma() {
    return this.n.ma;
  }
  set ma(v: number) {
    this.n.ma = v;
  }
  get mp0() {
    return this.n.mp0;
  }
  set mp0(v: number) {
    this.n.mp0 = v;
  }
  get mp1() {
    return this.n.mp1;
  }
  set mp1(v: number) {
    this.n.mp1 = v;
  }
  get md() {
    return this.n.md;
  }
  set md(v: number) {
    this.n.md = v;
  }
  get inhaleDur() {
    return this.n.inhaleDur;
  }
  set inhaleDur(v: number) {
    this.n.inhaleDur = v;
  }
  get exhaleDur() {
    return this.n.exhaleDur;
  }
  set exhaleDur(v: number) {
    this.n.exhaleDur = v;
  }
  get waveformM() {
    return this.n.waveformM;
  }
  set waveformM(v: number) {
    this.n.waveformM = v;
  }
  get volume() {
    return this.n.volume;
  }
  set volume(v: number) {
    this.n.volume = v;
  }
  get panOsc() {
    return this.n.panOsc;
  }
  set panOsc(v: number) {
    this.n.panOsc = v;
  }
  get panOscPeriod() {
    return this.n.panOscPeriod;
  }
  set panOscPeriod(v: number) {
    this.n.panOscPeriod = v;
  }
  get panOscTrans() {
    return this.n.panOscTrans;
  }
  set panOscTrans(v: number) {
    this.n.panOscTrans = v;
  }
  get animationValue() {
    return this.n.animationValue;
  }
  get isPaused() {
    return this.n.isPaused;
  }
  get isOn() {
    return this.n.isOn;
  }
  set isOn(v: boolean) {
    this.n.isOn = v;
  }
  get currentInhaleDur() {
    return this.n.currentInhaleDur;
  }
  get currentExhaleDur() {
    return this.n.currentExhaleDur;
  }
  get currentPeriod() {
    return this.n.currentPeriod;
  }

  start() {
    this.n.shouldStart = true;
  }
  pause() {
    this.n.shouldPause = true;
  }
  resume() {
    this.n.shouldResume = true;
  }
  stop() {
    this.n.shouldStop = true;
  }
}

export class BinauralNode extends AudioNode {
  private n: IBinauralNode;

  constructor(context: BaseAudioContext, node: IBinauralNode) {
    super(context, node);
    this.n = node;
  }

  get fl() {
    return this.n.fl;
  }
  set fl(v: number) {
    this.n.fl = v;
  }
  get fr() {
    return this.n.fr;
  }
  set fr(v: number) {
    this.n.fr = v;
  }
  get waveformL() {
    return this.n.waveformL;
  }
  set waveformL(v: number) {
    this.n.waveformL = v;
  }
  get waveformR() {
    return this.n.waveformR;
  }
  set waveformR(v: number) {
    this.n.waveformR = v;
  }
  get volume() {
    return this.n.volume;
  }
  set volume(v: number) {
    this.n.volume = v;
  }
  get panOsc() {
    return this.n.panOsc;
  }
  set panOsc(v: number) {
    this.n.panOsc = v;
  }
  get panOscPeriod() {
    return this.n.panOscPeriod;
  }
  set panOscPeriod(v: number) {
    this.n.panOscPeriod = v;
  }
  get panOscTrans() {
    return this.n.panOscTrans;
  }
  set panOscTrans(v: number) {
    this.n.panOscTrans = v;
  }
  get martigliAnimationValue() {
    return this.n.martigliAnimationValue;
  }
  set martigliAnimationValue(v: number) {
    this.n.martigliAnimationValue = v;
  }
  get isPaused() {
    return this.n.isPaused;
  }

  start() {
    this.n.shouldStart = true;
  }
  pause() {
    this.n.shouldPause = true;
  }
  resume() {
    this.n.shouldResume = true;
  }
  stop() {
    this.n.shouldStop = true;
  }
}

export class SymmetryNode extends AudioNode {
  private n: ISymmetryNode;

  constructor(context: BaseAudioContext, node: ISymmetryNode) {
    super(context, node);
    this.n = node;
  }

  get f0() {
    return this.n.f0;
  }
  set f0(v: number) {
    this.n.f0 = v;
  }
  get noctaves() {
    return this.n.noctaves;
  }
  set noctaves(v: number) {
    this.n.noctaves = v;
  }
  get nnotes() {
    return this.n.nnotes;
  }
  set nnotes(v: number) {
    this.n.nnotes = v;
  }
  get d() {
    return this.n.d;
  }
  set d(v: number) {
    this.n.d = v;
  }
  get waveform() {
    return this.n.waveform;
  }
  set waveform(v: number) {
    this.n.waveform = v;
  }
  get permfunc() {
    return this.n.permfunc;
  }
  set permfunc(v: number) {
    this.n.permfunc = v;
  }
  get volume() {
    return this.n.volume;
  }
  set volume(v: number) {
    this.n.volume = v;
  }

  start() {
    this.n.shouldStart = true;
  }
  pause() {
    this.n.shouldPause = true;
  }
  resume() {
    this.n.shouldResume = true;
  }
  stop() {
    this.n.shouldStop = true;
  }
}

export class MartigliBinauralNode extends AudioNode {
  private n: IMartigliBinauralNode;

  constructor(context: BaseAudioContext, node: IMartigliBinauralNode) {
    super(context, node);
    this.n = node;
  }

  get fl() {
    return this.n.fl;
  }
  set fl(v: number) {
    this.n.fl = v;
  }
  get fr() {
    return this.n.fr;
  }
  set fr(v: number) {
    this.n.fr = v;
  }
  get waveformL() {
    return this.n.waveformL;
  }
  set waveformL(v: number) {
    this.n.waveformL = v;
  }
  get waveformR() {
    return this.n.waveformR;
  }
  set waveformR(v: number) {
    this.n.waveformR = v;
  }
  get ma() {
    return this.n.ma;
  }
  set ma(v: number) {
    this.n.ma = v;
  }
  get mp0() {
    return this.n.mp0;
  }
  set mp0(v: number) {
    this.n.mp0 = v;
  }
  get mp1() {
    return this.n.mp1;
  }
  set mp1(v: number) {
    this.n.mp1 = v;
  }
  get md() {
    return this.n.md;
  }
  set md(v: number) {
    this.n.md = v;
  }
  get inhaleDur() {
    return this.n.inhaleDur;
  }
  set inhaleDur(v: number) {
    this.n.inhaleDur = v;
  }
  get exhaleDur() {
    return this.n.exhaleDur;
  }
  set exhaleDur(v: number) {
    this.n.exhaleDur = v;
  }
  get volume() {
    return this.n.volume;
  }
  set volume(v: number) {
    this.n.volume = v;
  }
  get panOsc() {
    return this.n.panOsc;
  }
  set panOsc(v: number) {
    this.n.panOsc = v;
  }
  get panOscPeriod() {
    return this.n.panOscPeriod;
  }
  set panOscPeriod(v: number) {
    this.n.panOscPeriod = v;
  }
  get panOscTrans() {
    return this.n.panOscTrans;
  }
  set panOscTrans(v: number) {
    this.n.panOscTrans = v;
  }
  get isOn() {
    return this.n.isOn;
  }
  set isOn(v: boolean) {
    this.n.isOn = v;
  }
  get isPaused() {
    return this.n.isPaused;
  }
  get animationValue() {
    return this.n.animationValue;
  }
  get currentInhaleDur() {
    return this.n.currentInhaleDur;
  }
  get currentExhaleDur() {
    return this.n.currentExhaleDur;
  }
  get currentPeriod() {
    return this.n.currentPeriod;
  }

  start() {
    this.n.shouldStart = true;
  }
  pause() {
    this.n.shouldPause = true;
  }
  resume() {
    this.n.shouldResume = true;
  }
  stop() {
    this.n.shouldStop = true;
  }
}

declare global {
  var createMyOscillatorNode: (context: IBaseAudioContext) => IMyOscillatorNode;
  var createMartigliNode: (context: IBaseAudioContext) => IMartigliNode;
  var createBinauralNode: (context: IBaseAudioContext) => IBinauralNode;
  var createSymmetryNode: (context: IBaseAudioContext) => ISymmetryNode;
  var createMartigliBinauralNode: (context: IBaseAudioContext) => IMartigliBinauralNode;
}
