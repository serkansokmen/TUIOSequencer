//
//  Segment.h
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCvBlob.h"


#define SEGMENT_STATE_OFF       0
#define SEGMENT_STATE_ACTIVE    1
#define SEGMENT_STATE_ON        2


enum SegmentState {
    off = SEGMENT_STATE_OFF,
    active = SEGMENT_STATE_ACTIVE,
    on = SEGMENT_STATE_ON
};

class GridSegment {
    ofRectangle     outerBox;
    ofRectangle     drawRect;
    SegmentState    state;
public:
    
    void setup(const ofRectangle &rect, int segmentId, float padding);
    void draw();
    
    void setState(SegmentState s);
    
    ofRectangle     boundingBox;
    
    int             segmentId;
};
