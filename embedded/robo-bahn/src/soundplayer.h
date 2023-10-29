/*
  soundplayer.h - Controls the sound player of the train
  Created by Marc Mendler, Oktober 2023
*/
#ifndef soundplayer_h
#define soundplayer_h

#include <Arduino.h>
//#include <AudioOutputI2S.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorFLAC.h>
#include <AudioGeneratorWAV.h>
#include "AudioOutputI2SNoDAC.h"

#include "sample2.h"

AudioOutputI2SNoDAC *out;
//AudioOutputI2S *out;
AudioFileSourcePROGMEM *file;
AudioGeneratorWAV *flac;

#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_12 // BCLK
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_5 // WSEL
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_25


class SoundPlayer {

  //private:

  public:

  SoundPlayer()
  {

/*      const char  RTTLsound[] PROGMEM = "5thSymph:d=16,o=5,b=130:g,g,g,4d#,4p,f,f,f,4d";
      file = new AudioFileSourcePROGMEM(RTTLsound, strlen_P(RTTLsound));
      flac = new AudioGeneratorRTTTL();
      rtttl->begin(file_progmem, out);*/

out = new AudioOutputI2SNoDAC();
out -> SetPinout(I2S_SPEAKER_SERIAL_CLOCK, I2S_SPEAKER_LEFT_RIGHT_CLOCK, I2S_SPEAKER_SERIAL_DATA);


    file = new AudioFileSourcePROGMEM( SAMPLES_WIN, sizeof(SAMPLES_WIN) );
    //out = new AudioOutputI2S(0, 1);
    flac = new AudioGeneratorWAV();
    flac->begin(file, out);

    

    //out->SetGain(1.0);    // Specify Volume between 0 and 1.0
    //out->SetChannels(1);    // Specify the channel(1) or (2)
   // out->SetBitsPerSample(8);    //  Specify bits per sample generally 8 or 16
    //out->SetRate(44100);      // Specify the rate here : 8000 ,22050 , 44100 , 48000
    //out->use_mclk = false;//(false);

//    ledcAttachPin(25, 0);
//    ledcWriteNote(0, NOTE_C, 4);

  }

  void Loop()
  {
    if (flac->isRunning()) {
      if (!flac->loop()) 
        flac->stop();
    } /*else {
      //Serial.printf("FLAC done\n");
      //delay(1000);
    }*/
  }
};
#endif