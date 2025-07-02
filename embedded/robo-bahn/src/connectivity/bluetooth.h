/*
  bluetooth.h - Allows controlling the train via BLE (NimBLE-Arduino)
  Refactored July 2025
  Original by Marc Mendler, August 2023
*/
#ifndef bluetooth_h
#define bluetooth_h

#include <NimBLEDevice.h>
#include <http-logger.h>
#include <config.h>

#define BLUETOOTH_CONNECTION_ESTABLISHED 1
#define BLUETOOTH_CONNECTION_TERMINATED 2

#define BLUETOOTH_COMMAND_NONE 3
#define BLUETOOTH_COMMAND_START 4
#define BLUETOOTH_COMMAND_STOP 5
#define BLUETOOTH_COMMAND_REVERSE 6
#define BLUETOOTH_COMMAND_WHISTLE 7

#define BLUETOOTH_COMMAND_COUNT 7

// UUIDs as strings for NimBLE
#define BLUETOOTH_SERVICE_MAIN_UUID            "566804f6-368b-44a4-a375-ce8789eb1dd8"
#define BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID   "c045fb9c-3447-11ee-be56-0242ac120002"
#define BLUETOOTH_CHARACTERISTIC_COMMAND_UUID       "3804213f-d6ad-458e-977c-4f9f1b9f6b9a"
#define BLUETOOTH_CHARACTERISTIC_LIGHT_COLOR_UUID   "c8d7e25c-3447-11ee-be56-0242ac120002"
#define BLUETOOTH_CHARACTERISTIC_COLOR_READING_UUID "45cfdad3-4f02-4e5c-aa16-1c513dc76a3c"

class BluetoothConnector : public NimBLECharacteristicCallbacks, public NimBLEServerCallbacks {
public:
    BluetoothConnector() {}

    void Setup() {
        NimBLEDevice::init(DEFAULT_WIFI_HOSTNAME);

        NimBLEServer *pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(this);

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

        // Set initial values
        uint8_t zeroSpeed = 0;
        pSpeedChar->setValue(&zeroSpeed, 1);

        uint8_t zeroColor[18] = {0};
        pLightColorChar->setValue(zeroColor, 18);

        uint8_t zeroCommand = 0;
        pCommandChar->setValue(&zeroCommand, 1);

        uint8_t zeroReading[4] = {0};
        pColorReadingChar->setValue(zeroReading, 4);

        pService->start();

        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(BLUETOOTH_SERVICE_MAIN_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->start();

        logger.Log("Bluetooth initialization finished!");
    }

    // Register a callback for command characteristic
    void onCommand(void (*callback)(uint8_t*, size_t)) { commandCallback = callback; }
    void onSpeedChanged(void (*callback)(uint8_t*, size_t)) { speedCallback = callback; }
    void onColorChanged(void (*callback)(uint8_t*, size_t)) { colorCallback = callback; }

    // Set sensor color and notify
    void setSensorColor(uint8_t color[4]) {
        if (pColorReadingChar) {
            pColorReadingChar->setValue(color, 4);
            pColorReadingChar->notify();
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
    void setCommand(uint8_t command) {
        if (pCommandChar) pCommandChar->setValue(&command, 1);
    }

protected:
    // NimBLECharacteristicCallbacks overrides
    void onWrite(NimBLECharacteristic *pCharacteristic) override {
        std::string uuid = pCharacteristic->getUUID().toString();
        std::string value = pCharacteristic->getValue();

        if (uuid == BLUETOOTH_CHARACTERISTIC_COMMAND_UUID && commandCallback) {
            commandCallback((uint8_t*)value.data(), value.length());
        } else if (uuid == BLUETOOTH_CHARACTERISTIC_MOTOR_SPEED_UUID && speedCallback) {
            speedCallback((uint8_t*)value.data(), value.length());
        } else if (uuid == BLUETOOTH_CHARACTERISTIC_LIGHT_COLOR_UUID && colorCallback) {
            colorCallback((uint8_t*)value.data(), value.length());
        }
        // Color reading characteristic is only written by the device itself
    }

    // NimBLEServerCallbacks overrides
    void onConnect(NimBLEServer* pServer) override {
        logger.Log("BLE client connected");
        if (connectionCallback) connectionCallback(BLUETOOTH_CONNECTION_ESTABLISHED);
    }
    void onDisconnect(NimBLEServer* pServer) override {
        logger.Log("BLE client disconnected");
        if (connectionCallback) connectionCallback(BLUETOOTH_CONNECTION_TERMINATED);
        NimBLEDevice::startAdvertising();
    }

public:
    // Register a callback for connection events
    void onConnectionEvent(void (*callback)(uint8_t event)) {
        connectionCallback = callback;
    }

private:
    NimBLECharacteristic *pSpeedChar = nullptr;
    NimBLECharacteristic *pLightColorChar = nullptr;
    NimBLECharacteristic *pCommandChar = nullptr;
    NimBLECharacteristic *pColorReadingChar = nullptr;

    void (*commandCallback)(uint8_t*, size_t) = nullptr;
    void (*speedCallback)(uint8_t*, size_t) = nullptr;
    void (*colorCallback)(uint8_t*, size_t) = nullptr;
    void (*connectionCallback)(uint8_t event) = nullptr;
};

extern BluetoothConnector bluetooth;

#endif