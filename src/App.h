//
//  App.h
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxTweener.h"
#include "MSABPMTapper.h"

#include "Sequencer.h"

#define SOUND_BANK_DIR      "soundbank/piano/"


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
    
    void loadSoundBank();
    
    void setupGUIMain();
    void saveGUISettings();
    void loadGUISettings();
    void guiEvent(ofxUIEventArgs &e);
    
    // Sequencer
    Sequencer     *sequencer;
    BPMTapper     bpmTapper;
    ofFbo         sequencerFbo;
    ofPoint       sequencerPosition;
    int           lastStep;
    float         randomizeRate;
    bool          bRandomizeSequencer;
    
    // Sound Players
    vector<ofSoundPlayer> soundPlayers;
    
    float       bpm;
    float       columns;
    float       rows;
    int         totalSteps;
    int         currentStep;
    
    bool        bDebugMode;
    bool        bInitGrid;
    
    // GUI
    vector<ofxUICanvas*>        guis;
	map<string, ofxUICanvas*>   guihash;
};
