//
//  Oscillator.cpp
//  MotionSynth
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "Oscillator.h"

void Oscillator::setup (int rate){
    sampleRate = rate;
}

void Oscillator::setFrequency (float freq){
    frequency = freq;
    phaseAdder = (float)(frequency * TWO_PI) / (float)sampleRate;
}

void Oscillator::setVolume (float vol){
    volume = vol;
}

float Oscillator::getSample(){
    phase += phaseAdder;
    while (phase > TWO_PI) phase -= TWO_PI;
    return sin(phase) * volume;
}