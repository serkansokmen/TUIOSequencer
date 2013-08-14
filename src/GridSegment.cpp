//
//  GridSegment.cpp
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#include "GridSegment.h"


//--------------------------------------------------------------
void GridSegment::setup(const ofRectangle &r, int segId, float p=8.0f){

    boundingBox.set(r);
    segmentId = segId;
    state = off;
    
    outerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - p*.2, boundingBox.getHeight() - p*.2);
    drawRect.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - p, boundingBox.getHeight() - p);
}

//--------------------------------------------------------------
void GridSegment::draw(){
    ofPushStyle();
    ofSetLineWidth(1.0);
    cout << state << endl;
    switch (state) {
        case off:
            ofSetColor(ofColor::gray);
            ofNoFill();
            ofRect(outerBox);
            ofSetColor(ofColor::gray, 100);
            ofFill();
            ofRect(drawRect);
            break;
        case active:
            ofSetColor(ofColor::gray);
            ofNoFill();
            ofRect(outerBox);
            ofSetColor(ofColor::blueViolet, 150);
            ofFill();
            ofRect(drawRect);
            break;
        case on:
            ofSetColor(ofColor::white);
            ofNoFill();
            ofRect(outerBox);
            ofSetColor(ofColor::blueViolet);
            ofFill();
            ofRect(drawRect);
            break;
            
        default:
            break;
    }
    
    ofPopStyle();
};

//--------------------------------------------------------------
void GridSegment::setState(SegmentState s){
    state = s;
}

//--------------------------------------------------------------
const SegmentState &GridSegment::getState(){
    return state;
}
