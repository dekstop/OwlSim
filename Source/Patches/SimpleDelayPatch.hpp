//
//  SimpleDelayPatch.hpp
//
//  A basic delay effect.
//
//  Created by martind on 11/06/2013.
//  http://github.com/dekstop/OwlSim/tree/test/Source/Patches
//
//  Parameter assignment:
//  - A: delay time
//  - B: feedback
//  - C:
//  - D: dry/wet mix
//  - Push-button:
//
//  TODO:
//  - clear buffer tail when shortening delay time
//    - OR: always write to full buffer, but stretch it (and adjust read/write speed)
//  - better dry/wet mixing method (don't just fade over)
//

/*
 Copyright (C) 2013 Martin Dittus
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef OwlSim_SimpleDelay_hpp
#define OwlSim_SimpleDelay_hpp

#include "StompBox.h"

class SimpleDelayPatch : public Patch {
  
  const float MIN_DELAY = 0.01; // in seconds
  const float MAX_DELAY = 4;
  
  float* circularBuffer;
  unsigned int bufferSize;
  unsigned int writeIdx;
  
public:
  void processAudio(AudioInputBuffer &input, AudioOutputBuffer &output){

    double rate = getSampleRate();
    
    if (circularBuffer==NULL)
    {
      bufferSize = MAX_DELAY * rate;
      circularBuffer = new float[bufferSize];
      memset(circularBuffer, 0, bufferSize*sizeof(float));
      writeIdx = 0;
    }

    float p1 = getRampedParameterValue(PARAMETER_A);
    float p2 = getRampedParameterValue(PARAMETER_B);
//    float p3 = getRampedParameterValue(PARAMETER_C);
    float p4 = getRampedParameterValue(PARAMETER_D);

    unsigned int sampleDelay = (MIN_DELAY + p1 * (MAX_DELAY-MIN_DELAY)) * rate;
    float feedback = p2;
    // float cutoff = p3;
    float dryWetMix = p4;
    
    int size = input.getSize();
    float* buf = input.getSamples();
    for (int i=0; i<size; ++i)
    {
      float delaySample = circularBuffer[writeIdx];
      circularBuffer[writeIdx] = buf[i] + circularBuffer[writeIdx] * feedback;
      buf[i] =
        delaySample * dryWetMix +
        buf[i] * (1 - dryWetMix);

      writeIdx = (++writeIdx) % sampleDelay;
    }
    output.setSamples(buf);
  }
  
  ~SimpleDelayPatch(){
    delete(circularBuffer);
  }

  
private:
  // Parameter ramping to reduce clicks.
  
  float oldVal[4] = {0, 0, 0, 0};
  float ramp = 0.1; // 0..1
  
  float getRampedParameterValue(PatchParameterId id){
    float val = getParameterValue(id);
    float result = val * ramp + oldVal[id] * (1-ramp);
    oldVal[id] = val;
    return result;
  }
};

#endif // OwlSim_SimpleDelay_hpp
