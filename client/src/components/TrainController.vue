<template>
    <div class="train-controller">

      <div class="turn-button-container">
        <div class="turn-button" :style="{'transform': 'rotate('+rotationAngle+'deg)'}" @click="nextMode">
          <div class="label">
             -
          </div>
        </div>
      </div>

      <button class="press-button" id="buttonA">
        <img :src="getImage(selectionA)"/>
      </button>
            
      <button class="press-button" id="buttonB">
        <img :src="getImage(selectionB)"/>
      </button>

      <input class="speed-slider" type="range" min="0" max="5"/>

      <div class="bottom-line">
        <div class="color-picker" :style="{'background-color': ledColor}">
          <input type="color" v-model="ledColor"/>
        </div>

        <div class="device-name">
          101.01
        </div>
      </div>
    </div>
  </template>
  
  <script>
  
  let MODE_COUNT = 6;
  let MIN_ANGLE = -45;
  let MAX_ANGLE = 225;
  let DEFAULT_COLOR = '#fff';

  export default {
    name: 'train-controller',
    components: {
      //HelloWorld
    },
  
    data: function() {
      return {selectionA: 0, selectionB: 1, ledColor: DEFAULT_COLOR};
    },

    computed: {
      rotationAngle: function() {
        return (MAX_ANGLE - MIN_ANGLE) / (MODE_COUNT - 1) * this.selectionA + MIN_ANGLE;
      },
    },
  
    methods: {
      nextMode: function() {
        if(this.selectionA < MODE_COUNT - 1) {
          this.selectionA++;
        }
        else {
          this.selectionA = 0;
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

      &:active
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
  