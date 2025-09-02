<template>
    <div class="track-layout">
      <general-track :model="piece" v-for="(piece, index) in trackPieces" :key="index"/>
      <button @click="addShortStraight">Add Short Straight</button>
      <button @click="addRightCurve">Add Right Curve</button>
    </div>
  </template>

  <script>
  import generalTrack from './general-track.vue';
  import { ShortStraightTrack } from './straight-track';
  import { CurveRight } from './curved-track';

  import { buildMatrix } from './matrices';

  import { getTranslation, getRotationAngle } from './matrices';

  export default {
    name: 'track-layout',

    props: [''],

    components: {
      generalTrack
    },

    data: function() {
      return {trackPieces: [new ShortStraightTrack(buildMatrix(100, 0, -20))]};
    },

    computed: {

    },

    methods: {
      addShortStraight: function() {
        let lastTrackPiece = this.trackPieces[this.trackPieces.length-1];
        this.trackPieces.push(new ShortStraightTrack(lastTrackPiece.getDestination()));
      },

      addRightCurve: function() {
        let lastTrackPiece = this.trackPieces[this.trackPieces.length-1];
        this.trackPieces.push(new CurveRight(lastTrackPiece.getDestination()));
      }
    }
  }
  </script>

  <style lang="scss">
  .track-layout {

  }
  </style>
