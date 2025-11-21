const audiovisualPreset = {
  _id: "Meditazione",
  header: {
    med2: "Meditazione",
    onlyOnce: false,
    d: 900,
    isShownIndex: -1,
    vcontrol: true,
    creator: "fabbri",
    communionSchedule: false,
  },
  visSetting: {
    lemniscate: 1,
    rainbowFlakes: false,
    ellipse: true,
    bPos: 0,
    bcc: "#D68C45",
    bgc: "#FEFEE3",
    fgc: "#4C956C",
    ccc: "#2C6E49",
    lcc: "#FFC9B9",
  },
  voices: [
    {
      nnotes: 2,
      noctaves: 2,
      f0: 200,
      d: 1,
      waveform: 0,
      permfunc: 0,
      type: "Symmetry",
      iniVolume: null,
    },
    {
      nnotes: 2,
      noctaves: 2,
      f0: 200,
      d: 1,
      waveform: 0,
      permfunc: 0,
      type: "Symmetry",
      iniVolume: null,
    },
    {
      fl: 150,
      waveformL: 0,
      fr: 159,
      waveformR: 0,
      ma: 120,
      mp0: 10,
      mp1: 20,
      md: 600,
      type: "Martigli-Binaural",
      panOsc: 1,
      panOscPeriod: 140,
      panOscTrans: 20,
      isOn: true,
      iniVolume: null,
    },
    {
      fl: 359,
      waveformL: 0,
      fr: 350,
      waveformR: 0,
      ma: 120,
      mp0: 10,
      mp1: 20,
      md: 600,
      type: "Martigli-Binaural",
      panOsc: 1,
      panOscPeriod: 120,
      panOscTrans: 20,
      isOn: false,
      iniVolume: null,
    },
    {
      fl: 453,
      waveformL: 0,
      fr: 450,
      waveformR: 0,
      type: "Binaural",
      panOsc: 1,
      panOscPeriod: 160,
      panOscTrans: 20,
      iniVolume: null,
    },
  ],
};

const anotherPreset = JSON.parse(
  '{"_id":"Uso Ricreativo", "header":{ "med2": "Uso Ricreativo", "onlyOnce": false, "d": 900, "isShownIndex":-1,  "vcontrol": true, "creator": "fabbri", "communionSchedule": false }, "visSetting": { "lemniscate": 3, "rainbowFlakes": false, "ellipse": true, "bPos": 2, "bcc": "#2B2D42", "bgc": "#8D99AE", "fgc": "#EDF2F4", "ccc": "#EF233C", "lcc": "#D90429" }, "voices": [{ "fl": 250, "waveformL": 0, "fr": 255, "waveformR": 0, "ma": 130, "mp0": 10, "mp1": 20, "md": 600, "type": "Martigli-Binaural", "panOsc": 0, "panOscPeriod": 120, "panOscTrans": 20, "isOn": true, "iniVolume": null }, { "fl": 150, "waveformL": 0, "fr": 155, "waveformR": 0, "ma": 120, "mp0": 0.02, "mp1": 0.002, "md": 850, "type": "Martigli-Binaural", "panOsc": 3, "panOscPeriod": 120, "panOscTrans": 20, "isOn": false, "iniVolume": null }, { "nnotes": 50, "noctaves": 2, "f0": 200, "d": 10, "waveform": 0, "permfunc": 1, "type": "Symmetry", "iniVolume": null }, { "nnotes": 50, "noctaves": 2, "f0": 203.1433133, "d": 10, "waveform": 0, "permfunc": 2, "type": "Symmetry", "iniVolume": null }]}'
);

const thirdPreset = JSON.parse(
  '{"_id":"Guadagno QI","header":{"med2":"Guadagno QI","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":5,"rainbowFlakes":false,"ellipse":true,"bPos":2,"bcc":"#264653","bgc":"#2A9D8F","fgc":"#E9C46A","ccc":"#F4A261","lcc":"#E76F51"},"voices":[{"fl":495,"waveformL":0,"fr":455,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":120,"panOscTrans":20,"iniVolume":null},{"nnotes":5,"noctaves":2,"f0":200,"d":100,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"nnotes":5,"noctaves":2,"f0":210,"d":120,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"fl":240,"waveformL":0,"fr":280,"waveformR":0,"ma":140,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":150,"panOscTrans":20,"isOn":true,"iniVolume":null}]}'
);

const fourthPreset = JSON.parse(
  '{"_id":"Lavoro Focalizzato","header":{"med2":"Lavoro Focalizzato","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":31,"rainbowFlakes":false,"ellipse":true,"bPos":2,"bcc":"#000000","bgc":"#14213D","fgc":"#FCA311","ccc":"#E5E5E5","lcc":"#FFFFFF"},"voices":[{"fl":150,"waveformL":0,"fr":190,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":100,"panOscTrans":20,"iniVolume":null},{"fl":495,"waveformL":0,"fr":455,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":120,"panOscTrans":20,"iniVolume":null},{"nnotes":6,"noctaves":2,"f0":100,"d":100,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"nnotes":7,"noctaves":2,"f0":100.2,"d":120,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"fl":270,"waveformL":0,"fr":280,"waveformR":0,"ma":140,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":150,"panOscTrans":20,"isOn":true,"iniVolume":null}]}'
);

const fifthPreset = JSON.parse(
  '{"_id":"Studio Rilassante","header":{"med2":"Studio Rilassante","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":7,"rainbowFlakes":false,"ellipse":true,"bPos":0,"bcc":"#F6BD60","bgc":"#F7EDE2","fgc":"#F5CAC3","ccc":"#84A59D","lcc":"#F28482"},"voices":[{"fl":350,"waveformL":0,"fr":359,"waveformR":0,"ma":120,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":120,"panOscTrans":10,"isOn":true,"iniVolume":null},{"fl":209,"waveformL":0,"fr":200,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":70,"panOscTrans":5,"iniVolume":null},{"nnotes":12,"noctaves":1,"f0":201,"d":120,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null}]}'
);

const sixthPreset = JSON.parse(
  '{"_id":"Studio Energizzante","header":{"med2":"Studio Energizzante","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":8,"rainbowFlakes":false,"ellipse":true,"bPos":1,"bcc":"#219EBC","bgc":"#023047","fgc":"#8ECAE6","ccc":"#FFB703","lcc":"#FB8500"},"voices":[{"fl":350,"waveformL":0,"fr":360,"waveformR":0,"ma":120,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":120,"panOscTrans":10,"isOn":true,"iniVolume":null},{"fl":240,"waveformL":0,"fr":200,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":70,"panOscTrans":5,"iniVolume":null},{"nnotes":12,"noctaves":2,"f0":201,"d":400,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null}]}'
);

const seventhPreset = JSON.parse(
  '{"_id":"Rilassamento","header":{"med2":"Rilassamento","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":3,"rainbowFlakes":false,"ellipse":true,"bPos":0,"bcc":"#83C5BE","bgc":"#006D77","fgc":"#EDF6F9","ccc":"#FFDDD2","lcc":"#E29578"},"voices":[{"fl":253,"waveformL":0,"fr":250,"waveformR":0,"ma":140,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":30,"panOscTrans":0.5,"isOn":true,"iniVolume":null},{"fl":535,"waveformL":0,"fr":526,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":20,"panOscTrans":1,"iniVolume":null},{"nnotes":7,"noctaves":2,"f0":100.1,"d":120,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"nnotes":7,"noctaves":2,"f0":100,"d":100,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"fl":390,"waveformL":0,"fr":399,"waveformR":0,"type":"Binaural","panOsc":2,"panOscPeriod":0.333333333,"panOscTrans":20,"iniVolume":null}]}'
);

const eighthPreset = JSON.parse(
  '{"_id":"Insonnia","header":{"med2":"Insonnia","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":6,"rainbowFlakes":false,"ellipse":true,"bPos":0,"bcc":"#0077B6","bgc":"#03045E","fgc":"#00B4D8","ccc":"#90E0EF","lcc":"#CAF0F8"},"voices":[{"fl":253,"waveformL":0,"fr":255.5,"waveformR":0,"ma":140,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":120,"panOscTrans":20,"isOn":true,"iniVolume":null},{"fl":531,"waveformL":0,"fr":526,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":200,"panOscTrans":10,"iniVolume":null},{"nnotes":5,"noctaves":3,"f0":100.1,"d":64,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"nnotes":5,"noctaves":3,"f0":100,"d":120,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"fl":397.5,"waveformL":0,"fr":399,"waveformR":0,"type":"Binaural","panOsc":2,"panOscPeriod":0.333333333,"panOscTrans":20,"iniVolume":null}]}'
);

const ninthPreset = JSON.parse(
  '{"_id":"Ansia e Depressione","header":{"med2":"Ansia e Depressione","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":4,"rainbowFlakes":false,"ellipse":true,"bPos":0,"bcc":"#CBF3F0","bgc":"#2EC4B6","fgc":"#FFFFFF","ccc":"#FFBF69","lcc":"#FF9F1C"},"voices":[{"fl":203,"waveformL":0,"fr":200,"waveformR":0,"ma":160,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":300,"panOscTrans":20,"isOn":true,"iniVolume":null},{"fl":335,"waveformL":0,"fr":326,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":200,"panOscTrans":1,"iniVolume":null},{"nnotes":7,"noctaves":2,"f0":150.1,"d":120,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"nnotes":9,"noctaves":2,"f0":150,"d":100,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"fl":449.1666,"waveformL":0,"fr":458.166666,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":100,"panOscTrans":20,"iniVolume":null}]}'
);

const tenthPreset = JSON.parse(
  '{"_id":"Dipendenza","header":{"med2":"Dipendenza","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"who","communionSchedule":false},"visSetting":{"lemniscate":7,"rainbowFlakes":false,"ellipse":true,"bPos":0,"bcc":"#D8E2DC","bgc":"#FFE5D9","fgc":"#FFCAD4","ccc":"#F4ACB7","lcc":"#9D8189"},"voices":[{"mf0":300,"waveformM":0,"ma":100,"mp0":10,"mp1":20,"md":600,"type":"Martigli","isOn":true,"iniVolume":null},{"mf0":300,"waveformM":0,"ma":140,"mp0":10,"mp1":20,"md":600,"type":"Martigli","isOn":false,"iniVolume":null},{"fl":150,"waveformL":0,"fr":159,"waveformR":0,"type":"Binaural","panOsc":0,"panOscPeriod":120,"panOscTrans":20,"iniVolume":null},{"nnotes":7,"noctaves":3,"f0":100,"d":120,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"nnotes":8,"noctaves":3,"f0":100,"d":130,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null}]}'
);

const eleventhPreset = JSON.parse(
  '{"_id":"Dolori Cronici","header":{"med2":"Dolori Cronici","onlyOnce":false,"d":900, "isShownIndex":-1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":2,"rainbowFlakes":false,"ellipse":true,"bPos":0,"bcc":"#B388EB","bgc":"#8093F1","fgc":"#72DDF7","ccc":"#F7AEF8","lcc":"#FDC5F5"},"voices":[{"fl":350,"waveformL":0,"fr":359.14,"waveformR":0,"ma":140,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":140,"panOscTrans":20,"isOn":true,"iniVolume":null},{"fl":126.125,"waveformL":0,"fr":125,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":180,"panOscTrans":15,"iniVolume":null},{"nnotes":9,"noctaves":3,"f0":101,"d":160,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null}]}'
);

const twelfthPreset = JSON.parse(
  '{"_id":"Emicrania","header":{"med2":"Emicrania","onlyOnce":false,"d":900, "isShownIndex": -1, "vcontrol":true,"creator":"fabbri","communionSchedule":false},"visSetting":{"lemniscate":6,"rainbowFlakes":false,"ellipse":true,"bPos":0,"bcc":"#028090","bgc":"#05668D","fgc":"#00A896","ccc":"#02C39A","lcc":"#F0F3BD"},"voices":[{"fl":250,"waveformL":0,"fr":259,"waveformR":0,"ma":140,"mp0":10,"mp1":20,"md":600,"type":"Martigli-Binaural","panOsc":1,"panOscPeriod":120,"panOscTrans":20,"isOn":true,"iniVolume":null},{"fl":126.125,"waveformL":0,"fr":125,"waveformR":0,"type":"Binaural","panOsc":1,"panOscPeriod":140,"panOscTrans":5,"iniVolume":null},{"nnotes":9,"noctaves":2,"f0":201,"d":160,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null},{"nnotes":9,"noctaves":2,"f0":200,"d":40,"waveform":0,"permfunc":0,"type":"Symmetry","iniVolume":null}]}'
);

const audiovisualPresetsList = [
  audiovisualPreset,
  anotherPreset,
  thirdPreset,
  fourthPreset,
  fifthPreset,
  sixthPreset,
  seventhPreset,
  eighthPreset,
  ninthPreset,
  tenthPreset,
  eleventhPreset,
  twelfthPreset,
];

const audiovisualPresets = audiovisualPresetsList.reduce((acc, preset) => {
  acc[preset.header.med2] = preset;
  return acc;
}, {});

export { audiovisualPresets, audiovisualPresetsList };
