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
#include "ofxTonic.h"
#include "ABlob.h"
#include "Track.h"
#include "OSCPoint.h"


using namespace std;
using namespace Tonic;


struct ParameterSlider{
    ControlParameter param;
    bool isActive;
    ParameterSlider(ControlParameter paramArg)
    :param(paramArg),
    isActive(false){}
};


class Sequencer {
    
    ofRectangle         scrubber;
    ofRectangle         boundingBox;
    ofRectangle         sliderArea;
    
    float               stepButtonWidth;
    float               stepButtonHeight;
    int                 columns;
    int                 rows;
    
    float               aTimer, rTimer, diffTime;
    int                 bpm;
    int                 step;
    
    vector<Track>           tracks;
    vector<ParameterSlider> sliders;
    
public:
    
    Sequencer();
    ~Sequencer();
    
    void setup(ofBaseApp *app,
               const ofRectangle rect,
               int columnCount=6,
               int rowCount=6);
    
    void update();
    void draw();
    void mouseDragged(int x, int y);
    
    void toggle(int x, int y);
    void toggleIndex(int i, int j);
    
    void setBPM(int _bpm);
    
    ofxTonicSynth synth;
    
    // Check segments against ofxFlob blobs
    void checkSegments(const vector<ABlob*> *blobs);
    
    // Check segments against ofPoint objects
    void checkSegments(const vector<OSCPoint> &points);
    
    void audioRequested(float * output, int bufferSize, int nChannels);
};
