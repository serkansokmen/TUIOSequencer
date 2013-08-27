//
//  Sequencer.h
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxTuio.h"
#include "Track.h"


using namespace std;


class Sequencer {
    
    ofxTuioClient       tuioClient;
    
    ofRectangle         boundingBox;
    
    float               stepButtonWidth;
    float               stepButtonHeight;
    int                 columns;
    int                 rows;
    
    float               aTimer, diffTime;
    int                 bpm;
    int                 currentStep;
    
public:
    
    Sequencer();
    ~Sequencer();
    
    void setup(const ofRectangle rect, int columns, int rows);
    
    void update(int step);
    void draw();
    void reset();
    
    void tuioAdded(ofxTuioCursor &tuioCursor);
	void tuioRemoved(ofxTuioCursor &tuioCursor);
	void tuioUpdated(ofxTuioCursor &tuioCursor);
    
    void refreshCells();
    
    void toggle(int x, int y);
    void randomize(float rate);
    
    const ofRectangle   &getBoundingBox() { return boundingBox; };
    void                setStep(int val)  { currentStep = val; };
    int                 getStep()         { return currentStep; };
    
    vector<Track>               tracks;
    vector<ofxTuioCursor *>     existingCursors;
};
