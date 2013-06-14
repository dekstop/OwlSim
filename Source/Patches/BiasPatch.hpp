//
//  BiasPatch.hpp
//
//  Adjust amplitude bias while retaining high peaks.
//  This works best on normalised signals.
//
//  Created by martind on 14/06/2013.
//  http://github.com/dekstop/OwlSim
//
//  Parameter assignment:
//  - A: bias: low..high
//  - B:
//  - C:
//  - D: dry/wet mix
//  - Push-button:
//
//  TODO:
//  - adjust to varying signal amplitude (don't just kill quiet signals at low bias values)
//  - ...
//

#ifndef __BiasPatch_hpp__
#define __BiasPatch_hpp__

#include "StompBox.h"

class BiasPatch : public Patch {

  const float MIN_BIAS = 0.1;
  const float MED_BIAS = 1;
  const float MAX_BIAS = 6;

public:
  void processAudio(AudioInputBuffer &input, AudioOutputBuffer &output){
    float bias = getBias(1 - getRampedParameterValue(PARAMETER_A));
    float dryWetMix = getRampedParameterValue(PARAMETER_D);

    int size = input.getSize();
    float* buf = input.getSamples();
    for (int i=0; i<size; ++i)
    {
      float v =
      powf(fabs(buf[i]), bias) * // bias
        (buf[i] < 0 ? -1 : 1);    // sign
      buf[i] =
        v * dryWetMix +
        buf[i] * (1 - dryWetMix);
    }
    output.setSamples(buf);
  }

private:

  // Mapping p1 parameter ranges so that:
  // - full-left (0) is "low bias"
  // - centre (0.5) is "no bias"
  // - full-right (1.0) is "high bias"
  float getBias(float p1){
    if (p1 < 0.5)
    { // min .. med
      p1 = p1 * 2; // [0..1] range
      return p1*p1 * (MED_BIAS-MIN_BIAS) + MIN_BIAS;
    } else
    { // med .. max
      p1 = (p1 - 0.5) * 2; // [0..1] range
      return p1*p1 * (MAX_BIAS-MED_BIAS) + MED_BIAS;
    }
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

#endif // __BiasPatch_hpp__
