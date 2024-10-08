import Vue from 'vue';
import {MIDI_CONTROLS} from './Enums.js';
import {MIDI_IDS} from './Enums.js';

let mdp = new Vue({
  data: function() 
  {
    return {       
      available: false, 
      listeners: {}
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

/*        if(msg.data[1] == MIDI_CONTROLS.speed1) {
          if(this.speedListeners[0]!== undefined) {
            this.speedListeners[0](msg.data[2] / MIDI_CONTROLS.MAX_VALUE);
          }          
        }*/
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