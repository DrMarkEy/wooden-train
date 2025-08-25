/**
 * Commands that can be triggered by the user via app or serial.
 * Created by Marc Mendler, 08.2025
 */
#ifndef user_commands_h
#define user_commands_h

#include <Arduino.h>

#define BLUETOOTH_COMMAND_NONE 0

#define BLUETOOTH_CONNECTION_ESTABLISHED 1
#define BLUETOOTH_CONNECTION_TERMINATED 2

#define BLUETOOTH_COMMAND_START 3
#define BLUETOOTH_COMMAND_STOP 4
#define BLUETOOTH_COMMAND_REVERSE 5
#define BLUETOOTH_COMMAND_WHISTLE 6

#define COMMAND_ANSWER 255

/*#define COMMAND_ERROR_TOO_FEW_PARAMETERS 1
#define COMMAND_ERROR_MODULESLOT_EMPTY 2
#define COMMAND_ERROR_MODULE_NOT_SOLVABLE 3
#define COMMAND_ERROR_MODULE_ALREADY_SOLVED 4
#define COMMAND_ERROR_MODULE_STILL_ARMED 5
#define COMMAND_ERROR_MODULE_NOT_REARMABLE 6
#define COMMAND_ERROR_MODULE_NOT_TRIGGERABLE 7
#define COMMAND_ERROR_INVALID_PARAMETER 8
#define COMMAND_ERROR_INVALID_COMMAND_AT_CURRENT_GAME_STATE 9
#define COMMAND_ERROR_NOT_IMPLEMENTED 10*/

#undef TAG
#define TAG "UserCommand"

class UserCommand {
  private:
  uint8_t id;
  uint8_t *parameters;
  uint8_t parametersLength;

  public:
  UserCommand(uint8_t _id, uint8_t* _parameters, uint8_t _parametersLength): id(_id), parametersLength(_parametersLength) {
    parameters = new uint8_t[_parametersLength];
    memcpy(parameters, _parameters, _parametersLength);
  }

  uint8_t getId() {
    return id;
  }

  uint8_t* getParameters() {
    return parameters;
  }

  uint8_t getParametersLength() {
    return parametersLength;
  }

  uint8_t getByteParameter(uint8_t position) {
    return parameters[position];
  }

  const void print() {
    Serial.printf("Command: %d, parameterLength: %d, parameters: ", id, parametersLength);
    for (int i = 0; i < parametersLength; i++) {
      Serial.printf("%d", parameters[i]);
    }
  }

  ~UserCommand() {
    delete[] parameters;
  }

};

#endif