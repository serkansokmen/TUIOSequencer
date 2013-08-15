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
    
    void setup(const ofRectangle &boundingBox, int columns, string stepSoundName);
    void update(int step);
    void draw();
    void mouseDown(int x, int y);
	
    vector <StepButton>     buttons;
    ofSoundPlayer			soundPlayer;
};
