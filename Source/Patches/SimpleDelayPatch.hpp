//
//  SimpleDelay.hpp
//  OwlSim
//
//  Created by mongo on 11/06/2013.
//
// TODO:
// - clear buffer tail when shortening delay time
//   - OR: always write to full buffer, but stretch it (and adjust read/write speed)
// - better dry/wet mixing method (don't just fade over)
//

#ifndef OwlSim_SimpleDelay_hpp
#define OwlSim_SimpleDelay_hpp

#include "StompBox.h"

class SimpleDelayPatch : public Patch {
    
    const float MIN_DELAY = 0.01; // in seconds
    const float MAX_DELAY = 8;
    
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
            writeIdx = 0;
        }

        float p1 = getRampedParameterValue(PARAMETER_A);
        float p2 = getRampedParameterValue(PARAMETER_B);
//        float p3 = getRampedParameterValue(PARAMETER_C);
        float p4 = getRampedParameterValue(PARAMETER_D);

        unsigned int sampleDelay = (MIN_DELAY + p1 * (MAX_DELAY-MIN_DELAY)) * rate;
        float feedback = p2;
        // float cutoff = p3;
        float dryWetMix = p4;
        
        int size = input.getSize();
        float* buf = input.getSamples();
        int readIdx = writeIdx - sampleDelay;
        while (readIdx<0)
            readIdx += bufferSize;
        for (int i=0; i<size; ++i)
        {
            circularBuffer[writeIdx] =
                circularBuffer[writeIdx] * feedback +
                buf[i] * (1 - feedback);
            buf[i] =
                circularBuffer[readIdx] * dryWetMix +
                buf[i] * (1 - dryWetMix);

            writeIdx = (++writeIdx) % sampleDelay;
            readIdx = (++readIdx) % sampleDelay;
        }
        output.setSamples(buf);
    }
    
    ~SimpleDelayPatch(){
        free(circularBuffer);
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
