<template>
    <div class="train-controller">

      <button class="remove-button" @click="removeController">x</button>

      <div class="turn-button-container">
        <div class="turn-button" :style="{'transform': 'rotate('+rotationAngle+'deg)'}" @click="nextMode">
          <div class="label">
             -
          </div>
        </div>
      </div>

      <press-button id="buttonA" ref="buttonA" :command="COMMANDS[selectionA]" :pressed="pressedA" :connection="connection"/>
      <press-button v-if="!stopped" ref="buttonB" id="buttonB" :command="COMMAND_REVERSE" :pressed="pressedB" :connection="connection"/>
      <press-button v-if="stopped" ref="buttonB" id="buttonBAlt" :command="COMMAND_PLAY" :pressed="pressedB" :connection="connection"/>

      <div :class="{'state-view': true, 'viewA': true, 'pressed': pressedB}" :style="{'background-color': sensorColor}">

      </div>

      <div :class="{'state-view': true, 'viewB': true}">
        {{ operationMode }}
      </div>

      <input class="speed-slider" type="range" v-model="speed" min="0" max="127"/>

      <div class="bottom-line">
        <div class="color-picker" :style="{'background-color': mLedColor}">
          <input type="color" v-model="mLedColor"/>
        </div>

        <div class="device-name">
          {{ name }}
        </div>
      </div>

      <div class="stopped-overlay" v-if="stopped">
        <div class="stopped-text"><!--STOPPED--></div>
      </div>
    </div>
  </template>

  <script>

  let MIN_ANGLE = -45;
  let MAX_ANGLE = 225;

  import {TRAIN_COMMAND} from '../TrainConnection.js';
  import {OPERATION_MODE} from '../TrainConnection.js';
  import PressButton from './PressButton.vue';

  function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
    } : null;
  }

  let COMMAND_PLAY = {
      command: TRAIN_COMMAND.START,
      icon: require('../assets/play.png')
  };

  let COMMAND_REVERSE = {
      command: TRAIN_COMMAND.REVERSE,
      icon: require('../assets/reverse.png')
  };

  let COMMANDS = {
    "0": {
      command: TRAIN_COMMAND.WHISTLE,
      icon: require('../assets/whistle.png')
    },
    "1": {
      command: TRAIN_COMMAND.TOGGLE_COLOR_SENSOR,
      icon: require('../assets/color.png')
    },
    "2": {
      command: TRAIN_COMMAND.STOP,
      icon: require('../assets/stop.png')
    },
    "3": {
      command: TRAIN_COMMAND.ACCIDENT_LIGHTS,
      icon: require('../assets/alert.png')
    }
  };

  export default {
    name: 'train-controller',

    props: ['index', 'ledColor', 'selectionA', 'connection'],

    components: {
      PressButton
    },

    mounted: function() {
      this.$midi.addController(this.index, this.handleMidiCommand);
    },

    beforeDestroy: function() {
      // TODO: Move this up to Controller
      this.connection.disconnect();
      this.$midi.removeController(this.index);
    },

    data: function() {
      return {speed: 0, pressedA: false, pressedB: false};
    },

    watch: {
      speed: function(val) {
        this.connection.setSpeed(val/127*5);
      }
    },

    computed: {
      COMMANDS() {
        return COMMANDS;
      },

      COMMAND_PLAY() {
        return COMMAND_PLAY;
      },

      COMMAND_REVERSE() {
        return COMMAND_REVERSE;
      },

      OPERATION_MODE() {
        return OPERATION_MODE;
      },

      modeCount: function() {
        return Object.keys(COMMANDS).length;
      },

      rotationAngle: function() {
        return (MAX_ANGLE - MIN_ANGLE) / (this.modeCount - 1) * this.selectionA + MIN_ANGLE;
      },

      name: function() {
        if(this.connection !== undefined && this.connection.device !== undefined) {
          return this.connection.device.name.replace("Baureihe ", "");
        }

        return "";
      },

      operationMode: function() {
        if(this.connection !== undefined && this.connection.state !== undefined) {
          return this.connection.state.operationMode;
        }

        return -1;
      },

      sensorColorCode: function() {
        if(this.connection !== undefined && this.connection.state !== undefined) {
          return this.connection.state.lastReadColor;
        }

        return "";
      },

      sensorColor: function() {
        switch(this.sensorColorCode) {
          case -1: return 'transparent';
          case 0: return 'burlywood'; // COLOR_WOOD
          case 1: return 'black';       // COLOR_BLACK
          case 2: return 'white';       // COLOR_WHITE
          case 3: return 'red';         // COLOR_RED
          case 4: return 'yellow';      // COLOR_YELLOW
          case 5: return 'green';       // COLOR_GREEN
          case 6: return 'blue';        // COLOR_BLUE
          case 7: return 'magenta';     // COLOR_MAGENTA
        }
      },

      stopped: function() {
        return this.operationMode === OPERATION_MODE.STOPPED;
      },

      mLedColor: {
        get: function() {
          return this.ledColor;
        },

        set: function(val) {
          this.$emit('change-color', val);
          let cols = hexToRgb(val);
          this.connection.setLightsColor(cols, cols, cols, cols, cols, cols);
        }
      },

      mSelectionA: {
        get: function() {
          return this.selectionA;
        },

        set: function(val) {
          this.$emit('change-selection-a', val);
        }
      }
    },

    methods: {
      handleMidiCommand: function(command, value) {
        switch(command) {
          case 'slider':
            this.speed = value;
          break;

          case 'knob':
            this.mSelectionA = Math.round(value / 127  * (this.modeCount - 1));
          break;

          case 'buttonA':
            if(value > 64) {
              this.pressedA = true;
              this.$refs.buttonA.press();
            }
            else {
              this.pressedA = false;
            }
          break;

          case 'buttonB':
            if(value > 64) {
              this.pressedB = true;
              this.$refs.buttonB.press();
            }
            else {
              this.pressedB = false;
            }
          break;

          default:
            console.log("Unknown command "+command);
          break;
        }
      },

      removeController: function() {
        this.$emit('remove');
      },

      nextMode: function() {
        if(this.mSelectionA < this.modeCount - 1) {
          this.mSelectionA++;
        }
        else {
          this.mSelectionA = 0;
        }
      }
    }
  }
  </script>

  <style lang="scss">
  .train-controller {
    position: relative;
    background-color: cornflowerblue;
    width: 100px;
    height: 325px;

    margin: 5px;
    border-style: solid;
    border-color: black;
    border-width: 1px;
    box-shadow: 0px 0px 3px #666;


    .remove-button
    {
      color: red;
      position: absolute;
      left: 8px;
      top: 8px;
      background-color: #444;
      border-style: solid;
      border-width: 1px;
      border-radius: 2px;
      width: 20px;
      height: 20px;
      font-family: sans-serif;
      padding-bottom: 5px;

      &:hover {
        background-color: #555;
      }

      &:active {
        box-shadow: inset 1px 1px 2px #333;
      }
    }

    .turn-button-container {
      position: absolute;
      top: 5px;
      right: 5px;

      background-color: white;
      width: 40px;
      height: 40px;
      border-radius: 25px;
      border-style: solid;
      border-width: 2px;
      border-color: #777;
      box-shadow: 3px 3px 4px #555;
      user-select: none;
      cursor: pointer;

      .turn-button
      {
        width: 40px;
        height: 40px;

        .label
        {
          position: absolute;
          left: 0px;
          top: 4px;
          font-size: 25px;
        }
      }
    }


    .press-button
    {
      position: absolute;
      left: 15px;
      width: 40px;
      height: 40px;
      border-radius: 5px;
      border-style: solid;
      border-width: 1px;
      border-color: #888;
      box-shadow: 2px 1px 1px #666;
      user-select: none;
      cursor: pointer;

      img
      {
        width: 30px;
      }


      &:active, &.pressed
      {
        box-shadow: inset 0px 0px 2px #555;
      }
    }

    #buttonA
    {
      top:90px;
    }

    #buttonB
    {
      top:180px;
    }

    #buttonBAlt
    {
      top:180px;
      z-index: 11;
    }

    .speed-slider
    {
      position: absolute;
      width: 160px;
      right: -56px;
      top: 140px;
      transform: rotate(270deg);

      accent-color: #eee;
    }

    .bottom-line
    {
      position: absolute;
      bottom: 8px;
      display: flex;
      width: 100%;
      height: 20px;

      .color-picker
      {
        width: 18px;
        margin-left: 8px;
        margin-right: 8px;
        border-style: solid;
        border-width: 1px;
        border-color: #444;
        border-radius: 3px;
        box-shadow: 1px 1px 1px #666;
        user-select: none;

        input
        {
          opacity: 0;
          width: 100%;
          height: 100%;
          cursor: pointer;
        }

        &:active
        {
          box-shadow: inset 0px 0px 2px #555;
        }
      }

      .device-name
      {
        background-color: #eee;
        border-style: solid;
        border-color: #888;
        border-width: 1px;
        padding: 2px;
        font-size: 9pt;
        flex: 1 1 auto;
        margin-right: 8px;
        user-select: none;

      }
    }

    .state-view
    {
      background-color: #eee;
      width: 32px;
      height: 32px;
      border-radius: 30px;

      border-style: solid;
      border-width: 1px;
      border-color: #888;

      box-shadow: inset 1px 1px 6px #666;
      user-select: none;
      cursor: pointer;

      position: absolute;
      bottom: 40px;

      &.viewA {
        left: 10px;
      }

      &.viewB {
        left: 55px;
        border-radius: 10px;
      }
    }

    .stopped-overlay {
      position: absolute;
      top: 0px;
      left: 0px;
      width: 100%;
      height: 100%;
      background-color: rgba(200, 200, 200, 0.7);
      z-index: 10;

      .stopped-text {
        position: absolute;
        top: calc(50% - 10px);
        left: 0px;
        width: 100%;
        text-align: center;
        font-size: 20px;
        font-weight: bold;
        color: #444;
        user-select: none;
      }
    }
  }
  </style>
