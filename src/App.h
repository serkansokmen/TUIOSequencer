//
//  App.h
//  BodySequencerApp
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxTuio.h"
#include "ofxOsc.h"
#include "ofxUI.h"
#include "ofxTweener.h"
#include "MSABPMTapper.h"

#include "Sequencer.h"
#include "OSCPoint.h"

#define OSC_HOST            "192.168.254.39"
#define OSC_RECEIVE_PORT    8000
#define OSC_SEND_PORT       9000
#define OSC_POINT_COUNT     5


using namespace msa;


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
    
    void tuioAdded(ofxTuioCursor &tuioCursor);
	void tuioRemoved(ofxTuioCursor &tuioCursor);
	void tuioUpdated(ofxTuioCursor &tuioCursor);
    
    void setupGUIMain();
    void saveGUISettings();
    void loadGUISettings();
    void guiEvent(ofxUIEventArgs &e);
    
    // Sequencer
    Sequencer           *sequencer;
    BPMTapper           bpmTapper;
    ofFbo               sequencerFbo;
    ofPoint             sequencerPosition;
    int                 lastStep;
    
    bool                bRandomizeSequencer;
    
    // TUIO Client
    ofxTuioClient           tuioClient;
    
    // Sounds
    vector<ofSoundPlayer>   soundPlayers;
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
