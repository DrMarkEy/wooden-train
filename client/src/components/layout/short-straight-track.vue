<template>
    <svg class="short-straight-track" :style="style" width="1px" height="1px" >
        <path fill="none" stroke="burlywood" stroke-width="0.35" d="M0.5,0 L0.5, 1" />
    </svg>
</template>

  <script>

  import { matrixMultiply, translate, rotate, buildMatrix } from './matrices';

  import { ShortStraightTrack } from './straight-track';

  export default {
    name: 'short-track-layout',

    props: ['model'],

    components: {

    },

    computed: {

      scale() {
        return 100;
      },

      style: function() {

        let originPosition = this.model.getOriginPosition();
        let originAngle = this.model.getOriginRotation();

        console.log('origin', this.model, originPosition, originAngle);

        return {
          'background-color': 'white',
          'transform': 'scale(' + this.scale + ', ' + this.scale + ') rotate(' + (-originAngle) + 'deg)',
          'top': (originPosition.y + this.scale/2) + 'px',
          'left': (originPosition.x) + 'px',
        };
      }
    },

    /*methods: {
      getMatrix() {
        return matrixMultiply(translate(this.x, this.y), matrixMultiply(rotate(-this.angle), straight(1)));
      }
    }*/
  }
  </script>

  <style lang="scss">
  .short-straight-track {
    position: absolute;
  }
  </style>
