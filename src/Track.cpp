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
	for(int i =0; i<cols; i++){
        buttons[i].setup(ofRectangle(buttonWidth * i, bb.getY(), buttonWidth, buttonHeight));
	}
}

//--------------------------------------------------------------
void Track::update(){
	for(int i=0; i<buttons.size(); i++){
        
        if (buttons[i].getState() == on){
            soundPlayer.play();
        }
//		if(stepB[i].bActive == true && _step == i && bPlayOnce){
//			if(bPlayOnce){
//				stepSound.setSpeed(1.0f);
//				stepSound.play();
//				bPlayOnce = false;
//			}
//			
//		}
	}
}

//--------------------------------------------------------------
void Track::draw(){
	for(int i=0; i<buttons.size(); i++){
		buttons[i].draw();
	}
}
