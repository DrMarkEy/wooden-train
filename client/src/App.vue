<template>
  <div id="app">
    <img alt="Logo" id="logo" src="./assets/logo.webp">
    

    <hr/>

    <button @click="connectBLE">Connect to Device!</button>

    <button @click="startDriving">Go!</button>
    <button @click="stopDriving">Stop!</button>

    <input class="speed-slider" type="range" v-model="speed0" min="0" max="100" :disabled="midiAvailable"/>
  </div>
</template>

<script>
//import HelloWorld from './components/HelloWorld.vue';
import {COMMAND} from './Enums.js';
//import WebSocket from 'ws';


export default {
  name: 'App',
  components: {
    //HelloWorld
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
      this.speed0 = speed*100;
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
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}

#logo
{
  width: 500px;  
}

.speed-slider
{
  position: absolute;
   top: 40%;
   transform: rotate(270deg);
}
</style>
