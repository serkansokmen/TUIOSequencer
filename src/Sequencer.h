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
    
    vector<Track>       tracks;
    
public:
    
    Sequencer();
    ~Sequencer();
    
    void setup(const ofRectangle rect,
               int columnCount=8,
               int rowCount=8);
    
    void update(int step);
    void draw();
    
    void stepEvent(float& val);
    
    void reset();
    
    void setStep(int val);
    
    void toggle(int x, int y);
    void toggleIndex(int i, int j);
    
    void checkSegments(const vector<OSCPoint> &points);
};
