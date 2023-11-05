import BLEConnection from './BLEConnection.js';

import {UUIDS} from './Enums.js';

let TRAIN_COMMAND = {
  START: 4,    
  STOP: 5,
  WHISTLE: 7
};


class TrainConnection extends BLEConnection {
  constructor(device, server) {
    super(device, server);    
  }

  async setup() {
    if(this.server === undefined)
      return;

    let service = await this.server.getPrimaryService(UUIDS.train.service);
    this.engineSpeedCharacteristic = await service.getCharacteristic(UUIDS.train.engineSpeed);
    this.commandCharacteristic = await service.getCharacteristic(UUIDS.train.command);
    
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

  async sendCommand(command) //, answerCode
  {
    //Convert argument to array
    if(!Array.isArray(command))
      command = [command];
      
    let commandValue = new Uint8Array(command);
      
    try
    {
      this.commandCharacteristic.writeValue(commandValue);
        
      /*
      if(answerCode !== undefined)
      {
        let thi = this;
        this.pendingCommandAnswer = answerCode;          
        return new Promise((resolve) => setTimeout(function() {thi.checkModuleAnswer(resolve, answerCode);}, 100));
      }*/
    }
    catch(ex)
    {
      //this.reconnect();
      // TODO!
    }
  }
}

export default TrainConnection;
export {TRAIN_COMMAND};