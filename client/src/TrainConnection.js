import BLEConnection from './BLEConnection.js';

import {UUIDS} from './Enums.js';

class TrainConnection extends BLEConnection {
  constructor(device, server) {
    super(device, server);    
  }

  async setup() {
    if(this.server === undefined)
      return;

    let service = await this.server.getPrimaryService(UUIDS.train.service);
    this.engineSpeedCharacteristic = await service.getCharacteristic(UUIDS.train.engineSpeed);
    
    this.engineCharacteristicLock = false;
    this.requestedEngineSpeed = 0;

    // Only call this method last to set connected flag not too early
    super.setup();
  }

  async setSpeed(speed) 
  {        
    if(this.connected) {   
        
        if(this.engineCharacteristicLock) 
        {
          this.requestedEngineSpeed = speed;
          console.log("Requested speed of ", speed);
        }
        else
        { 
          this.engineCharacteristicLock = true;
          console.log('Set speed to', speed);

          let speedValue = new Uint8Array([speed]);          

          try
          {
            await this.engineSpeedCharacteristic.writeValue(speedValue);            
            this.engineCharacteristicLock = false;
            //console.log('Released speed lock');

            if(this.requestedEngineSpeed !== undefined) {
              let speed = this.requestedEngineSpeed;
              this.requestedEngineSpeed = undefined;              
              this.setSpeed(speed);
            }
          }
          catch(ex)
          {
            this.reconnect();
          }
        }
      }
    }
}

export default TrainConnection;