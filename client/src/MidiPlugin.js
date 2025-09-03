import Vue from 'vue';
import {MIDI_CONTROLS, MIDI_IDS, isButtonB} from './Enums.js';

let mdp = new Vue({
  data: function()
  {
    return {
      available: false,
      listeners: {},
      searchNewDeviceCallback: undefined,
      stopSearchingNewDeviceCallback: undefined
    };
  },

  created: function() {
    if (navigator.requestMIDIAccess) {
      navigator.requestMIDIAccess({
        sysex: false
      }).then(this.onMIDISuccess, this.onMIDIFailure);
    } else {
      console.warn("No MIDI support in your browser");
    }
  },

  methods: {
    setSearchNewDeviceCallback: function(callback, stopCallback) {
      this.searchNewDeviceCallback = callback;
      this.stopSearchingNewDeviceCallback = stopCallback;
    },

    onMIDISuccess: function(midi) {
      if(midi.inputs.size > 0) {
        this.available = true;
      }

      midi.onstatechange = (event) => {
        console.log(event.port.state);
        if(event.port.state == 'connected') {
          this.available = true;
        } else if(event.port.state == 'disconnected') {
          this.available = false;
        }
      };

      var allInputs = midi.inputs.values();
      // loop over all available inputs and listen for any MIDI input
      for (var input = allInputs.next(); input && !input.done; input = allInputs.next()) {
        // when a MIDI value is received call the onMIDIMessage function
        input.value.onmidimessage = this.handleMidiMessage;
      }
    },

    handleMidiMessage: function(msg) {
      if(msg.data[0] == MIDI_CONTROLS.device) {

        let button = msg.data[1];
        if(button in this.listeners) {
          this.listeners[button](msg.data[2]);
        }

        else if(isButtonB(button)) {
          if(this.searchNewDeviceCallback !== undefined) {
            if(msg.data[2] == 127) {
              this.searchNewDeviceCallback(); // Note: This does only work if the callback does not directly call an API that requires a user gesture
              //  as midi commands do not count as a user gesture... :-(
            }
            else {
              this.stopSearchingNewDeviceCallback();
            }
          }
        }
      }


      //console.log(msg.data);
    },

    onMIDIFailure: function() {
      alert("midi failure!");
    },

    addController: function(index, listener) {
      let controls = MIDI_IDS[index];

      for(let key in controls) {
        let id = controls[key];
        this.listeners[id] = function(val) {
          listener(key, val);
        };
      }
    },

    removeController: function(index) {
      let controls = MIDI_IDS[index];

      for(let key in controls) {
        let id = controls[key];
        delete this.listeners[id];
      }
    }
  }
});


//Export as vue-plugin
const MidiPlugin = {
    install(Vue) {
        Vue.prototype.$midi = mdp;
    },
};
export default MidiPlugin;