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
    
    void setupGUI();
    void clearGUI();
    
    void columnsChanged(int &newColumns);
    void rowsChanged(int &newRows);
    void bpmChanged(float &newBpm);
    void randomizeSequencer();
    void resetSequencer();
    
    void load_Pack_1();
    void load_Pack_2();
    
    // Sequencer
    Sequencer     *sequencer;
    BPMTapper     bpmTapper;
    ofFbo         sequencerFbo;
    int           lastStep;
    
    ofParameter<ofVec2f>    sequencerPosition;
    ofParameter<float>      randomizeRate;
    ofParameter<float>      bpm;
    ofParameter<int>        columns;
    ofParameter<int>        rows;
    ofParameter<bool>       bDebugMode;
    ofParameter<bool>       bResetGrid;
    ofParameter<string>     currentPack;
    
    ofxButton               randomizeButton;
    ofxButton               resetButton;
    ofxButton               load_Pack_1_Button;
    ofxButton               load_Pack_2_Button;
    
    ofxPanel                gui;
    bool                    bHideGui;
    
    int         totalSteps;
    int         currentStep;
};
