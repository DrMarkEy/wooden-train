<template>
  <div id="app">
    <img alt="Logo" id="logo" src="./assets/logo.webp">

    <div class="controller-panel">
      <controller v-for="(c, i) in controllers" :key="c.connection.identifier()" :index="i" :type="c.type" :connection="c.connection" :ledColor="c.ledColor" @change-color="val => c.ledColor = val" :selectionA="c.selectionA" @change-selection-a="sel => c.selectionA = sel" :selectionB="c.selectionB" @change-selection-b="sel => c.selectionB = sel"/>

      <div class="empty-controller">
        <div class="circle" @click="searchBluetoothDevice">
          <div class="inner">+</div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>

import Controller from './components/Controller.vue';

import TrainConnection from './TrainConnection.js';

function nextColor(index) {
  switch(index) {
    case 0: return '#ff0000';
    case 1: return '#00ff00';
    case 2: return '#0000ff';
    case 3: return '#ffff00';
    case 4: return '#00ffff';
    case 5: return '#ff00ff';
    case 6: return '#ff8800';
    case 7: return '#ffffff';
    case 8: return '#5382ee';
  }
}

export default {
  name: 'App',
  components: {
    Controller
  },

  data: function() {
    return {controllers: []};
  },

  methods: {
    searchBluetoothDevice: async function() {
      let connection = await TrainConnection.searchDevice(this.controllers.map(cont => cont.connection));

      if(connection !== undefined) {
        this.addTrainController(connection);

        setTimeout(function(){
          connection.setup();
        }, 10);
      }
    },

    addTrainController: function(trainConnection) {
      this.controllers.push({
        type: 'train',
        ledColor: nextColor(this.controllers.length),
        selectionA: 0,
        selectionB: 1,
        connection: trainConnection
      });

      let thi = this;
      trainConnection.setOnConnectionClosedListener(function() {
          // Finde den aktuellen Index dieses Controllers
          const idx = thi.controllers.findIndex(c => c.connection === trainConnection);
          if(idx !== -1) thi.removeTrainController(idx);
      });
    },

    removeTrainController: function(index) {
      this.controllers.splice(index, 1);
    },
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

  .empty-controller
  {
    position: relative;
    width: 100px;
    height: 320px;
    border-style: dotted;
    border-width: 3px;
    border-color: #aaa;
    background-color: #ccc;
    margin: 5px;

    .circle
    {
      left: calc(50% - 20px);
      top: calc(50% - 20px);
      position: absolute;
      background-color: #ccc;
      width: 40px;
      height: 40px;
      border-radius: 40px;
      border-style: dashed;
      border-width: 2px;
      border-color: #999;

      .inner
      {
        font-size: 24px;
        color: #999;
        margin-top: 5px;
      }

      &:hover
      {
        background-color: cornflowerblue;
        cursor: pointer;
        border-style: solid;
        border-color: #333;
        box-shadow: 1px 1px 2px #666;

        .inner
        {
          color:#333;
        }
      }
    }
  }
}

#logo
{
  width: 500px;
}
</style>
