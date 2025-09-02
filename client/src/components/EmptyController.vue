<template>
    <div class="empty-controller">
        <div class="circle" @click="searchBluetoothDevice">
          <div class="inner">+</div>
        </div>
    </div>
</template>

<script>

  import TrainConnection from '../TrainConnection.js';


    function nextColor(index) {
    switch(index) {
        case 0: return '#ff0000';
        case 1: return '#00ff00';
        case 2: return '#0000ff';
        case 3: return '#ffff00';
        case 4: return '#00ffff';
        case 5: return '#ff00ff';
        case 6: return '#ff8800';
        case 7: return '#ffffff';
        case 8: return '#5382ee';
    }
    }

  export default {
    name: 'empty-controller',

    props: ['existing-controllers'],

    components: {

    },

    mounted: function() {
      //this.$midi.addController(this.index, this.handleMidiCommand);
    },

    data: function() {
      return {};
    },

    methods: {
      searchBluetoothDevice: async function() {
        let connection = await TrainConnection.searchDevice(this.existingControllers.map(cont => cont.connection));

        if(connection !== undefined) {
          this.addTrainController(connection);

          setTimeout(function(){
            connection.setup();
          }, 10);
        }
      },

      addTrainController: function(trainConnection) {
        this.$emit('add-controller', {
          type: 'train',
          ledColor: nextColor(this.existingControllers.length),
          selectionA: 0,
          selectionB: 1,
          connection: trainConnection
        });
      },

      handleMidiCommand: function(command, value) {
        switch(command) {
          case 'slider':
            this.speed = value;
          break;

          case 'knob':
            this.mSelectionA = Math.round(value / 127  * (this.modeCount - 1));
          break;

          case 'buttonA':
            if(value > 64) {
              this.pressedA = true;
              this.$refs.buttonA.press();
            }
            else {
              this.pressedA = false;
            }
          break;

          case 'buttonB':
            if(value > 64) {
              this.pressedB = true;
              this.$refs.buttonB.press();
            }
            else {
              this.pressedB = false;
            }
          break;

          default:
            console.log("Unknown command "+command);
          break;
        }
      }
    }
  }
  </script>

  <style lang="scss">
  .empty-controller
  {
    position: relative;
    width: 100px;
    height: 320px;
    border-style: dotted;
    border-width: 3px;
    border-color: #aaa;
    background-color: #ccc;
    margin: 5px;

    .circle
    {
      left: calc(50% - 20px);
      top: calc(50% - 20px);
      position: absolute;
      background-color: #ccc;
      width: 40px;
      height: 40px;
      border-radius: 40px;
      border-style: dashed;
      border-width: 2px;
      border-color: #999;

      .inner
      {
        font-size: 24px;
        color: #999;
        margin-top: 5px;
      }

      &:hover
      {
        background-color: cornflowerblue;
        cursor: pointer;
        border-style: solid;
        border-color: #333;
        box-shadow: 1px 1px 2px #666;

        .inner
        {
          color:#333;
        }
      }
    }
  }
  </style>
