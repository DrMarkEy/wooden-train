/*
  library.h - Contains common library code for multiple modules
  Created by Marc Mendler, Oktober 2023
*/
#ifndef library_h
#define library_h

// TODO: Move buttons.track_button_state here!

//#define SINGLETON(CLASSNAME) CLASSNAME* instance = nullptr;
//#define STORE_SINGLETON if(instance == nullptr) instance = this;

#define EVERY_X_MS(ms, callback) static long next_ ## callback = 0; if(time() > next_ ## callback) { next_ ## callback = time() + ms; callback(); }


#endif