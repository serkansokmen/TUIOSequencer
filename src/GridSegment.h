//
//  SegmentRectangle.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 29.07.2013.
//
//
#include "ofMain.h"
#include "ofxCvBlob.h"

#pragma once

class GridSegment {
    
public:
    GridSegment();
    GridSegment(const ofRectangle &r){
        rect.set(r);
        bTouchesBlob = false;
    }
    void update(){
        if (bTouchesBlob){
            color = ofColor(ofColor::blue);
        } else {
            color = ofColor(ofColor::green);
        }
    };
    
    void draw(){
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
    
    ofRectangle rect;
    bool        bTouchesBlob;
    ofColor     color;
};
