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


class Sequencer {
    
    ofDirectory			soundBank;
    ofRectangle         scannerRect;
    
    float               stepButtonWidth;
    float               stepButtonHeight;
    int                 columns;
    int                 rows;
    
    float               aTimer, rTimer, diffTime;
    int                 bpm;
    int                 step;
    
    vector<string>      soundPaths;
    vector<Track>       tracks;
    
public:
    
    Sequencer();
    ~Sequencer();
    
    void setup(const ofRectangle rect,
               int columnCount=6,
               int rowCount=6);
    
    void update();
    void draw();
    
    void toggle(int x, int y);
    
    void setBPM(int _bpm);
    
    // Check segments against ofxFlob blobs
    void checkSegments(const vector<ABlob*> *blobs);
    
    // Check segments against ofPoint objects
    void checkSegments(const vector<OSCPoint> &points);
};
