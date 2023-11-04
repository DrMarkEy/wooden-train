<template>
  <div id="app">
    <img alt="Logo" id="logo" src="./assets/logo.webp">
    

    <hr/>

    <button @click="connectBLE">Connect to Device!</button>

    <button @click="startDriving">Go!</button>
    <button @click="stopDriving">Stop!</button>

    <input class="test-speed-slider" type="range" v-model="speed0" min="0" max="5" :disabled="midiAvailable"/>

    <div class="controller-panel">
      <train-controller/>
      <train-controller/>
      <train-controller/>
    </div>
  </div>
</template>

<script>

import {COMMAND, CONFIG} from './Enums.js';
//import WebSocket from 'ws';
import TrainController from './components/TrainController.vue';


export default {
  name: 'App',
  components: {
    TrainController
  },

  data: function() {
    return {speed0: 0, midiAvailable: false};
  },

  mounted: function() {
    this.$midi.setSpeedListener(0, this.setSpeed0);    
  },

  methods: {
    connectBLE: function() {

      this.$bluetooth.connect();


    },

    setSpeed0: function(speed) {
      let speedStufe = Math.round(speed * CONFIG.SPEED_STEPS);

      if(this.speed0 != speedStufe) {
        
        this.speed0 = speedStufe;        
        this.$bluetooth.setSpeed(this.speed0);
        
        console.log('speed', this.speed0);
      }
    },

    startDriving: function() {
      this.$bluetooth.sendCommand(COMMAND.START);
    },


    stopDriving: function() {
      this.$bluetooth.sendCommand(COMMAND.STOP);
    }
  },

  watch: {
    '$midi.available': function(val) {
      this.midiAvailable = val;
    }
  }
}
</script>

<style lang="scss">

html {  
  background-color: #888;
}

#app {  
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;  
}

.controller-panel
{
  position: absolute;
  bottom: 0px;
  right: 0px;
  left: 0px;  
  display: flex;
  margin: 10px;
  background-color: #ddd;
  border-style: solid;
  border-color: black;
  border-width: 1px;
  border-radius: 12px;
  padding: 10px;
  box-shadow: 4px 4px 6px #444;
}

#logo
{
  width: 500px;  
}

.test-speed-slider
{
  position: absolute;
   top: 40%;
   transform: rotate(270deg);
}
</style>
