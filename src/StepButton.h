//
//  StepButton.h
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"


#define STEP_BUTTON_STATE_OFF       0
#define STEP_BUTTON_STATE_ACTIVE    1
#define STEP_BUTTON_STATE_ON        2


enum StepButtonState {
    off = STEP_BUTTON_STATE_OFF,
    active = STEP_BUTTON_STATE_ACTIVE,
    on = STEP_BUTTON_STATE_ON
};


class StepButton {
    
    ofRectangle     outerBox;
    ofRectangle     innerBox;
    
    StepButtonState state;
    
public:
    void setup(const ofRectangle &boundingBox, int step);
    void draw();
    void toggle(int x, int y);
    
    void setState(StepButtonState s);
    const StepButtonState &getState();
    
    ofRectangle     boundingBox;
    int             step;
};
