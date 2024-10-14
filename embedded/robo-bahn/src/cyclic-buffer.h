/*
  cyclic-buffer.h - A buffer that is written to in a cyclic manner. Maximum buffer size is 255 entries.
  Created by Marc Mendler, Oktober 2024
*/

#ifndef cyclic_buffer_h
#define cyclic_buffer_h

#include <Arduino.h>

template <typename T, uint8_t N>
class CyclicBuffer {
private:
    T array[N];
    uint8_t nextWritePosition = 0;

public:
    CyclicBuffer(uint8_t initialValue) {
        for(int i = 0; i < N; i++) {
           array[i] = initialValue;
        }
    }

    // Returns the total capacity of the buffer. This does not mean, that the buffer is completely filled!
    uint8_t getCapacity() const {
        return N;
    }

    // Returns the nth element of the buffer. Element zero is always the oldest Element!
    T& operator[](uint8_t index) {
        uint8_t realIndex = nextWritePosition + index;
        while(realIndex >= N)
          realIndex -= N;
        
        return array[index];
    }

    // Adds a new value to the buffer, possibly overwriting the old entry at the current writing position
    void add(T value) {
      array[nextWritePosition] = value;
      
      nextWritePosition++;
      if(nextWritePosition == N) {
        nextWritePosition = 0;
      }
    }

    void print() {
      String result = "Last Measurements: ";
      
      for(uint8_t i = 0; i < N; i++) {
        result += String(array[i]) + " ";
      }
      
      logger->Log(result);
    }

    /* // TODO: UNTESTED!
    bool matches(T[] comparisonArray, uint8_t length) {
      for(int i = 0; i < length; i++) {        
        shiftedIndex = nextWritePosition + i;
        while(shiftedIndex >= N) {
          shiftedIndex -= N;
        }

        if(comparisonArray[i] != array[shiftedIndex])
          return false;
      }

      return true;
    }*/
};
#endif