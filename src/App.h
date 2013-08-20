//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "MSABPMTapper.h"
#include "ofxUI.h"
#include "Sequencer.h"
#include "OSCPoint.h"

#define OSC_HOST            "192.168.254.39"
#define OSC_RECEIVE_PORT    8000
#define OSC_SEND_PORT       9000
#define OSC_POINT_COUNT     5

#define COLUMNS             4
#define ROWS                4


using namespace msa;


class App : public ofBaseApp {
    
public:
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
    
    void audioRequested(float * output, int bufferSize, int nChannels);
    
    void setupGUIMain();
    void saveGUISettings();
    void loadGUISettings();
    
    void guiEvent(ofxUIEventArgs &e);
    
    Sequencer           *sequencer;
    BPMTapper           bpmTapper;
    
    ofRectangle         scanRect;
    
    // OSC
    ofxOscReceiver      oscReceiver;
    ofxOscSender        oscSender;
    
    float       bpm;
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
