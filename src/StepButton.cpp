//
//  StepButton.cpp
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#include "StepButton.h"


//--------------------------------------------------------------
void StepButton::setup(const ofRectangle &bb, int s){
    
    float padding = 8.0f;
    
    state = off;
    step = s;
    
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
    
    ofPushMatrix();
    ofTranslate(boundingBox.getTopLeft());
    ofSetColor(ofColor::white);
    ofDrawBitmapString(ofToString(step), 20, 20);
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

//--------------------------------------------------------------
void StepButton::mouseDown(int x, int y){
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
