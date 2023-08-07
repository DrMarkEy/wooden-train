/*
  bluetooth.h - Allows controlling the train via BLE
  Created by Marc Mendler, August 2023
*/
#ifndef bluetooth_h
#define bluetooth_h

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

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
	
	void updateEngine(boolean notify)
	{
	  /*byte buffer[BombInfo::LENGTH];
	  bombInfo->serialize(buffer);
	  
	  //Only update if value has changed
	  if(checkBufferChange(lastBombInfo, buffer, BombInfo::LENGTH))	  
	  {				
		Serial.println("Updated Bomb Info.");
	    infoChar->setValue(buffer, BombInfo::LENGTH);
		
	    if(notify)
		  infoChar->notify();
	  }*/
	}
	
	/*
	void updateModuleStates(boolean notify)
	{
	  byte buffer[NUMBER_OF_MODULES + 3];
	  unsigned int bools = 0;
	  
	  buffer[0] = NUMBER_OF_MODULES;
	  for(byte i = 0; i < NUMBER_OF_MODULES; i++)
	  {
		buffer[i + 1] = bombInfo->modules[i].type;  

		//If solved, set ith bit to one
		if(bombInfo->modules[i].solved)
		  bools |= 1 << (16 - i);
	  }

	  buffer[NUMBER_OF_MODULES + 1] = bools;
	  buffer[NUMBER_OF_MODULES + 2] = bools >> 8;  
	  
      //Calculate hash	  	  
	  if(checkBufferChange(lastModuleState, buffer, NUMBER_OF_MODULES + 3))	  
	  {		
	    Serial.println("Updated Module Info.");
	    moduleStatesChar->setValue(buffer, NUMBER_OF_MODULES + 3);
	  
	    if(notify)
		  moduleStatesChar->notify();	  
	  }
	}*/
	
	void onConnected()
	{
		Serial.println("Bluetooth-Client connected.");		
		//updateInfo(true); //Würde eigentlich hier reichen, aber muss dann mit resetted werden, wenn die bombe auf active umschaltet...
	}
	
	void update()
	{
	  if(clientConnected)
	  {	
		updateEngine(true);				
	  }
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
		  engineChar = mainService->createCharacteristic(BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
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
		
	    handleCommands();
		
		//Update with ~ 10 Hz		
		if(millis() > lastUpdate + 100)
		{
			lastUpdate = millis();
			update();			
		}
	}
	
	//Register a callback to a module-Command
	void on(byte command, byte bufferSize, void (*callback) (byte command, byte* buffer, byte bufferSize))
	{		
		BluetoothCommandCallback cbk = { .callback = callback, .bufferSize = bufferSize };		
		commandCallbacks[command] = cbk;
	}
	
	//Remove all command-callbacks
	void clearCallbacks()
	{
		//Reset callbacks to NUlL
		for(byte i = 0; i < BLUETOOTH_COMMAND_COUNT; i++)
		{
			commandCallbacks[i].callback = NULL;
			commandCallbacks[i].bufferSize = 0;
		}
	}
	
	/*void answerCommand(byte* buffer, byte length)
	{		
	    commandAnswerChar->setValue(buffer, length);	  
		commandAnswerChar->notify();		
	}*/
};

#endif