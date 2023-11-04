import {UUIDS} from './Enums.js';
import TrainConnection from './TrainConnection.js';

class BLEConnection {
  static async searchDevice(connectedDevices) {
      
      try
      {

        // Exclude devices that are already connected
        let exclusionFilters = connectedDevices.map(con => { return {name: con.device.name};});
        exclusionFilters.push({name: "_EMPTY FILTER_"}); // Add pseudo element, because an empty filter acts as "remove all"

        // Scan for device with one of the supported services
        let device = await navigator.bluetooth.requestDevice({
          filters: [{ services: [UUIDS.train.service] }], // TODO: Add signal, switch and station services
          exclusionFilters: exclusionFilters
        });
     
        let server = await device.gatt.connect();    

        let services = await server.getPrimaryServices();
        let type = services[0].uuid;
        switch(type) {
          case UUIDS.train.service:
            return new TrainConnection(device, server);

          // TODO: Add other connection types
        }        

        return;
      }    
      catch(ex)
      {
        return;
      }              
  }

  constructor(device, server) {
    this.device = device;
    this.server = server;
    this.connected = false;    
        
    let thi = this;
    this.device.addEventListener('gattserverdisconnected', () => this.onConnectionClosed.apply(thi)); 
    setTimeout(this.connectionCheck, 500);
  }

  setup() {
    this.connected = true;
  }

  connectionCheck() {
    if(this.server !== undefined && this.server.connected === false)
    {
      this.onConnectionClosed();  
    }      
    else
    {
      setTimeout(this.connectionCheck, 500);
    }
  }

  onConnectionClosed() {
    console.log('Connection lost.', this);
    
    try {
      this.server.disconnect();
    }
    catch(ex) {}

    this.device = undefined;
    this.server = undefined;    
    
    if(this.onConnectionClosedListener !== undefined) {
      this.onConnectionClosedListener();
    }
  }

  setOnConnectionClosedListener(listener) {
    this.onConnectionClosedListener = listener;
  }

  isConnected() {
    return this.connected;
  }
}

export default BLEConnection;