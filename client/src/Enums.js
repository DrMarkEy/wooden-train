let UUIDS = {	
	service: "ba451494-3447-11ee-be56-0242ac120002",	
	engineSpeed: "c045fb9c-3447-11ee-be56-0242ac120002",
	command: "c8d7e25c-3447-11ee-be56-0242ac120002",
/*
	moduleInfos: "9be5491f-198c-4d6b-9f97-7634d63fd30f",
	command: "36881b37-d617-4e8f-906a-3ab843b2e9ed",
	commandAnswers: "b399bd6d-fbce-4488-ae82-314a0195db4d",*/
};

let STATE = {
  DISCONNECTED: -1,
  INITIALIZING: 0,
  ACTIVE: 1, 
  PAUSED: 2,
  STOPPED: 3,
  CHARGING: 4
};

let TIMING = {
  BRILLIANT: 1,                //Players are solving the bomb very fast, so that much time will be left
  DEFAULT: 2,                  //Players are solving the bomb at a pace, so that it will probably be solved with e few seconds left
  BAD: 3,          //Players are solving the bomb at a pace, so that it will likely, but not definetly explode
  CATASTROPHIC: 4  //Players are solving the bomb at a pace that there is almost no chance not to explode
};

let COMMAND = {
  START: 4,    
  STOP: 5
};

export {UUIDS, STATE, COMMAND, TIMING};