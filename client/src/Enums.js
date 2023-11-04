let UUIDS = {	
	service: "ba451494-3447-11ee-be56-0242ac120002",	
	engineSpeed: "c045fb9c-3447-11ee-be56-0242ac120002",
	command: "c8d7e25c-3447-11ee-be56-0242ac120002",
/*
	moduleInfos: "9be5491f-198c-4d6b-9f97-7634d63fd30f",
	command: "36881b37-d617-4e8f-906a-3ab843b2e9ed",
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
  }

  //...
};

let CONFIG = {
  SPEED_STEPS: 5
};

let COMMAND = {
  START: 4,    
  STOP: 5
};

export {UUIDS, COMMAND, MIDI_CONTROLS, CONFIG, MIDI_IDS};