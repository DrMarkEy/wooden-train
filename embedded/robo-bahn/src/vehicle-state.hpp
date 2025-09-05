#ifndef VEHICLE_STATE_HPP
#define VEHICLE_STATE_HPP

#include <Arduino.h>

#define VEHICLE_STATE_SIZE 4

#define VEHICLE_STATE_OPERATION_MODE 0
#define VEHICLE_STATE_DRIVING_DIRECTION 1
#define VEHICLE_STATE_COLOR_SENSOR_ENABLED 2
#define VEHICLE_STATE_LAST_READ_COLOR 3

#define OPERATION_MODE_STOPPED 1
#define OPERATION_MODE_DRIVING 2
#define OPERATION_MODE_CALLING_AT_STATION 3

#define DRIVING_DIRECTION_FORWARD 0
#define DRIVING_DIRECTION_REVERSE 1


class VehicleState {
    private:
    uint8_t pState[4];
    void (*stateChangedCallback)() = nullptr;

    public:
    VehicleState() {
        pState[VEHICLE_STATE_OPERATION_MODE] = OPERATION_MODE_STOPPED;
        pState[VEHICLE_STATE_DRIVING_DIRECTION] = DRIVING_DIRECTION_FORWARD;
        pState[VEHICLE_STATE_COLOR_SENSOR_ENABLED] = 1;
        pState[VEHICLE_STATE_LAST_READ_COLOR] = 0;
    }

    void setOnStateChangedCallback(void (*callback)()) {
        stateChangedCallback = callback;
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

    void setOperationMode(uint8_t mode) {
        if(pState[VEHICLE_STATE_OPERATION_MODE] != mode) {
            pState[VEHICLE_STATE_OPERATION_MODE] = mode;
            if(stateChangedCallback != nullptr) {
                stateChangedCallback();
            }
        }
    }

    void setDrivingDirection(uint8_t direction) {
        if(pState[VEHICLE_STATE_DRIVING_DIRECTION] != direction) {
            pState[VEHICLE_STATE_DRIVING_DIRECTION] = direction;
            if(stateChangedCallback != nullptr) {
                stateChangedCallback();
            }
        }
    }

    void setColorSensorEnabled(bool enabled) {
        uint8_t val = enabled ? 1 : 0;
        if(pState[VEHICLE_STATE_COLOR_SENSOR_ENABLED] != val) {
            pState[VEHICLE_STATE_COLOR_SENSOR_ENABLED] = val;
            if(stateChangedCallback != nullptr) {
                stateChangedCallback();
            }
        }
    }

    void setLastReadColor(uint8_t color) {
        if(pState[VEHICLE_STATE_LAST_READ_COLOR] != color) {
            pState[VEHICLE_STATE_LAST_READ_COLOR] = color;
            if(stateChangedCallback != nullptr) {
                stateChangedCallback();
            }
        }
    }
};

#endif