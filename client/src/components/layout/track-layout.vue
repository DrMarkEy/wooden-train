<template>
    <div class="track-layout">
      <general-track :model="piece" v-for="(piece, index) in trackPieces" :key="index"/>
      <button @click="addTrackPiece">ADD</button>
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
      return {trackPieces: [new ShortStraightTrack(buildMatrix(100, 0, 20))]};
    },

    computed: {

    },

    methods: {
      addTrackPiece: function() {
        //let matrix = this.$refs.firstPiece.getMatrix();
        //let translation = getTranslation(matrix);
        //let angle = getRotationAngle(matrix);
        let lastTrackPiece = this.trackPieces[this.trackPieces.length-1];

        this.trackPieces.push(new ShortStraightTrack(lastTrackPiece.getDestination()));
        /*{
          type: 'short-straight',
          x: this.trackPieces[this.trackPieces.length-1].x,
          y: this.trackPieces[this.trackPieces.length-1].y + 120,
          angle: this.trackPieces[this.trackPieces.length-1].angle
        });*/

      }
    }
  }
  </script>

  <style lang="scss">
  .track-layout {

  }
  </style>
