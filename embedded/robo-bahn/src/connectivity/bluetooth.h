/*
  bluetooth.h - Allows controlling the train via BLE (NimBLE-Arduino)
  Migrated to NimBLE July 2025
  Original by Marc Mendler, August 2023
*/
#ifndef bluetooth_h
#define bluetooth_h

#include <NimBLEDevice.h>
#include <http-logger.h>
#include <config.h>
#include <user-command.hpp>

#define BLUETOOTH_COMMAND_COUNT 7

// UUIDs as strings for NimBLE
#define BLUETOOTH_SERVICE_MAIN_UUID            "566804f6-368b-44a4-a375-ce8789eb1dd8"
#define BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID   "c045fb9c-3447-11ee-be56-0242ac120002"
#define BLUETOOTH_CHARACTERISTIC_COMMAND_UUID       "3804213f-d6ad-458e-977c-4f9f1b9f6b9a"
#define BLUETOOTH_CHARACTERISTIC_LIGHT_COLOR_UUID   "c8d7e25c-3447-11ee-be56-0242ac120002"
#define BLUETOOTH_CHARACTERISTIC_COLOR_READING_UUID "45cfdad3-4f02-4e5c-aa16-1c513dc76a3c"
#define BLUETOOTH_CHARACTERISTIC_OPERATION_MODE_UUID "522df05c-fd6c-49cd-8fb7-ea6beb9018f1"

static void connectCallback();
static void disconnectCallback();
static void userCommandCallback(UserCommand* command);

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
    Serial.printf("BLE client address %s connected.", connInfo.getAddress().toString().c_str());
    connectCallback();
  }

  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
    Serial.printf("BLE client address %s disconnected.", connInfo.getAddress().toString().c_str());
    disconnectCallback();
  }
};


class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    Serial.printf("Client sent write request to characteristic %s", pCharacteristic->getUUID().toString().c_str());
    NimBLEAttValue value = pCharacteristic->getValue();

    uint8_t buffer[value.length() - 1];
    memcpy(buffer, value.c_str() + 1, value.length() - 1);

    UserCommand* command = new UserCommand(value.c_str()[0], buffer, value.length() - 1);
    command->print();
    userCommandCallback(command);
  }
};

class BluetoothConnector : public NimBLECharacteristicCallbacks {
public:
    BluetoothConnector() {}

    void Setup() {
        // Disable authentication stuff for better performance
        NimBLEDevice::setSecurityAuth(false);

        // Init device with device name
        NimBLEDevice::init(DEFAULT_WIFI_HOSTNAME);

        NimBLEServer *pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(new ServerCallbacks);

        NimBLEService *pService = pServer->createService(BLUETOOTH_SERVICE_MAIN_UUID);

        // Speed characteristic
        pSpeedChar = pService->createCharacteristic(
            BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pSpeedChar->setCallbacks(this);

        // Light color characteristic
        pLightColorChar = pService->createCharacteristic(
            BLUETOOTH_CHARACTERISTIC_LIGHT_COLOR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pLightColorChar->setCallbacks(this);

        // Command characteristic
        pCommandChar = pService->createCharacteristic(
            BLUETOOTH_CHARACTERISTIC_COMMAND_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pCommandChar->setCallbacks(this);

        // Color reading characteristic (with notify)
        pColorReadingChar = pService->createCharacteristic(
            BLUETOOTH_CHARACTERISTIC_COLOR_READING_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
        );
        pColorReadingChar->setCallbacks(this);

        // Operation mode characteristic (with notify)
        pOperationModeChar = pService->createCharacteristic(
            BLUETOOTH_CHARACTERISTIC_OPERATION_MODE_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
        );
        pOperationModeChar->setCallbacks(this);

        // Set initial values
        uint8_t zeroSpeed = 0;
        pSpeedChar->setValue(&zeroSpeed, 1);

        uint8_t zeroColor[18] = {0};
        pLightColorChar->setValue(zeroColor, 18);

        uint8_t zeroCommand = 0;
        pCommandChar->setValue(&zeroCommand, 1);

        uint8_t zeroReading[4] = {0};
        pColorReadingChar->setValue(zeroReading, 4);

        pOperationMode[0] = 23;
        pOperationModeChar->setValue(pOperationMode, 1);

        pService->start();

        pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(BLUETOOTH_SERVICE_MAIN_UUID);

        // Set name also in scanResponseData
        NimBLEAdvertisementData scanResponseData;
        scanResponseData.setName(DEFAULT_WIFI_HOSTNAME);
        pAdvertising->setScanResponseData(scanResponseData);


        pAdvertising->start();

        logger.Log("Bluetooth initialization finished!");
    }

    // Register a callback for command characteristic
    void onCommand(void (*callback)(uint8_t*, size_t)) { commandCallback = callback; }
    void onSpeedChanged(void (*callback)(uint8_t*, size_t)) { speedCallback = callback; }
    void onColorChanged(void (*callback)(uint8_t*, size_t)) { colorCallback = callback; }

    // Set sensor color and notify
    void setSensorColor(uint8_t color[1]) {
        if (pColorReadingChar) {
            pColorReadingChar->setValue(color, 1);
            pColorReadingChar->notify();
        }
    }

    // Set operation mode and notify
    void setOperationMode(uint8_t operationMode) {
        if (pOperationModeChar) {
            pOperationMode[0] = operationMode;
            pOperationModeChar->setValue(pOperationMode, 1);
            pOperationModeChar->notify();
        }
    }

    // Set speed value
    void setSpeed(uint8_t speed) {
        if (pSpeedChar) pSpeedChar->setValue(&speed, 1);
    }

    // Set light color value
    void setLightColor(uint8_t* color, size_t len) {
        if (pLightColorChar) pLightColorChar->setValue(color, len);
    }

    // Set command value
    /*void setCommand(uint8_t command) {
        if (pCommandChar) pCommandChar->setValue(&command, 1);
    }*/

protected:
    // NimBLECharacteristicCallbacks overrides
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        Serial.printf("Client sent write request to characteristic %s", pCharacteristic->getUUID().toString().c_str());

        std::string uuid = pCharacteristic->getUUID().toString();
        std::string value = pCharacteristic->getValue();

        if (uuid == BLUETOOTH_CHARACTERISTIC_COMMAND_UUID && commandCallback) {
            commandCallback((uint8_t*)value.data(), value.length());


/*
        NimBLEAttValue value = pCharacteristic->getValue();

        uint8_t buffer[value.length() - 1];
        memcpy(buffer, value.c_str() + 1, value.length() - 1);

        UserCommand* command = new UserCommand(value.c_str()[0], buffer, value.length() - 1);
        command->print();
        userCommandCallback(command);*/



        } else if (uuid == BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID && speedCallback) {
            speedCallback((uint8_t*)value.data(), value.length());
        } else if (uuid == BLUETOOTH_CHARACTERISTIC_LIGHT_COLOR_UUID && colorCallback) {
            colorCallback((uint8_t*)value.data(), value.length());
        }
        // Color reading characteristic is only written by the device itself

    }

public:
    // Register a callback for connection events
    void onConnectionEvent(void (*callback)(uint8_t event)) {
        connectionCallback = callback;
    }

    void onClientConnect() {
        if(connectionCallback != nullptr) {
            connectionCallback(BLUETOOTH_CONNECTION_ESTABLISHED);
        }
    }

    void onClientDisconnect() {
        if(connectionCallback != nullptr) {
            connectionCallback(BLUETOOTH_CONNECTION_TERMINATED);
        }
        if (pAdvertising) {
            pAdvertising->start();
        }
    }

  // Placeholder for handling received UserCommand objects.
  void onCommandReceived(UserCommand* userCommand) {

  }


private:
    NimBLEAdvertising *pAdvertising = nullptr;

    uint8_t pOperationMode[1];

    NimBLECharacteristic *pSpeedChar = nullptr;
    NimBLECharacteristic *pLightColorChar = nullptr;
    NimBLECharacteristic *pCommandChar = nullptr;
    NimBLECharacteristic *pColorReadingChar = nullptr;
    NimBLECharacteristic *pOperationModeChar = nullptr;

    void (*commandCallback)(uint8_t*, size_t) = nullptr;
    void (*speedCallback)(uint8_t*, size_t) = nullptr;
    void (*colorCallback)(uint8_t*, size_t) = nullptr;
    void (*connectionCallback)(uint8_t event) = nullptr;
};

extern BluetoothConnector bluetooth;

void connectCallback() {
  bluetooth.onClientConnect();
}

void disconnectCallback() {
  bluetooth.onClientDisconnect();
}

void userCommandCallback(UserCommand* command) {
  bluetooth.onCommandReceived(command);
}


#endif