//
//  TrackCell.cpp
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#include "TrackCell.h"


//--------------------------------------------------------------
void TrackCell::setup(const ofRectangle &bb, int s){
    
    float padding = 8.0f;
    
    state = off;
    step = s;
    
    boundingBox.set(bb);
    outerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding*.2, boundingBox.getHeight() - padding*.2);
    innerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding, boundingBox.getHeight() - padding);
}

//--------------------------------------------------------------
void TrackCell::draw(){
    ofPushMatrix();
    ofPushStyle();
    ofSetLineWidth(1.0);
    
    switch (state) {
        case off:
            ofSetColor(ofColor::gray);
            ofNoFill();
            ofRect(outerBox);
            ofSetColor(ofColor::gray, 100);
            ofFill();
            ofRect(innerBox);
            break;
        case active:
            ofSetColor(ofColor::gray);
            ofNoFill();
            ofRect(outerBox);
            ofSetColor(ofColor::blueViolet, 100);
            ofFill();
            ofRect(innerBox);
            break;
        case on:
            ofSetColor(ofColor::white);
            ofNoFill();
            ofRect(outerBox);
            ofSetColor(ofColor::blueViolet);
            ofFill();
            ofRect(innerBox);
            break;
            
        default:
            break;
    }
    ofPopStyle();
    ofPopMatrix();
};

//--------------------------------------------------------------
void TrackCell::setState(TrackCellState s){
    state = s;
}

//--------------------------------------------------------------
const TrackCellState &TrackCell::getState(){
    return state;
}

//--------------------------------------------------------------
void TrackCell::toggle(int x, int y){
    if (boundingBox.inside(x, y)){
        switch (getState()) {
            case off:
                setState(active);
                break;
            case on:
            case active:
                setState(off);
                break;
        }
    }
}
