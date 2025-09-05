#ifndef VEHICLE_STATE_HPP
#define VEHICLE_STATE_HPP

#include <Arduino.h>
#include <vector>

#define VEHICLE_STATE_SIZE 4

#define VEHICLE_STATE_OPERATION_MODE 0
#define VEHICLE_STATE_DRIVING_DIRECTION 1
#define VEHICLE_STATE_COLOR_SENSOR_ENABLED 2
#define VEHICLE_STATE_LAST_READ_COLOR 3

#define VEHICLE_STATE_ENGINE_SPEED 4 // Not stored in pState array

#define OPERATION_MODE_STOPPED 1
#define OPERATION_MODE_DRIVING 2
#define OPERATION_MODE_CALLING_AT_STATION 3

#define DRIVING_DIRECTION_FORWARD 0
#define DRIVING_DIRECTION_REVERSE 1


class VehicleState {
    private:
    uint8_t pState[5]; // Readonly (for web client) state variables
    std::vector<void (*)(uint8_t changedState)> stateChangedCallbacks; // Store multiple callbacks

    uint8_t engineSpeed; // Writeable state (for web client)

    public:
    VehicleState() {
        pState[VEHICLE_STATE_OPERATION_MODE] = OPERATION_MODE_STOPPED;
        pState[VEHICLE_STATE_DRIVING_DIRECTION] = DRIVING_DIRECTION_FORWARD;
        pState[VEHICLE_STATE_COLOR_SENSOR_ENABLED] = 1;
        pState[VEHICLE_STATE_LAST_READ_COLOR] = 0;
    }

    void addOnStateChangedCallback(void (*callback)(uint8_t changedState)) {
        stateChangedCallbacks.push_back(callback);
    }

    const uint8_t* getState() {
        return pState;
    }

    uint8_t getOperationMode() {
        return pState[VEHICLE_STATE_OPERATION_MODE];
    }

    uint8_t getDrivingDirection() {
        return pState[VEHICLE_STATE_DRIVING_DIRECTION];
    }

    bool isColorSensorEnabled() {
        return pState[VEHICLE_STATE_COLOR_SENSOR_ENABLED] == 1;
    }

    uint8_t getLastReadColor() {
        return pState[VEHICLE_STATE_LAST_READ_COLOR];
    }

    uint8_t getEngineSpeed() {
        return engineSpeed;
    }

    void notifyStateChanged(uint8_t changedState) {
        for (auto cb : stateChangedCallbacks) {
            if (cb != nullptr) cb(changedState);
        }
    }

    void setOperationMode(uint8_t mode) {
        if(pState[VEHICLE_STATE_OPERATION_MODE] != mode) {
            pState[VEHICLE_STATE_OPERATION_MODE] = mode;
            notifyStateChanged(VEHICLE_STATE_OPERATION_MODE);
        }
    }

    void setDrivingDirection(uint8_t direction) {
        if(pState[VEHICLE_STATE_DRIVING_DIRECTION] != direction) {
            pState[VEHICLE_STATE_DRIVING_DIRECTION] = direction;
            notifyStateChanged(VEHICLE_STATE_DRIVING_DIRECTION);
        }
    }

    void setColorSensorEnabled(bool enabled) {
        uint8_t val = enabled ? 1 : 0;
        if(pState[VEHICLE_STATE_COLOR_SENSOR_ENABLED] != val) {
            pState[VEHICLE_STATE_COLOR_SENSOR_ENABLED] = val;
            notifyStateChanged(VEHICLE_STATE_COLOR_SENSOR_ENABLED);
        }
    }

    void setLastReadColor(uint8_t color) {
        if(pState[VEHICLE_STATE_LAST_READ_COLOR] != color) {
            pState[VEHICLE_STATE_LAST_READ_COLOR] = color;
            notifyStateChanged(VEHICLE_STATE_LAST_READ_COLOR);
        }
    }

    void setEngineSpeed(uint8_t speed) {
        if(engineSpeed != speed) {
            engineSpeed = speed;
            notifyStateChanged(VEHICLE_STATE_ENGINE_SPEED);
        }
    }
};

#endif