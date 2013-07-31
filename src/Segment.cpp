//
//  Segment.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "Segment.h"


//--------------------------------------------------------------
Segment::Segment(const ofRectangle &r){
    rect.set(r);
    bTouchesBlob = false;
}

//--------------------------------------------------------------
void Segment::update(){
    if (bTouchesBlob){
        color = ofColor(ofColor::blue);
    } else {
        color = ofColor(ofColor::green);
    }
};

//--------------------------------------------------------------
void Segment::draw(){
    ofPushStyle();
    if (bTouchesBlob) {
        ofFill();
        ofSetColor(color, 200);
    } else {
        ofNoFill();
        ofSetColor(color, 50);
    }
    ofRect(rect);
    ofPopStyle();
};