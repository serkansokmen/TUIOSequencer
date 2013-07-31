//
//  Oscillator.h
//  MotionSynth
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"


class Oscillator {
    
public:
    
    int sampleRate;
    float frequency;
    float volume;
    float phase;
    float phaseAdder;
    
    void setup (int sampleRate);
    void setFrequency (float freq);
    void setVolume (float volume);
    
    float getSample();
    
};