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


class AppTheme {
    
public:
    
    void setup(string soundPath, float bpm, const ofRectangle &rect, string bgPath){
        
        this->soundPath = soundPath;
        this->bpm = bpm;
        
        gridRect.set(rect);
        background.loadImage(bgPath);
    };
    
    string      soundPath;
    float       bpm;
    ofRectangle gridRect;
    ofImage     background;
};



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
    
    void setTheme0();
    void setTheme1();
    void currentThemeIdChanged(int &newThemeId);
    
    void columnsChanged(int &newColumns);
    void rowsChanged(int &newRows);
    void bpmChanged(float &newBpm);
    
//    void randomizeSequencer();
//    void resetSequencer();
    
    // Sequencer
    Sequencer     *sequencer;
    BPMTapper     bpmTapper;
    ofFbo         sequencerFbo;
    int           lastStep;
    
    ofParameter<int>        columns;
    ofParameter<int>        rows;
    ofParameter<float>      bpm;
    ofParameter<bool>       bPlay;
    
    ofParameter<int>        currentThemeId;
    AppTheme                *currentTheme;
    vector<AppTheme>        themes;
    ofxButton               themeButton0, themeButton1;
    
//    ofParameter<float>      randomizeRate;
//    ofxButton               randomizeButton;
//    ofxButton               resetButton;
    
    ofxPanel                gui;
	
    int         totalSteps;
    int         currentStep;
    bool        bHideGui;
};
