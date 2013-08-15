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
void Track::setup(int id, const ofRectangle &bb, int cols){
    
    trackId = id;
    columns = cols;
    
	float buttonWidth = bb.getWidth() / cols;
    trackHeight = bb.getHeight();
	
    buttons.clear();
    buttons.assign(cols, StepButton());
	for(int i = 0; i<columns; i++){
        buttons[i].setup(ofRectangle(buttonWidth * i, bb.getY(), buttonWidth, trackHeight), i);
	}
}

//--------------------------------------------------------------
void Track::update(int step){
    
    for(int i=0; i<buttons.size(); i++){
        
        if (step == buttons[i].step){
            // Current step column
            if (buttons[i].getState() == active){
                buttons[i].setState(on);
                
                // TODO: set sound something
//                soundPlayer->play();
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
    
    ofPushMatrix();
    ofSetColor(ofColor::white, 100);
    ofTranslate(0, trackHeight * trackId);
    ofPopMatrix();
}

//--------------------------------------------------------------
void Track::toggle(int x, int y){
	for(int i=0; i<buttons.size(); i++){
		buttons[i].toggle(x, y);
	}
}
