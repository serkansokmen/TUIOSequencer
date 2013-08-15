//
//  Track.cpp
//  MotionSynth
//
//  Created by Serkan Sokmen on 14.08.2013.
//
//

#include "Track.h"


//--------------------------------------------------------------
Track::~Track(){
    buttons.clear();
}


//--------------------------------------------------------------
void Track::setup(const ofRectangle &bb, int cols, string stepSoundName){
    
	float buttonWidth = bb.getWidth() / cols;
    float buttonHeight = bb.getHeight();
	
    buttons.clear();
    buttons.assign(cols, StepButton());
	for(int i = 0; i<cols; i++){
        buttons[i].setup(ofRectangle(buttonWidth * i, bb.getY(), buttonWidth, buttonHeight), i);
	}
    
    soundPlayer.loadSound(stepSoundName);
}

//--------------------------------------------------------------
void Track::update(int step){
    
    for(int i=0; i<buttons.size(); i++){
        
        if (step == buttons[i].step){
            // Current step column
            if (buttons[i].getState() == active){
                buttons[i].setState(on);
                soundPlayer.play();
            }
        } else {
            if (buttons[i].getState() == on){
                buttons[i].setState(active);
            }
        }
    }
}

//--------------------------------------------------------------
void Track::draw(){
	for(int i=0; i<buttons.size(); i++){
		buttons[i].draw();
	}
}

//--------------------------------------------------------------
void Track::toggle(int x, int y){
	for(int i=0; i<buttons.size(); i++){
		buttons[i].toggle(x, y);
	}
}
