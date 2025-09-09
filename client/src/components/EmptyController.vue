<template>
  <div class="empty-controller-container">
    <div :class="{'empty-controller': true, 'waiting-for-space': showOverlay}">
        <div class="circle" @click="searchBluetoothDevice">
          <div class="inner">+</div>
        </div>
    </div>

    <div class="waiting-for-space-overlay" v-if="showOverlay" @click="stopWaitingForSpace">
          Leertaste zum Hinzufügen eines Controllers drücken
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
      this.$midi.setSearchNewDeviceCallback(this.waitForSpace, this.stopWaitingForSpace);

      window.addEventListener('keydown', (event) => {
        if(event.code === 'Space') {
          this.handleSpaceKey();
        }
      });
    },

    data: function() {
      return {showOverlay: false};
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

      waitForSpace: function() {
        this.showOverlay = true;
      },

      stopWaitingForSpace: function() {
        this.showOverlay = false;
      },

      handleSpaceKey: function() {
        if(this.showOverlay) {
          this.showOverlay = false;
          this.searchBluetoothDevice();
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
      }
    }
  }
  </script>

  <style lang="scss">

  .empty-controller-container {
    position: relative;
  }

  .empty-controller
  {
    position: relative;
    width: 100px;
    height: 275px;
    border-style: dotted;
    border-width: 3px;
    border-color: #aaa;
    background-color: #ccc;
    margin: 5px;

    &.waiting-for-space {
      border-color: cornflowerblue;
      z-index: 100;
      background-color: #eee;

      .circle {
        background-color: cornflowerblue;
        border-style: solid;
        border-color: #333;
        box-shadow: inset 1px 1px 2px #666;

        .inner
        {
          display:none
        }
      }
    }

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

   .waiting-for-space-overlay {
      position: fixed;
      top: 0px;
      left: 0px;
      width: 100%;
      height: 100%;
      background-color: rgba(200, 200, 200, 0.8);
      color: #444;
      font-size: 18px;
      display: flex;
      align-items: center;
      justify-content: center;
      text-align: center;
      font-weight: bold;
      user-select: none;
    }
  </style>
