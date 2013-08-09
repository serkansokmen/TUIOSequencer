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
    
    float padding = 12.0f;
    drawRect.setFromCenter(rect.getCenter(), rect.getWidth() - padding, rect.getHeight() - padding);
    
    segmentId = segId;
    colorOn.set(ofColor::hotPink);
    colorOff.set(ofColor::gray);
    
    bTouchesBlob = false;
}

//--------------------------------------------------------------
void GridSegment::draw(){
    ofPushStyle();
    ofSetLineWidth(2.0);
    if (bTouchesBlob) {
        ofFill();
        ofSetColor(colorOn);
    } else {
        ofNoFill();
        ofSetColor(colorOff);
    }
    ofRectRounded(drawRect, 8);
    ofPopStyle();
};
