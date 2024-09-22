/*
  soundplayer.h - Controls the sound player of the train
  Created by Marc Mendler, Oktober 2023
*/
#ifndef soundplayer_h
#define soundplayer_h

#include <Arduino.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2SNoDAC.h>

#include "config.h"
#include "sample2.h"


class SoundPlayer {

  private:
  AudioOutputI2SNoDAC *out; // TODO: Rather use AudioOutputI2S *out; if the bug with esp32 are ever fixed. Belongs to //out = new AudioOutputI2S(0, 1);
 // See: https://github.com/earlephilhower/ESP8266Audio/issues/551
 // See: https://github.com/earlephilhower/ESP8266Audio/issues/420

  AudioFileSourcePROGMEM *file = nullptr;
  AudioGeneratorWAV *audioGenerator;

  public:

  SoundPlayer()
  {

    // MIGHT BE INTERESTING: MIDI-LIKE SOUND GENERATION
    /*const char  RTTLsound[] PROGMEM = "5thSymph:d=16,o=5,b=130:g,g,g,4d#,4p,f,f,f,4d";
      file = new AudioFileSourcePROGMEM(RTTLsound, strlen_P(RTTLsound));
      flac = new AudioGeneratorRTTTL();
      rtttl->begin(file_progmem, out);*/

    // Configure audio sink
    out = new AudioOutputI2SNoDAC();
    out -> SetPinout(PIN_UNUSED_SPEAKER_ALIBI_A, PIN_UNUSED_SPEAKER_ALIBI_B, PIN_SPEAKER);
    out->SetGain(1.0);    // Specify Volume between 0 and 1.0
    out->SetChannels(1);    // Specify the channel(1) or (2)
    out->SetBitsPerSample(8);    //  Specify bits per sample generally 8 or 16
    out->SetRate(44100);      // Specify the rate here : 8000 ,22050 , 44100 , 48000
    //out->use_mclk = false;//(false);
    
    // Configure audio generator
    audioGenerator = new AudioGeneratorWAV();    
  }


  void playSound(int sound) 
  {
    // Close old sound file
    if(file != nullptr) {
      audioGenerator->stop();
      file->close();      
      delete file;
      file = nullptr;
    }

    switch(sound) {
      case 1:
        
        // Configure audio source        
        file = new AudioFileSourcePROGMEM( SAMPLES_WIN, sizeof(SAMPLES_WIN) );
        audioGenerator->begin(file, out);
      break;
    }
  }

  void Loop()
  {
    if (audioGenerator->isRunning()) {
      if (!audioGenerator->loop()) 
        audioGenerator->stop();
    }
  }
};
#endif