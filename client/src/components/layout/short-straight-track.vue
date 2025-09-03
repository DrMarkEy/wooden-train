<template>
    <svg class="short-straight-track" :style="style" width="1px" height="1px" >
        <path fill="none" stroke="burlywood" stroke-width="0.35" d="M0.5,0 L0.5, 1" />
    </svg>
</template>

  <script>

  export default {
    name: 'short-straight-track',

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

        return {
          'transform': 'scale(' + this.scale + ', ' + this.scale + ') rotate(' + (originAngle) + 'deg)',
          'top': (originPosition.y + this.scale/2) + 'px',
          'left': (originPosition.x) + 'px',
        };
      }
    }
  }
  </script>

  <style lang="scss">
  .short-straight-track {
    position: absolute;
  }
  </style>
