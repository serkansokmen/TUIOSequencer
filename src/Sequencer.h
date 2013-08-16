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
#include "Track.h"
#include "OSCPoint.h"


using namespace std;


class Sequencer {
    
    ofRectangle         scrubber;
    ofRectangle         boundingBox;
    
    float               stepButtonWidth;
    float               stepButtonHeight;
    int                 columns;
    int                 rows;
    
    float               aTimer, rTimer, diffTime;
    int                 bpm;
    int                 step;
    
    vector<Track>       tracks;
    
public:
    
    ~Sequencer();
    
    void setup(const ofRectangle rect,
               int columnCount=8,
               int rowCount=8);
    
    void update();
    void draw();
    
    void stepEvent(float& val);
    
    void reset();
    
    void toggle(int x, int y);
    void toggleIndex(int i, int j);
    
    // Check segments against ofxFlob blobs
    void checkSegments(const vector<ABlob*> *blobs);
    
    // Check segments against ofPoint objects
    void checkSegments(const vector<OSCPoint> &points);
    
    void audioRequested(float * output, int bufferSize, int nChannels);
};
