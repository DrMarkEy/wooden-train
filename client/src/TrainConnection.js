import BLEConnection from './BLEConnection.js';
import Vue from 'vue';

import {UUIDS} from './Enums.js';

let TRAIN_COMMAND = {
  START: 3,
  STOP: 4,
  REVERSE: 5,
  WHISTLE: 6
};

let OPERATION_MODE = {
  STOPPED: 1,
  DRIVING: 2,
  DRIVING_REVERSE: 3,
  CALLING_AT_STATION: 4
};


class TrainConnection extends BLEConnection {
  constructor(device, server) {
    super(device, server);

    this.colorSensor = Vue.observable({
      colorCode: -1,
      color: 'transparent'
    });

    this.operation = Vue.observable({
      mode: -1
    });

    // Bind event handlers
    this.colorReadingReceived = this.colorReadingReceived.bind(this);
    this.operationModeChanged = this.operationModeChanged.bind(this);
  }

  async setup() {
    if(this.server === undefined)
      return;

    let service = await this.server.getPrimaryService(UUIDS.train.service);
    this.engineSpeedCharacteristic = await service.getCharacteristic(UUIDS.train.engineSpeed);
    this.lightColorCharacteristic = await service.getCharacteristic(UUIDS.train.lightColor);
    this.commandCharacteristic = await service.getCharacteristic(UUIDS.train.command);
    this.stateCharacteristic = await service.getCharacteristic(UUIDS.train.state);

    let thi = this;
    /*await this.colorReadingCharacteristic.startNotifications();
    this.colorReadingCharacteristic.addEventListener('characteristicvaluechanged', (e) => {
      thi.colorReadingReceived(e.target.value);
    });
    this.colorReadingReceived(await this.colorReadingCharacteristic.readValue());*/

    await this.stateCharacteristic.startNotifications();
    this.stateCharacteristic.addEventListener('characteristicvaluechanged', (e) => {
      thi.operationModeChanged(e.target.value);
    });
    this.operationModeChanged(await this.stateCharacteristic.readValue());

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

  async setLightsColor(led1, led2, led3, led4, led5, led6) {
    if(this.connected) {

      let colorsValue = new Uint8Array([
        led1.r, led1.g, led1.b,
        led2.r, led2.g, led2.b,
        led3.r, led3.g, led3.b,
        led4.r, led4.g, led4.b,
        led5.r, led5.g, led5.b,
        led6.r, led6.g, led6.b
      ]);

      try
      {
        await this.lightColorCharacteristic.writeValue(colorsValue);
      }
      catch(ex)
      {
        this.reconnect();
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

  /*async requestColorReading() {
    try
    {
      let result = await this.colorReadingCharacteristic.readValue();
      console.log('color', [result.getUint8(),result.getUint8(),result.getUint8(),result.getUint8()]);
    }
    catch(ex)
    {
      console.error("Error reading sensor color!");
    }
  }*/

  async readState() {
    try
    {
      let result = await this.stateCharacteristic.readValue();
      console.log('state characteristic', result.getUint8());
    }
    catch(ex)
    {
      console.error("Error reading operation mode!");
    }
  }

  colorReadingReceived(value)
  {
    this.colorSensor.colorCode = value.getUint8(0);
  }

  operationModeChanged(value)
  {
    this.operation.mode = value.getUint8(0);
  }
}

export default TrainConnection;
export {TRAIN_COMMAND, OPERATION_MODE};