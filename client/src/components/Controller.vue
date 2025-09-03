<template>
  <div v-if="type === 'train'">
    <train-controller v-if="connection.isConnected()" ref="implementation" :connection="connection" :index="index" :ledColor="ledColor" @change-color="$emit('change-color', $event)" :selectionA="selectionA" @change-selection-a="$emit('change-selection-a', $event)" :selectionB="selectionB" @change-selection-b="$emit('change-selection-a', $event)" @remove="$emit('remove', $event)"/>
    <div class="loading-controller" v-else ref="implementation">
      <img src="../assets/loading.svg" style="position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); width: 50px; height: 50px;"/>
       <!-- TODO: Loading screen wird nicht angezeigt... -->
    </div>
  </div>
</template>

<script>
  import TrainController from './TrainController.vue';

  export default {
    name: 'controller',

    beforeDestroy: function() {

    },

    components: {
      TrainController,
    },

    props: ['index', 'ledColor', 'selectionA', 'selectionB', 'type', 'connection'],
  }
</script>

<style lang="scss">
  .loading-controller
  {
    position: relative;
    background-color: cornflowerblue;
    width: 100px;
    height: 280px;

    margin: 5px;
    border-style: solid;
    border-color: black;
    border-width: 1px;
    box-shadow: 0px 0px 3px #666;
  }
</style>
