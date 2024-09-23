/*
  soundplayer.h - Controls the sound player of the train
  Created by Marc Mendler, Oktober 2023
*/
#ifndef soundplayer_h
#define soundplayer_h

#include <Arduino.h>



class SoundPlayer {

  private:
  int nextNote;
  int nextLength;
  long nextTime = 0;

  public:

  SoundPlayer()
  {
   
  }


  void playSound(int sound) 
  {
   
    switch(sound) {
      case 1:         
        tone(PIN_SPEAKER, 1440, 300);
        nextNote = 1440;
        nextLength = 700;
        nextTime = millis() + 500;
      break;
    }
  }

  void Loop()
  {
    if(nextTime != 0 && millis() > nextTime) {
      nextTime = 0;
      tone(PIN_SPEAKER, nextNote, nextLength);
    }
  }
};
#endif