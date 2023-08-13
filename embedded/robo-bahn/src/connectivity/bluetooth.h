/*
  bluetooth.h - Allows controlling the train via BLE
  Created by Marc Mendler, August 2023
*/
#ifndef bluetooth_h
#define bluetooth_h

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include <config.h>


//https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/BLE_notify/BLE_notify.ino


//ACHTUNG: 5 Charakteristiken sind das Maximum!!!
//Alles darüber funktioniert einfach nicht, aber liefert keine Fehlermeldung...

//Der Wert einer Charakteristik darf höchstens 23 bytes groß sein...


#define BLUETOOTH_SERVICE_MAIN_UUID "ba451494-3447-11ee-be56-0242ac120002"

#define BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID "c045fb9c-3447-11ee-be56-0242ac120002"
#define BLUETOOTH_CHARACTERISTIC_COMMAND_UUID "c8d7e25c-3447-11ee-be56-0242ac120002"

#define BLUETOOTH_CONNECTION_ESTABLISHED 1
#define BLUETOOTH_CONNECTION_TERMINATED 2

#define BLUETOOTH_COMMAND_NONE 3
#define BLUETOOTH_COMMAND_START 4
#define BLUETOOTH_COMMAND_STOP 5
#define BLUETOOTH_COMMAND_REVERSE 6

#define BLUETOOTH_COMMAND_COUNT 6


struct BluetoothCommandCallback
{
	void (*callback) (byte command, byte* buffer, byte bufferSize);	
	byte bufferSize;
};

class ServerCallbacks: public BLEServerCallbacks 
{       
    public:
    ServerCallbacks(boolean* connected,  void (*connectedCallback) ())
    {
       connectedPointer = connected;
	   onConnectedCallback = connectedCallback;
    }
    
    void onConnect(BLEServer* pServer) 
    {
      *connectedPointer = true;
	  onConnectedCallback();

      // Set fast connection params
	  BLEAddress bleAddress = BLEDevice::getAddress();
	  pServer->updateConnParams(*(bleAddress.getNative()), (uint16_t)6, (uint16_t)10, (uint16_t)0, (uint16_t)1000);
	  Serial.println("BLE Client connected.");
    }

    void onDisconnect(BLEServer* pServer) {
      *connectedPointer = false;

      BLEDevice::startAdvertising();
      
	  Serial.println("Client disconnected.");
    }    

    private:
    boolean* connectedPointer;
	void (*onConnectedCallback) ();
};


class BluetoothConnector
{
  private:
	boolean clientConnected;  
    int lastUpdate = 0;
	BluetoothCommandCallback commandCallbacks[BLUETOOTH_COMMAND_COUNT];
	void (*engineSpeedChangedCallback) (byte speed) = nullptr;

	BLEServer *bluetoothConnection;
	BLEService *mainService;
	
	BLECharacteristic *engineChar;
    BLECharacteristic *commandChar;
	
	bool checkBufferChange(byte* oldValue, byte* newValue, byte length)
	{
		bool ret = false;
		for(byte i = 0; i < length; i++)
		{
			if(oldValue[i] != newValue[i])
			{
				oldValue[i] = newValue[i];
				ret = true;
			}
		}
		
		return ret;
	}
	
	void onConnected()
	{
		Serial.println("Bluetooth-Client connected.");		
	}
	
	void handleCommands()
	{
	  //Read command
	  byte *buffer = commandChar->getData();
	  
	  byte val = *buffer;	  
	  if(val == BLUETOOTH_COMMAND_NONE)
	  {		
		return;
	  }	  	 
	  
	  //Handle command if listener is registered
	  else if(commandCallbacks[val].callback != NULL)
	  {		
		commandCallbacks[val].callback(val, buffer + 1, commandCallbacks[val].bufferSize);
	  }
	  
	  //Unset command	  
	  byte none[1];
	  none[0] = BLUETOOTH_COMMAND_NONE;
	  commandChar->setValue(none, 1);	  	  
	}	

	void updateEngineSpeed()
	{
	  //Read speed
	  byte *buffer = engineChar->getData();
	  
	  byte val = *buffer;	  
	  
	  //Handle command if listener is registered
	  if(engineSpeedChangedCallback != NULL)
	  {		
		engineSpeedChangedCallback(val);
	  }	  	 
	}

  public:    			
	
	BluetoothConnector()
	{
		//Initialize callbacks as NUlL
		for(byte i = 0; i < BLUETOOTH_COMMAND_COUNT; i++)
		{
			commandCallbacks[i].callback = NULL;
			commandCallbacks[i].bufferSize = 0;
		}
		
		//Create Server		
		BLEDevice::init(DEFAULT_WIFI_HOSTNAME);
		bluetoothConnection = BLEDevice::createServer();
		
		static BluetoothConnector* thi = this;
		bluetoothConnection->setCallbacks(new ServerCallbacks(&clientConnected, []() {
		  thi->onConnected();
		}));
		  
		   //------- Create STATE-Service ------------
		  mainService = bluetoothConnection->createService(BLUETOOTH_SERVICE_MAIN_UUID);  

		  // Engine
		  engineChar = mainService->createCharacteristic(BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
		  //updateInfo(false);  
		  engineChar->addDescriptor(new BLE2902()); //Allows notify

		  //Commands
		  commandChar = mainService->createCharacteristic(BLUETOOTH_CHARACTERISTIC_COMMAND_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);  
		  byte none[1];
		  none[0] = BLUETOOTH_COMMAND_NONE;
		  commandChar->setValue(none, 1);  

		  //Start main service
		  mainService->start();
		  

		  //--------------- Start Advertising --------------
		  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
		  pAdvertising->addServiceUUID(BLUETOOTH_SERVICE_MAIN_UUID);
		  pAdvertising->setScanResponse(true);
		  
		  // functions that help with iPhone connections issue
		  pAdvertising->setMinPreferred(0x06);
		  pAdvertising->setMinPreferred(0x12);
		  BLEDevice::startAdvertising();
	}	
	
		
	void Loop()
	{
		if(!clientConnected)
			return;	    
		
		//Update with ~ 100 Hz		
		if(millis() > lastUpdate + 10)
		{
			lastUpdate = millis();
			//update();			
			handleCommands();
			updateEngineSpeed();
		}
	}
	
	//Register a callback to a module-Command
	void on(byte command, byte bufferSize, void (*callback) (byte command, byte* buffer, byte bufferSize))
	{		
		BluetoothCommandCallback cbk = { .callback = callback, .bufferSize = bufferSize };		
		commandCallbacks[command] = cbk;
	}

	void onSpeedChanged(void (*callback) (byte speed))
	{		
		this->engineSpeedChangedCallback = callback;
	}
};

#endif