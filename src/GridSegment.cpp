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
    colorOn.set(ofColor::hotPink);
    colorOff.set(ofColor::lavender);
    
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