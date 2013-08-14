//
//  StepButton.cpp
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#include "StepButton.h"


//--------------------------------------------------------------
void StepButton::setup(const ofRectangle &bb){
    
    float padding = 8.0f;
    
    state = off;
    
    boundingBox.set(bb);
    outerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding*.2, boundingBox.getHeight() - padding*.2);
    innerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding, boundingBox.getHeight() - padding);
}

//--------------------------------------------------------------
void StepButton::draw(){
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
            ofSetColor(ofColor::blueViolet, 150);
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
void StepButton::setState(StepButtonState s){
    state = s;
}

//--------------------------------------------------------------
const StepButtonState &StepButton::getState(){
    return state;
}
