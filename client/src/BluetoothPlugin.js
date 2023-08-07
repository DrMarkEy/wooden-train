import Vue from 'vue';
import {UUIDS, STATE, TIMING} from './Enums.js';

let CONNECTION_STATE = {
    DISCONNECTED: 0,
    CHOOSE_DEVICE: 1,
    CONNECTING: 2,
    CONNECTED: 3
};

let btp = new Vue({
  data: function() 
  {
    return {
      state: CONNECTION_STATE.DISCONNECTED,            
    
      device: undefined,
      server: undefined,      
    
      engineSpeedCharacteristic: undefined,
      commandCharacteristic: undefined,
      
      lastCommandAnswerCode: undefined,
      lastCommandAnswerBuffer: undefined
    };
  },
  
  beforeUnmount: function() {
    this.disconnect();  
  },
  
  methods: {          
    checkModuleAnswer: function(resolve, answerCode) {
      if(this.lastCommandAnswerCode === answerCode)
      {
        this.lastCommandAnswerCode = undefined;
        resolve(this.lastCommandAnswerBuffer);
        this.lastCommandAnswerBuffer = undefined;
      }
      else
      {
        let thi = this;
        setTimeout(function() {thi.checkModuleAnswer(resolve, answerCode);}, 100);
      }
    },  
  
    sendCommand: function(command, answerCode) 
    {
      //Convert argument to array
      if(!Array.isArray(command))
        command = [command];
        
      let commandValue = new Uint8Array(command);
      
      try
      {
        this.commandCharacteristic.writeValue(commandValue);
        
        if(answerCode !== undefined)
        {
          let thi = this;
          this.pendingCommandAnswer = answerCode;          
          return new Promise((resolve) => setTimeout(function() {thi.checkModuleAnswer(resolve, answerCode);}, 100));
        }
      }
      catch(ex)
      {
        this.reconnect();
      }
    },
    

    handleCommandAnswers: function(event) {
      var buffer = event.target.value;     

      if(buffer.byteLength === 0)
        return;      
    
      //Save answer
      this.lastCommandAnswerCode = buffer.getUint8(0);
      this.lastCommandAnswerBuffer = buffer;
    },
    
    /*startListening: async function(service, uuid, callback)
    {
      var characteristic = await service.getCharacteristic(uuid);
      characteristic.startNotifications();
      characteristic.addEventListener('characteristicvaluechanged', callback);
      
      let res = await characteristic.readValue();
      callback({target: {value: res}});      
    },*/
    
    connect: async function() { 
      this.state = CONNECTION_STATE.CHOOSE_DEVICE;      
      
      try
      {
        // Step 1: Scan for a device with the general service
        this.device = await navigator.bluetooth.requestDevice({
          filters: [{ services: [UUIDS.service] }]
        });
      }    
      catch(ex)
      {
        this.state = CONNECTION_STATE.DISCONNECTED;        
        return;
      }        

      this.device.addEventListener( 'gattserverdisconnected', this.onConnectionClosed);
      this.reconnect();
    },
    
    reconnect: async function() {
      this.state = CONNECTION_STATE.CONNECTING;
     
      this.server = await this.device.gatt.connect();    
      var service = await this.server.getPrimaryService(UUIDS.service);
    
      /*this.startListening(service, UUIDS.motorSpeed, this.updateBombState);
      this.startListening(service, UUIDS.command, this.updateBombInfos);
      this.startListening(service, UUIDS.commandAnswers, this.handleCommandAnswers);    
      this.startListening(service, UUIDS.moduleInfos, this.updateModuleInfos);*/                
           
      this.engineSpeedCharacteristic = await service.getCharacteristic(UUIDS.engineSpeed);
      this.commandCharacteristic = await service.getCharacteristic(UUIDS.command);
      
      setTimeout(this.checkConnected, 500);
    },
    
    checkConnected: function() {
      if(this.server.connected === false)
      {
        this.disconnect();
      }
      //Continue to check the connection until connected or disconnected
      else if(this.state !== CONNECTION_STATE.CONNECTED)
      {
        setTimeout(this.checkConnected, 500);
      }
    },
    
    onConnectionClosed: function() {
      console.log('Connection lost.');
      this.disconnect();
    },
    
    disconnect: async function() {                  
      //Abort any type of connection
      if(this.state !== CONNECTION_STATE.DISCONNECTED)
      {
        this.state = CONNECTION_STATE.DISCONNECTED;
          
        this.$nextTick(function() {
          this.server.disconnect();
          this.server = undefined;
          this.device = undefined;        
          this.modules = [];        
          this.bombInfos = defaultBombInfos;    
          this.bombState = defaultBombState;
          this.commandCharacteristic = undefined;        
        });
      }
    }    
  },
  
  computed: {
    ready: function() {
      return this.device !== undefined;// && this.bombInfos.serial !== '' && this.bombState.state !== STATE.DISCONNECTED && this.modules.length > 0;
    },        
    
    deviceName: function() {
        if(this.device === undefined)
            return undefined;
        
        return this.device.name;
    }
  },

  watch: {
    ready: function(val) {
      if(val === true)
        this.state = CONNECTION_STATE.CONNECTED;
    }
  }    
});


//Export as vue-plugin
const BluetoothPlugin = {
    install(Vue) {              
        Vue.prototype.$bluetooth = btp;
        Vue.prototype.CONNECTION_STATE = CONNECTION_STATE;
    },
};
export default BluetoothPlugin;