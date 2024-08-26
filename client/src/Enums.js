let UUIDS = {	
  train: {
	  service: "566804f6-368b-44a4-a375-ce8789eb1dd8",	
	  engineSpeed: "c045fb9c-3447-11ee-be56-0242ac120002",
  	lightColor: "c8d7e25c-3447-11ee-be56-0242ac120002",
    command: "3804213f-d6ad-458e-977c-4f9f1b9f6b9a",
    colorReading: "45cfdad3-4f02-4e5c-aa16-1c513dc76a3c"
  }
/*
	moduleInfos: "9be5491f-198c-4d6b-9f97-7634d63fd30f",
	commandAnswers: "b399bd6d-fbce-4488-ae82-314a0195db4d",*/
};

let MIDI_CONTROLS = {
  device: 176,
  speed1: 2,
  speed2: 3,
  speed3: 4,

  MAX_VALUE: 127
};

let MIDI_IDS = {
  0: {
    slider: 2,
    knob: 14,
    buttonA: 23,
    buttonB: 33
  },

  1: {
    slider: 3,
    knob: 15,
    buttonA: 24,
    buttonB: 34
  },

  2: {
    slider: 4,
    knob: 16,
    buttonA: 25,
    buttonB: 35
  },

  3: {
    slider: 5,
    knob: 17,
    buttonA: 26,
    buttonB: 36
  },

  4: {
    slider: 6,
    knob: 18,
    buttonA: 27,
    buttonB: 37
  },

  5: {
    slider: 8,
    knob: 19,
    buttonA: 28,
    buttonB: 38
  },

  6: {
    slider: 9,
    knob: 20,
    buttonA: 29,
    buttonB: 39
  },

  7: {
    slider: 12,
    knob: 21,
    buttonA: 30,
    buttonB: 40
  },

  8: {
    slider: 13,
    knob: 22,
    buttonA: 31,
    buttonB: 41
  }
  //...
};

let CONFIG = {
  SPEED_STEPS: 5
};

export {UUIDS, MIDI_CONTROLS, CONFIG, MIDI_IDS};