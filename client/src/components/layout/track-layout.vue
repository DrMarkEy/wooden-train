<template>
    <div class="track-layout">
      <general-track :model="piece" v-for="(piece, index) in trackPieces" :key="index"/>
      <button @click="addLeftCurve">⬅️</button>
      <button @click="addShortStraight">⬆️</button>
      <button @click="addRightCurve">➡️</button>
    </div>
  </template>

  <script>
  import generalTrack from './general-track.vue';
  import { ShortStraightTrack } from './straight-track';
  import { CurveRight, CurveLeft } from './curved-track';

  import { buildMatrix } from './matrices';

  import { getTranslation, getRotationAngle } from './matrices';

  export default {
    name: 'track-layout',

    props: [''],

    components: {
      generalTrack
    },

    data: function() {
      return {trackPieces: [new ShortStraightTrack(buildMatrix(100, 0, -22.5))]};
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
      },

      addLeftCurve: function() {
        let lastTrackPiece = this.trackPieces[this.trackPieces.length-1];
        this.trackPieces.push(new CurveLeft(lastTrackPiece.getDestination()));
      }
    }
  }
  </script>

  <style lang="scss">
  .track-layout {

  }
  </style>
