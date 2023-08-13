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

#include <http-logger.h>
#include <config.h>


//https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/BLE_notify/BLE_notify.ino


//ACHTUNG: 5 Charakteristiken sind das Maximum!!!
//Alles darüber funktioniert einfach nicht, aber liefert keine Fehlermeldung...

//Der Wert einer Charakteristik darf höchstens 23 bytes groß sein...


//#define BLUETOOTH_SERVICE_MAIN_UUID "ba451494-3447-11ee-be56-0242ac120002"

static uint8_t BLUETOOTH_SERVICE_MAIN_UUID[16] = {
    0xba, 0x45, 0x14, 0x94, 0x34, 0x47, 0x11, 0xee, 0xbe, 0x56, 0x02, 0x42, 0xac, 0x12, 0x00, 0x02
};


#define BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID "c045fb9c-3447-11ee-be56-0242ac120002"
#define BLUETOOTH_CHARACTERISTIC_COMMAND_UUID "c8d7e25c-3447-11ee-be56-0242ac120002"

#define BLUETOOTH_CONNECTION_ESTABLISHED 1
#define BLUETOOTH_CONNECTION_TERMINATED 2

#define BLUETOOTH_COMMAND_NONE 3
#define BLUETOOTH_COMMAND_START 4
#define BLUETOOTH_COMMAND_STOP 5
#define BLUETOOTH_COMMAND_REVERSE 6

#define BLUETOOTH_COMMAND_COUNT 6

#define GATTS_SERVICE_UUID_TEST_A   0x00FF


struct BluetoothCommandCallback
{
	void (*callback) (byte command, byte* buffer, byte bufferSize);	
	byte bufferSize;
};


static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0007,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 16,
    .p_service_uuid = BLUETOOTH_SERVICE_MAIN_UUID,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    //.min_interval = 0x0006,
    //.max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(BLUETOOTH_SERVICE_MAIN_UUID),
    .p_service_uuid = BLUETOOTH_SERVICE_MAIN_UUID,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};


struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t adv_config_done = 0;
#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)



    // Declaration
    static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);


	static struct gatts_profile_inst gattsProfile = { 
		.gatts_cb = gatts_profile_a_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
	};


static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
        switch (event) {
        case ESP_GATTS_REG_EVT:
             logger->Logf("REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
             gattsProfile.service_id.is_primary = true;
             gattsProfile.service_id.id.inst_id = 0x00;
             gattsProfile.service_id.id.uuid.len = ESP_UUID_LEN_16;
             gattsProfile.service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_TEST_A;

             esp_ble_gap_set_device_name(DEFAULT_WIFI_HOSTNAME);

            //config adv data
            esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
            if (ret){
                logger->Log("config adv data failed, error code = "+ret);
            }
            adv_config_done |= adv_config_flag;
            //config scan response data
            ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (ret){
                logger->Log("config scan response data failed, error code = "+ ret);
            }
            adv_config_done |= scan_rsp_config_flag;
		}
	}

class BluetoothConnector
{
  private:
	boolean clientConnected;  
    int lastUpdate = 0;
	BluetoothCommandCallback commandCallbacks[BLUETOOTH_COMMAND_COUNT];
	void (*engineSpeedChangedCallback) (byte speed) = nullptr;

	/*BLEServer *bluetoothConnection;
	BLEService *mainService;
	
	BLECharacteristic *engineChar;
    BLECharacteristic *commandChar;*/
	
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
	  /*//Read command
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
	  commandChar->setValue(none, 1);*/	  	  
	}	

	void updateEngineSpeed()
	{
	  /*//Read speed
	  byte *buffer = engineChar->getData();
	  
	  byte val = *buffer;	  
	  
	  //Handle command if listener is registered
	  if(engineSpeedChangedCallback != NULL)
	  {		
		engineSpeedChangedCallback(val);
	  }	  	 */
	}

	static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
    {
        switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~adv_config_flag);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~scan_rsp_config_flag);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            //advertising start complete event to indicate advertising start successfully or failed
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                logger->Log("Advertising start failed");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                logger->Log("Advertising stop failed");
            } else {
                logger->Log("Stop adv successfully");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
             logger->Logf( "update connection params status = %d, min_int = %d, max_int = %d, conn_int = %d, latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
			break;
        default:
            break;
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





    esp_err_t ret;
    	
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    
	// TODO: Eventually adjust settings to be even faster!

    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        logger->Log("BluetoothConnector initialize controller failed");
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        logger->Log("BluetoothConnector enable controller failed");
        return;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        logger->Log("BluetoothConnector init bluetooth failed");
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        logger->Log("BluetoothConnector enable bluetooth failed");
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_profile_a_event_handler);
    if (ret){
        logger->Log("gatts register error, error code = " + ret);
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        logger->Log("gap register error, error code = " + ret);
        return;
    }
    ret = esp_ble_gatts_app_register(0);
    if (ret){
        logger->Log("gatts app register error, error code = " + ret);
        return;
    }
    /*ret = esp_ble_gatts_app_register(PROFILE_B_APP_ID);
    if (ret){
        logger->Log("gatts app register error, error code = " + ret);
        return;
    }*/
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(512);
    if (local_mtu_ret){
        logger->Log("set local  MTU failed, error code = " + local_mtu_ret);
    }
    return;

		/*
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
		  BLEDevice::startAdvertising();*/
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