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
    
    ofSoundPlayer           *soundPlayer;
    string                  soundPath;
    float                   trackHeight;
    int                     columns;
    int                     trackId;
    
    vector <StepButton>     buttons;
    
public:
    ~Track();
    
    void setup(int trackId, const ofRectangle &boundingBox, int columns, string stepSoundName);
    void update(int step);
    void draw();
    void toggle(int x, int y);
	
    bool bPlayOnce;
};
