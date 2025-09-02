<template>
  <div id="app">
    <img alt="Logo" id="logo" src="./assets/logo.webp">

    <div class="controller-panel">
      <controller v-for="(c, i) in controllers" :key="c.connection.identifier()" :index="i" :type="c.type" :connection="c.connection" :ledColor="c.ledColor" @change-color="val => c.ledColor = val" :selectionA="c.selectionA" @change-selection-a="sel => c.selectionA = sel" :selectionB="c.selectionB" @change-selection-b="sel => c.selectionB = sel" @remove="removeController(i)"/>

      <empty-controller :existing-controllers="controllers" @add-controller="addController"/>
    </div>
  </div>
</template>

<script>

import Controller from './components/Controller.vue';
import EmptyController from './components/EmptyController.vue';

export default {
  name: 'App',
  components: {
    Controller,
    EmptyController
  },

  data: function() {
    return {controllers: []};
  },

  methods: {
    addController: function(controller) {
      this.controllers.push(controller);

      let thi = this;
      controller.connection.setOnConnectionClosedListener(function() {
          // Finde den aktuellen Index dieses Controllers
          const idx = thi.controllers.findIndex(c => c.connection === controller.connection);
          if(idx !== -1) thi.removeController(idx);
      });
    },

    removeController: function(index) {
      console.log("Removing controller", index);
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
}

#logo
{
  width: 500px;
}
</style>
