//
//  App.cpp
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 29.07.2013.
//
//

#include "App.h"


#pragma mark App
App::App(){
    sequencer = new Sequencer();
}


//--------------------------------------------------------------
void App::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofSetLogLevel(OF_LOG_WARNING);
    
    ofSetWindowTitle("TUIO Sequencer");
    ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())*.5, (ofGetScreenHeight()-ofGetHeight())*.5);
    
    // Setup Tweener
    Tweener.setMode(TWEENMODE_OVERRIDE);
    
    // Scan rect
    float w = 640;
    float h = 480;
    ofRectangle scanRect(0, 0, w, h);
    
    // Setup GUIs
    setupGUI();
    // Load existing values
    gui.loadFromFile("settings.xml");
    
    // Initialize Sequencer
    sequencer->setup(scanRect, columns, rows);
    // Allocate draw FBO
    sequencerFbo.allocate(scanRect.getWidth(), scanRect.getHeight());
    
    // Set initial values
    bHideGui = false;
    
    // Initialize Sound Bank
    loadSoundBank();
    
    totalSteps = columns;
    currentStep = 0;
    lastStep = 0;
}

//--------------------------------------------------------------
void App::update(){
    
    // Update sound
    ofSoundUpdate();
    
    // Update Tweener
    Tweener.update();
    
    // Update bpm
    bpmTapper.update();
    currentStep = (int)bpmTapper.beatTime() % totalSteps;
    if (currentStep > totalSteps){
        currentStep = 0;
    }
    
    // Update sequencer
    sequencer->update(currentStep);
    
    // Check on/off states
    for (int i=0; i<sequencer->tracks.size(); i++) {
        Track *track = &sequencer->tracks[i];
        if (track->cellStates[currentStep] > 0 && lastStep != currentStep){
            soundPlayers[i].play();
        }
    }
    lastStep = currentStep;
    
    sequencerFbo.begin();
    ofClear(255);
    sequencer->draw();
    sequencerFbo.end();
}

//--------------------------------------------------------------
void App::draw(){
    
    ofBackground(ofColor::black);
    
    // Draw scan rect
    ofPushMatrix();
    ofTranslate(sequencer->getBoundingBox().getPosition());
    sequencerFbo.draw(sequencerPosition->x, sequencerPosition->y);
    ofPopMatrix();
    
    if (!bHideGui) {
        gui.draw();
    }
}

//--------------------------------------------------------------
void App::keyPressed(int key){
    
    switch (key)
    {
        case 'd':
            bHideGui = !bHideGui;
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void App::keyReleased(int key){

}

//--------------------------------------------------------------
void App::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void App::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void App::mousePressed(int x, int y, int button){
    ofRectangle *currentRect = new ofRectangle;
    currentRect->set(sequencer->getBoundingBox());
    currentRect->setPosition(sequencerPosition->x, sequencerPosition->y);
    
    if (currentRect->inside(x, y)){
        float rx = x - sequencerPosition->x;
        float ry = y - sequencerPosition->y;
        sequencer->toggle(rx, ry);
    }
    
    currentRect = 0;
    delete currentRect;
}

//--------------------------------------------------------------
void App::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void App::windowResized(int w, int h){
//    sequencerPosition->x = (w - sequencer->getBoundingBox().getWidth()) * .5;
//    sequencerPosition->y = (h - sequencer->getBoundingBox().getHeight()) * .5;
}

//--------------------------------------------------------------
void App::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void App::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void App::exit(){
    
    gui.saveToFile("settings.xml");
    
    soundPlayers.clear();
    
    sequencer = 0;
    delete sequencer;
}

#pragma mark - GUI
//--------------------------------------------------------------
void App::setupGUI(){
    gui.setup("Settings");
    gui.add(columns.set("Columns", 8, 4, 20));
    gui.add(rows.set("Rows", 8, 4, 20));
    
    float diffW = ofGetWidth() - sequencer->getBoundingBox().getWidth();
    float diffH = ofGetHeight() - sequencer->getBoundingBox().getHeight();
    gui.add(sequencerPosition.set("Sequencer Position",
                                  ofVec2f(diffW * .5, diffH * .5),
                                  ofVec2f(0,0),
                                  ofVec2f(diffW, diffH)));
    
    gui.add(bpm.set("BPM", 100.0f, 32.0f, 240.0f));
    
    gui.add(randomizeRate.set("Randomize Rate", 1, 1, 100));
    gui.add(randomizeButton.setup("Randomize"));
    gui.add(resetButton.setup("Reset"));
    
    columns.addListener(this, &App::columnsChanged);
    rows.addListener(this, &App::rowsChanged);
    bpm.addListener(this, &App::bpmChanged);
    randomizeButton.addListener(this, &App::randomizeSequencer);
    resetButton.addListener(this, &App::resetSequencer);
}

//--------------------------------------------------------------
void App::columnsChanged(int &newColumns){
    loadSoundBank();
    sequencer->setup(ofRectangle(0, 0, sequencer->getBoundingBox().getWidth(), sequencer->getBoundingBox().getHeight()), columns, rows);
    totalSteps = columns;
}

//--------------------------------------------------------------
void App::rowsChanged(int &newRows){
    loadSoundBank();
    sequencer->setup(ofRectangle(0, 0, sequencer->getBoundingBox().getWidth(), sequencer->getBoundingBox().getHeight()), columns, rows);
}

//--------------------------------------------------------------
void App::bpmChanged(float &newBpm){
    bpmTapper.setBpm(newBpm);
}

//--------------------------------------------------------------
void App::randomizeSequencer(){
    sequencer->reset();
    sequencer->randomize(randomizeRate);
}

//--------------------------------------------------------------
void App::resetSequencer(){
    sequencer->reset();
    sequencer->setup(sequencer->getBoundingBox(), columns, rows);
    bpmTapper.startFresh();
}

#pragma mark - Sound
//--------------------------------------------------------------
void App::loadSoundBank(){
    
    soundPlayers.clear();
    
    ofDirectory dir;
    dir.listDir(SOUND_BANK_DIR);
    
    if (dir.size()){
        dir.sort();
        for (int i=0; i<rows; i++) {
            
            int soundPathIndex = dir.numFiles() - (i % dir.numFiles()) - 1;
            
            ofSoundPlayer player;
            player.loadSound(dir.getPath(soundPathIndex));
            player.setMultiPlay(false);
            
            soundPlayers.push_back(player);
            
            ofLog(OF_LOG_NOTICE, dir.getPath(soundPathIndex) + " loaded into track " + ofToString(i + 1));
        }
    }
}