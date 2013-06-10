#ifndef __DualTremoloPatch_hpp__
#define __DualTremoloPatch_hpp__

#include "StompBox.h"

class DualTremoloPatch : public Patch {

    const float MIN_FREQ = 0.5;
    const float MAX_FREQ = 35;
    float phase1, phase2 = 0;

public:
    void processAudio(AudioInputBuffer &input, AudioOutputBuffer &output){
        double rate = getSampleRate();

        float p1 = getRampedParameterValue(PARAMETER_A);
        float freq1 = p1*p1 * (MAX_FREQ-MIN_FREQ) + MIN_FREQ;
        double step1 = freq1 / rate;
        float amt1 = getRampedParameterValue(PARAMETER_B);

        float p2 = getRampedParameterValue(PARAMETER_C);
        float freq2 = p2*p2 * (MAX_FREQ-MIN_FREQ) + MIN_FREQ;
        float amt2 = getRampedParameterValue(PARAMETER_D);
        double step2 = freq2 / rate;
        
        int size = input.getSize();
        float* buf = input.getSamples();
        for (int i=0; i<size; ++i)
        {
            float mod1 = sin(2 * M_PI * phase1) / 2 + .5; // 0..1
            float mod2 = sin(2 * M_PI * phase2) / 2 + .5; // 0..1
            float gain1 = (amt1 * mod1) + (1 - amt1);
            float gain2 = (amt2 * mod2) + (1 - amt2);
            buf[i] = (gain1 * gain2) * buf[i];
            phase1 += step1;
            phase2 += step2;
        }
        output.setSamples(buf);
    }

private:
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

#endif // __DualTremoloPatch_hpp__