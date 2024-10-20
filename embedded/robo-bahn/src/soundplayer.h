/*
  soundplayer.h - Controls the sound player of the train
  Created by Marc Mendler, Oktober 2023
*/
#ifndef soundplayer_h
#define soundplayer_h

#include <Arduino.h>
#include <config.h>

// A note that plays with the specified pitch and length, followed by the specified pause
struct Note {
  int pitch;  
  int duration;
  int pause;
};

static const Note MELODY_DEPARTURE_SIGNAL[2] = {{.pitch = 1440, .duration = 300, .pause = 200}, {.pitch = 1440, .duration = 700, .pause = 0}};

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
      case 1:      
      currentMelody = MELODY_DEPARTURE_SIGNAL;          
      melodyLength = sizeof(MELODY_DEPARTURE_SIGNAL) / sizeof(Note);
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
    //Serial.println("Searching melody!");
    if(currentMelody != NULL) {
      /*Serial.println("Found one!!");

      Serial.println("current Position:");
      Serial.println(currentPosition);

      Serial.println("melody length:");
      Serial.println(melodyLength);*/

      Note currentNote = currentMelody[currentPosition];      

/*      Serial.println("Current note:");
      Serial.println(currentNote.pitch);
      Serial.println(currentNote.duration);
      Serial.println(currentNote.pause);*/
      tone(PIN_SPEAKER, currentNote.pitch, currentNote.duration);
      
      // Wait for the duration of the note + pause
      vTaskDelay(currentNote.duration / portTICK_PERIOD_MS);

      if(currentNote.pause > 0)
        vTaskDelay(currentNote.pause / portTICK_PERIOD_MS);
      
      currentPosition++;
      if(currentPosition >= melodyLength) {
        //Serial.println("Stopping task!");
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