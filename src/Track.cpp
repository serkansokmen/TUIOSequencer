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
    
    cells.clear();
    cellStates.clear();
    
}


//--------------------------------------------------------------
void Track::setup(int id, const ofRectangle &bb, int cols){
    
    trackId = id;
    columns = cols;
    
	float buttonWidth = bb.getWidth() / cols;
    trackHeight = bb.getHeight();
	
    cells.assign(cols, TrackCell());
	for(int i = 0; i<columns; i++){
        cells[i].setup(ofRectangle(buttonWidth * i, bb.getY(), buttonWidth, trackHeight), i);
	}
    
    cellStates.assign(columns, bool(false));
    bIsReady = true;
}

//--------------------------------------------------------------
void Track::update(int step){
    
    for (int i=0; i<cells.size(); i++){
        
        if (step == cells[i].step){
            // Current step column
            if (cells[i].getState() == active){
                cells[i].setState(on);
            }
        } else {
            if (cells[i].getState() == on){
                cells[i].setState(active);
            }
        }
    }
    
    for (int i=0; i<cellStates.size(); i++) {
        cellStates[i] = (cells[i].getState() != off);
    }
}

//--------------------------------------------------------------
void Track::draw(){
	for(int i=0; i<cells.size(); i++){
		cells[i].draw();
	}
    
    ofPushMatrix();
    ofSetColor(ofColor::white, 100);
    ofTranslate(0, trackHeight * trackId);
    ofPopMatrix();
}

//--------------------------------------------------------------
void Track::toggle(int x, int y){
	for(int i=0; i<cells.size(); i++){
		cells[i].toggle(x, y);
	}
}
