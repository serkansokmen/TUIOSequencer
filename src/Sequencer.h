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
#include "ofxTweener.h"
#include "ABlob.h"
#include "Track.h"
#include "OSCPoint.h"


using namespace std;


enum sequenceDirection{
	SEQ_DIRECTION_HORIZONTAL,
	SEQ_DIRECTION_VERTICAL
};


class Sequencer {
    
    void                startScan(bool fromBeginning);
    
    sequenceDirection   direction;
    
    ofPoint             lineStartPos;
    ofPoint             lineEndPos;
    
    float               stepButtonWidth;
    float               stepButtonHeight;
    float               trackSpeed;
    int                 columns;
    int                 rows;
    
public:
    
    ~Sequencer();
    
    void setup(const ofRectangle rect, int columnCount, int rowCount,
               float speed=1000, sequenceDirection dir=SEQ_DIRECTION_HORIZONTAL);
    void update();
    void draw();
    
    void setSpeed(float speed);
    
    // Check segments against ofxFlob blobs
    void checkSegments(const vector<ABlob*> *blobs);
    
    // Check segments against ofPoint objects
    void checkSegments(const vector<OSCPoint> &points);
    
    // Tracks
    vector<Track> tracks;
};
