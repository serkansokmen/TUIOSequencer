//
//  SegmentsContainer.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ABlob.h"
#include "Segment.h"


using namespace std;


class SegmentsContainer {
    
public:
    void update();
    void draw();
    
    // Creates grid
    void initGrid(const ofRectangle rect, int columns, int rows);
    
    // Check segments against OpenCV blobs
    void checkSegments(const vector<ofxCvBlob> &blobs);
    
    // Check segments against ofxFlob blobs
    void checkSegments(const vector<ABlob*> *blobs);
    
    // Grid segments
    vector<Segment> segments;
};
