/*
  soundplayer.h - Controls the sound player of the train
  Created by Marc Mendler, Oktober 2023
*/
#ifndef soundplayer_h
#define soundplayer_h

#include <Arduino.h>



class SoundPlayer {

  private:

  public:

  SoundPlayer()
  {
   
  }


  void playSound(int sound) 
  {
   
    switch(sound) {
      case 1:
        tone(PIN_SPEAKER, 1440, 300);
        delay(500);  // TODO: Replace delay
        tone(PIN_SPEAKER, 1440, 700);
      break;
    }
  }

  void Loop()
  {
   
  }
};
#endif