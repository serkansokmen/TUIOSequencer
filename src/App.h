//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxOsc.h"
#include "ofxUI.h"
#include "MSABPMTapper.h"
#include "maxiGrains.h"
#include <sys/time.h>

#include "Sequencer.h"
#include "OSCPoint.h"

#define OSC_HOST            "192.168.254.39"
#define OSC_RECEIVE_PORT    8000
#define OSC_SEND_PORT       9000
#define OSC_POINT_COUNT     5


using namespace msa;

typedef hannWinFunctor grainPlayerWin;


class App : public ofBaseApp {
    
public:
    
    App();
    
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void audioRequested(float * input, int bufferSize, int nChannels); /* output method */
	
    float 	*lAudioOut; /* outputs */
	float   *rAudioOut;
	
	int		initialBufferSize; /* buffer size */
	int		sampleRate;
    
    void setupGUIMain();
    void saveGUISettings();
    void loadGUISettings();
    
    void guiEvent(ofxUIEventArgs &e);
    
    Sequencer           *sequencer;
    BPMTapper           bpmTapper;
    int                 lastStep;
    
    // Sounds
    vector<ofSoundPlayer> soundPlayers;
    /*
    vector<maxiSample>   samples;
    
    ofxMaxiMix      mix;
    maxiOsc         timer;
    
    double          compositeSample, outputs[2];
    int             currentCount, lastCount, playHead;
    int             trigger;
     */
    
    // OSC
    ofxOscReceiver      oscReceiver;
    ofxOscSender        oscSender;
    
    float       bpm;
    float       columns;
    float       rows;
    int         totalSteps;
    int         currentStep;
    
    bool        bDebugMode;
    bool        bInitGrid;
    
    //
    vector <OSCPoint>           oscPoints;
    
    // GUI
    vector<ofxUICanvas*>        guis;
	map<string, ofxUICanvas*>   guihash;
};
