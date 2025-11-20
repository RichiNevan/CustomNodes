import { AudioNode, BaseAudioContext } from "react-native-audio-api";
import {
  IAudioNode,
  IBaseAudioContext,
} from "react-native-audio-api/lib/typescript/interfaces";

export interface IMyOscillatorNode extends IAudioNode {
  frequency: number;
  volume: number;
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

declare global {
  var createMyOscillatorNode: (context: IBaseAudioContext) => IMyOscillatorNode;
}
