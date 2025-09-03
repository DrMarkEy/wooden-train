<template>
    <svg class="curved-track" :style="style" width="1px" height="1px" >
        <path fill="none" stroke="burlywood" stroke-width="0.35" :d="d" />
    </svg>
</template>

  <script>

  export default {
    name: 'curved-track',

    props: ['model'],

    components: {

    },

    computed: {
      scale() {
        return 100;
      },

      d: function() {
        if(this.model.getTransformationRotation() > 0) {
          // Right curve
          let x = 0;
          let y = 1;
          let r = Math.sqrt(2);

          return "M0.5,-0.1 A"+r+" "+r+" 0 0 1 "+x+" "+y;
        }
        else {
          // Left curve
          let x = 1;
          let y = 1;
          let r = Math.sqrt(2);

          return "M0.5,-0.1 A"+r+" "+r+" 0 0 0 "+x+" "+y;
        }
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
    },
  }
  </script>

  <style lang="scss">
  .curved-track {
    position: absolute;
  }
  </style>
