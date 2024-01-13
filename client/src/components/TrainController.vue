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

      <button :class="{'press-button': true, 'pressed': pressedA}" id="buttonA" @click="pressButtonA">
        <img :src="getImage(selectionA)"/>
      </button>
            
      <button :class="{'press-button': true, 'pressed': pressedB}" id="buttonB" @click="pressButtonB">
        <img :src="getImage(selectionB)"/>
      </button>

      <input class="speed-slider" type="range" v-model="speed" min="0" max="127"/>

      <div class="bottom-line">
        <div class="color-picker" :style="{'background-color': mLedColor}">
          <input type="color" v-model="mLedColor"/>
        </div>

        <div class="device-name">
          {{ name }}
        </div>
      </div>
    </div>
  </template>
  
  <script>
  
  let MODE_COUNT = 6;
  let MIN_ANGLE = -45;
  let MAX_ANGLE = 225;

  import {TRAIN_COMMAND} from '../TrainConnection.js';

  function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
    } : null;
  }

  export default {
    name: 'train-controller',

    props: ['index', 'ledColor', 'selectionA', 'selectionB', 'connection'],

    mounted: function() {      
      this.$midi.addController(this.index, this.handleMidiCommand);
    },

    unmounted: function() {
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
      rotationAngle: function() {
        return (MAX_ANGLE - MIN_ANGLE) / (MODE_COUNT - 1) * this.selectionA + MIN_ANGLE;
      },

      name: function() {
        if(this.connection !== undefined && this.connection.device !== undefined) {
          return this.connection.device.name.replace("Baureihe ", "");
        }
        
        return "";
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
            this.mSelectionA = Math.round(value / 127  * (MODE_COUNT - 1));          
          break;

          case 'buttonA':
            if(value > 64) {
              this.pressedA = true;              
              this.pressButtonA();
            }
            else {
              this.pressedA = false;              
            }
          break;

          case 'buttonB':
            if(value > 64) {
              this.pressedB = true;          
              this.pressButtonB();    
            }
            else {
              this.pressedB = false;              
            }
          break;
        }
      },

      pressButtonA: function() {        
        // TODO: Send different commands, depending on mSelectionA
        this.connection.sendCommand(TRAIN_COMMAND.WHISTLE);
      },

      pressButtonB: function() {
        //TRAIN_COMMAND
      },

      removeController: function() {
        this.$emit('remove');
      },

      nextMode: function() {
        if(this.mSelectionA < MODE_COUNT - 1) {
          this.mSelectionA++;
        }
        else {
          this.mSelectionA = 0;
        }
      },

      getImage: function(idx) {
        if(idx % 2 == 0) {
          return 'https://www.creativefabrica.com/wp-content/uploads/2022/01/28/1643383088/Whistle-580x386.jpg';
        }
        else {
          return 'https://cdn1.iconfinder.com/data/icons/systemui-vol-2/21/reverse-256.png';
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
    height: 280px;

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

      &#buttonA
      {
        top:90px;
      }

      &#buttonB
      {
        top:180px;
      }

      &:active, &.pressed
      {
        box-shadow: inset 0px 0px 2px #555;
      }
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
  }
  </style>
  