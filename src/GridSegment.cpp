//
//  GridSegment.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "GridSegment.h"

//--------------------------------------------------------------
void GridSegment::setup(const ofRectangle &r, int segId){
    rect.set(r);
    
    segmentId = segId;
    frequency = 100.0f + 155.0f * segId;
    colorOn.set(100, frequency, 100);
    colorOff.set(ofColor::green);
    
    bTouchesBlob = false;
}

//--------------------------------------------------------------
void GridSegment::draw(){
    ofPushStyle();
    if (bTouchesBlob) {
        ofFill();
        ofSetColor(colorOn);
    } else {
        ofNoFill();
        ofSetColor(colorOff);
    }
    ofRect(rect);
    ofPopStyle();
};