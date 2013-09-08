//
//  App.h
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxTSPSReceiver.h"
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
        
        // Sound
        ofDirectory dir;
        dir.listDir(soundPath);
        
        if (dir.size()){
            dir.sort();
            
            for (int i=0; i<dir.size(); i++){
                ofSoundPlayer player;
                player.loadSound(dir.getPath(i));
                players.push_back(player);
            }
        }
    };
    
    string      soundPath;
    float       bpm;
    ofRectangle gridRect;
    ofImage     background;
    
    vector<ofSoundPlayer> players;
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
    
    void currentThemeIdChanged(int &newThemeId);
    void bpmChanged(float &newBpm);
    
    
    ofxTSPS::Receiver tspsReceiver;
    
    // event listeners
    void onPersonEntered(ofxTSPS::EventArgs & tspsEvent);
    void onPersonUpdated(ofxTSPS::EventArgs & tspsEvent);
    void onPersonWillLeave(ofxTSPS::EventArgs & tspsEvent);
    
    // Sequencer
    Sequencer           *currentSequencer;
    vector<Sequencer>   sequencers;
    
    BPMTapper     bpmTapper;
    ofFbo         sequencerFbo;

    ofParameter<float>      bpm;
    
    ofParameter<int>        currentThemeId;
    AppTheme                *currentTheme;
    vector<AppTheme>        themes;
    
    int         lastStep;
    int         columns;
    int         rows;
    int         totalSteps;
    int         currentStep;
};
