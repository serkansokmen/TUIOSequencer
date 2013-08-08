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

class GridSegment {
    
public:
    
    void setup(const ofRectangle &r, int segId);
    void draw();
    
    ofRectangle     rect;
    bool            bTouchesBlob;
    ofColor         colorOn;
    ofColor         colorOff;
    int             segmentId;
};
