<template>
    <train-controller v-if="type === 'train' && connection.isConnected()" :connection="connection" :index="index" :name="name" :ledColor="ledColor" @change-color="$emit('change-color', $value)" :selectionA="selectionA" @change-selection-a="$emit('change-selection-a', $value)" :selectionB="selectionB" @change-selection-b="$emit('change-selection-a', $value)" @remove="$emit('remove')"/>
    <div class="loading-controller" v-else-if="type === 'train'">
      Q
    </div>
</template>
  
<script>7
  import TrainController from './TrainController.vue';
  
  export default {
    name: 'controller',

    components: {
      TrainController,    
    },

    props: ['index', 'name', 'ledColor', 'selectionA', 'selectionB', 'type', 'connection'],
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
  