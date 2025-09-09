/*
  soundplayer.h - Controls the sound player of the train
  Created by Marc Mendler, Oktober 2023
*/
#ifndef soundplayer_h
#define soundplayer_h

#include <Arduino.h>
#include <config.h>

#define SOUND_STARTUP 1
#define SOUND_WHISTLE 2
#define SOUND_ACCIDENT 3

// A note that plays with the specified pitch and length, followed by the specified pause
struct Note {
  int pitch;
  int duration;
  int pause;
};

static const Note MELODY_STARTUP[3] = {{.pitch = 1440, .duration = 120, .pause = 100}, {.pitch = 1440, .duration = 120, .pause = 100}, {.pitch = 1440, .duration = 120, .pause = 0}};
static const Note MELODY_WHISTLE[2] = {{.pitch = 1440, .duration = 300, .pause = 200}, {.pitch = 1440, .duration = 700, .pause = 0}};
static const Note MELODY_ACCIDENT[1] = {{.pitch = 1440, .duration = 1500, .pause = 0}};

static void SoundTaskFunction (void* parameter);

class SoundPlayer {

  private:
  const Note* currentMelody = NULL;
  uint8_t currentPosition = 0;
  uint8_t melodyLength = 0;

  public:

  SoundPlayer()
  {

  }

  void playSound(int sound)
  {
    currentPosition = 0;

    switch(sound) {
      case SOUND_STARTUP:
      currentMelody = MELODY_STARTUP;
      melodyLength = sizeof(MELODY_STARTUP) / sizeof(Note);
      break;

      case SOUND_WHISTLE:
      currentMelody = MELODY_WHISTLE;
      melodyLength = sizeof(MELODY_WHISTLE) / sizeof(Note);
      break;

      case SOUND_ACCIDENT:
      currentMelody = MELODY_ACCIDENT;
      melodyLength = sizeof(MELODY_ACCIDENT) / sizeof(Note);
      break;
    }

    xTaskCreatePinnedToCore(
      SoundTaskFunction,
      "Play sound",
      1500,
      NULL,//( void * ) &buttonController,
      1, // Priority
      NULL,
      1 // Core affinity
    );
  }

  bool continueMelody()
  {
    //logger.Log("Searching melody!");
    if(currentMelody != NULL) {
      /*logger.Log("Found one!!");

      logger.Log("current Position:");
      logger.Log(currentPosition);

      logger.Log("melody length:");
      logger.Log(melodyLength);*/

      Note currentNote = currentMelody[currentPosition];

/*      logger.Log("Current note:");
      logger.Log(currentNote.pitch);
      logger.Log(currentNote.duration);
      logger.Log(currentNote.pause);*/
      tone(PIN_SPEAKER, currentNote.pitch, currentNote.duration);

      // Wait for the duration of the note + pause
      vTaskDelay(currentNote.duration / portTICK_PERIOD_MS);

      if(currentNote.pause > 0)
        vTaskDelay(currentNote.pause / portTICK_PERIOD_MS);

      currentPosition++;
      if(currentPosition >= melodyLength) {
        //logger.Log("Stopping task!");
        currentMelody = NULL;
        return false;
      }

      return true;
    }

    return false; // Should normally never happen...
  }
} extern soundPlayer;

static void SoundTaskFunction (void* parameter) {
  while(1) {
    if(!soundPlayer.continueMelody()) {
      vTaskDelete(NULL);
    }
  }
}

#endif