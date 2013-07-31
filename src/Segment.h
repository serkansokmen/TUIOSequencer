//
//  Segment.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCvBlob.h"

class Segment {
    
public:
    Segment(const ofRectangle &r);
    
    void update();
    void draw();
    
    ofRectangle     rect;
    bool            bTouchesBlob;
    ofColor         color;
    int             segmentId;
};
