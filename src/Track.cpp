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
    
	soundPlayer.loadSound(stepSoundName, true);
    soundPlayer.setSpeed(1.0f);
    soundPlayer.setMultiPlay(true);
    
    float buttonWidth = bb.getWidth() / cols;
    float buttonHeight = bb.getHeight();
	
    buttons.clear();
    buttons.assign(cols, StepButton());
	for(int i = 0; i<cols; i++){
        cout << i << endl;
        buttons[i].setup(ofRectangle(buttonWidth * i, bb.getY(), buttonWidth, buttonHeight), i);
	}
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
void Track::mouseDown(int x, int y){
	for(int i=0; i<buttons.size(); i++){
		buttons[i].mouseDown(x, y);
	}
}
