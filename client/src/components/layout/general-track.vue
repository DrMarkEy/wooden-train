<template>
    <div class="general-track">
      <straight-track v-if="model.getType() === 'straight'" :model="model"/>
      <curved-track v-if="model.getType() === 'curve'" :model="model"/>
    </div>
  </template>

  <script>

  import straightTrack from './short-straight-track.vue';
  import curvedTrack from './curved-track.vue';


  export default {
    name: 'general-track',

    props: ['model'],

    components: {
      straightTrack,
      curvedTrack
    },

    data: function() {
      return {};
    },

    methods: {

    }
  }
  </script>

  <style lang="scss">

  </style>
