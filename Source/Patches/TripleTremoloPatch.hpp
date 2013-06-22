//
//  TrippleTremoloPatch.hpp
//
//  Three independent tremolo LFOs.
//
//  Created by martind on 15/06/2013.
//  http://github.com/dekstop/OwlSim/tree/test/Source/Patches
//
//  Parameter assignment:
//  - A: frequency 1
//  - B: frequency 2
//  - C: frequency 3
//  - D: amount
//  - Push-button:
//
//  TODO:
//  - ...
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

#ifndef __TripleTremoloPatch_hpp__
#define __TripleTremoloPatch_hpp__

#include "StompBox.h"

class TripleTremoloPatch : public Patch {

  const float MIN_FREQ = 0.5;
  const float MAX_FREQ = 35;
  float phase1, phase2, phase3 = 0;

public:
  void processAudio(AudioInputBuffer &input, AudioOutputBuffer &output){
    double rate = getSampleRate();
    float step1 = getLFORate(getRampedParameterValue(PARAMETER_A), rate);
    float step2 = getLFORate(getRampedParameterValue(PARAMETER_B), rate);
    float step3 = getLFORate(getRampedParameterValue(PARAMETER_C), rate);
    float amount = getRampedParameterValue(PARAMETER_D);
    
    int size = input.getSize();
    float* buf = input.getSamples();
    for (int i=0; i<size; ++i)
    {
      float mod1 = sin(phase1) / 2 + .5; // 0..1
      float mod2 = sin(phase2) / 2 + .5; // 0..1
      float mod3 = sin(phase3) / 2 + .5; // 0..1
      float gain1 = (amount * mod1) + (1 - amount);
      float gain2 = (amount * mod2) + (1 - amount);
      float gain3 = (amount * mod3) + (1 - amount);
      buf[i] = (gain1 * gain2 * gain3) * buf[i];
      phase1 += step1;
      phase2 += step2;
      phase3 += step3;
    }
    output.setSamples(buf);
  }

private:
  
  // Returns a step rate, in radians.
  float getLFORate(float p1, float sampleRate){
    float freq1 = p1*p1 * (MAX_FREQ-MIN_FREQ) + MIN_FREQ;
    return freq1 / sampleRate * 2 * M_PI;
  }
  
  // Parameter ramping to reduce clicks.

  float oldVal[4] = {0, 0, 0, 0};
  float ramp = 0.1; // 0..1

  float getRampedParameterValue(PatchParameterId id) {
    float val = getParameterValue(id);
    float result = val * ramp + oldVal[id] * (1-ramp);
    oldVal[id] = val;
    return result;
  }
};

#endif // __TripleTremoloPatch_hpp__
