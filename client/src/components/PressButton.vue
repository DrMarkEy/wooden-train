<template>
      <button :class="{'press-button': true, 'pressed': pressed}" @click="press">
        <img :src="command.icon"/>
      </button>
  </template>

  <script>

  export default {
    name: 'press-button',

    props: ['command', 'pressed', 'connection'],

    computed: {

      mSelectionA: {
        get: function() {
          return this.selectionA;
        },

        set: function(val) {
          this.$emit('change-selection-a', val);
        }
      }
    },

    methods: {
      press: function() {
        this.connection.sendCommand(this.command.command);
      },
    }
  }
  </script>

  <style lang="scss">
  .press-button
  {
    position: absolute;
    left: 15px;
    width: 40px;
    height: 40px;
    border-radius: 5px;
    border-style: solid;
    border-width: 1px;
    border-color: #888;
    box-shadow: 2px 1px 1px #666;
    user-select: none;
    cursor: pointer;

    img
    {
      width: 30px;
    }

    &:active, &.pressed
    {
      box-shadow: inset 0px 0px 2px #555;
    }
  }
  </style>
