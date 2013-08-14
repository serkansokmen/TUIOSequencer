//
//  Track.h
//  MotionSynth
//
//  Created by Serkan Sokmen on 14.08.2013.
//
//

#pragma once

#include "ofMain.h"
#include "StepButton.h"


class Track {
    
public:
    ~Track();
    
    void setup(string stepSoundName, const ofRectangle &boundingBox, int columns);
    void update(int _step);
    void draw();
	
    vector <StepButton>     buttons;
    ofSoundPlayer			soundPlayer;
};
