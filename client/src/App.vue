<template>
  <div id="app">
    <img alt="Logo" id="logo" src="./assets/logo.webp">
    

    <hr/>

    <button @click="connectBLE">Connect to Device!</button>

    <button @click="startDriving">Go!</button>
    <button @click="stopDriving">Stop!</button>
  </div>
</template>

<script>
import HelloWorld from './components/HelloWorld.vue';
import {COMMAND} from './Enums.js';
//import WebSocket from 'ws';


export default {
  name: 'App',
  components: {
    HelloWorld
  },

  methods: {
    connectBLE: function() {

      this.$bluetooth.connect();


    },

    startDriving: function() {
      this.$bluetooth.sendCommand(COMMAND.START);
    },


    stopDriving: function() {
      this.$bluetooth.sendCommand(COMMAND.STOP);
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
</style>
