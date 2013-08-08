//
//  SegmentsContainer.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ABlob.h"
#include "GridSegment.h"
#include "OSCPoint.h"


using namespace std;


enum sequenceDirection{
	SEQ_DIRECTION_HORIZONTAL,
	SEQ_DIRECTION_VERTICAL
};


class Sequencer {
    
    void initTimer();
//    void sendGridStateViaOSC();
    
    sequenceDirection   direction;
    ofPoint             lineStartPos;
    ofPoint             lineEndPos;
    float               rectW;
    float               rectH;
    int                 columns;
    int                 rows;
    
    float               trackSpeed;
    float               startTime;      // store when we start time timer
    float               endTime;        // when we want to stop the timer
    
public:
    
    void setup(const ofRectangle rect, int columnCount, int rowCount,
               float speed=1000, sequenceDirection dir=SEQ_DIRECTION_HORIZONTAL);
    void update();
    void draw();
    
    void setSpeed(float speed);
    
    // Check segments against OpenCV blobs
    void checkSegments(const vector<ofxCvBlob> &blobs);
    
    // Check segments against ofxFlob blobs
    void checkSegments(const vector<ABlob*> *blobs);
    
    // Check segments against ofPoint objects
    void checkSegments(const vector<OSCPoint> &points);
    
    // Toggles segment by coordinate
    void toggleSegment(int x, int y);
    
    // Grid segments
    vector<GridSegment> segments;
};
