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
    currentTheme = new AppTheme();
    currentSequencer = new Sequencer();
}


//--------------------------------------------------------------
void App::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetWindowTitle("Body Sequencer");
    
    // Setup TUIO
    tuioClient.start(3333);
    
    ofAddListener(tuioClient.cursorAdded, this, &App::tuioAdded);
	ofAddListener(tuioClient.cursorRemoved, this, &App::tuioRemoved);
	ofAddListener(tuioClient.cursorUpdated, this, &App::tuioUpdated);
    
    // Setup Tweener
    Tweener.setMode(TWEENMODE_OVERRIDE);
    
    columns         = 4;
    rows            = 4;
    
    // Setup GUI
    setupGUI();
    
    // Initialize themes
    themes.clear();
    
    AppTheme    theme0, theme1;
    
    theme0.setup("themes/pack_1/sounds/", 224.0f, ofRectangle(360, 162, 707, 582), "themes/pack_1/images/interface.png");
    theme1.setup("themes/pack_2/sounds/", 192.0f, ofRectangle(364, 162, 707, 582), "themes/pack_2/images/interface.png");
    
    themes.push_back(theme0);
    themes.push_back(theme1);
    
    // Initialize Sequencer
    Sequencer seq0, seq1;
    seq0.setup(theme0.gridRect, columns, rows);
    seq1.setup(theme1.gridRect, columns, rows);
    
    sequencers.push_back(seq0);
    sequencers.push_back(seq1);
    
    currentThemeId = 0;
    
    // Set initial values
    bHideGui = true;
    
    totalSteps = columns;
    currentStep = 0;
    lastStep = 0;
}

//--------------------------------------------------------------
void App::update(){
    
    // Get TUIO messages
    tuioClient.getMessage();
    
    // Update Tweener
    Tweener.update();
    
    // Update bpm
    bpmTapper.update();
    currentStep = (int)bpmTapper.beatTime() % totalSteps;
    if (currentStep > totalSteps){
        currentStep = 0;
    }
    
    // Update sequencer
    currentSequencer->update(currentStep);
    
    // Check on/off states and play cell sound
    for (int i=0; i<currentSequencer->tracks.size(); i++) {
        SequencerTrack *track = &currentSequencer->tracks[i];
        if (track->cellStates[currentStep] > 0 && lastStep != currentStep){
            int j = currentStep + i * columns;
            if (themes[currentThemeId].players[j].isLoaded())
                themes[currentThemeId].players[j].play();
        }
    }
    lastStep = currentStep;
    
    sequencerFbo.begin();
    ofClear(255);
    currentSequencer->draw();
    sequencerFbo.end();
}

//--------------------------------------------------------------
void App::draw(){
    
    ofBackground(ofColor::black);
    
    // Draw background
    currentTheme->background.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    // Draw sequencer
    sequencerFbo.draw(currentTheme->gridRect.getX(),
                      currentTheme->gridRect.getY());
    
    if (!bHideGui) {
        gui.draw();
    }
}

//--------------------------------------------------------------
void App::keyPressed(int key){
    
    switch (key)
    {
        case '1':
            currentThemeId = 0;
            break;
        case '2':
            currentThemeId = 1;
            break;
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
    
    if (currentTheme->gridRect.inside(x, y)){
        float rx = x - currentTheme->gridRect.getX();
        float ry = y - currentTheme->gridRect.getY();
        currentSequencer->toggle(rx, ry);
    }
}

//--------------------------------------------------------------
void App::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void App::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void App::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void App::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void App::exit(){
    
    ofRemoveListener(tuioClient.cursorAdded, this, &App::tuioAdded);
	ofRemoveListener(tuioClient.cursorRemoved, this, &App::tuioRemoved);
	ofRemoveListener(tuioClient.cursorUpdated, this, &App::tuioUpdated);
    
    clearGUI();
    
    themes.clear();
    
    currentTheme = 0;
    currentSequencer = 0;
    
    delete currentTheme;
    delete currentSequencer;
}

#pragma mark - TUIO
//--------------------------------------------------------------
void App::tuioAdded(ofxTuioCursor &tuioCursor){
    float x = ofMap(tuioCursor.getX(), 0.0, 1.0, 1.0, 0.0) * currentSequencer->getBoundingBox().getWidth();
    float y = tuioCursor.getY()*currentSequencer->getBoundingBox().getHeight();
    
	ofPoint loc = ofPoint(x, y);
    ofLog(OF_LOG_NOTICE, "Point n" + ofToString(tuioCursor.getSessionId()) + " add at " + ofToString(loc));
    
    currentSequencer->existingCursors.push_back(&tuioCursor);
    currentSequencer->refreshCells();
}

//--------------------------------------------------------------
void App::tuioUpdated(ofxTuioCursor &tuioCursor){
    
    float x = ofMap(tuioCursor.getX(), 0.0, 1.0, 1.0, 0.0) * currentSequencer->getBoundingBox().getWidth();
    float y = tuioCursor.getY()*currentSequencer->getBoundingBox().getHeight();
    
	ofPoint loc = ofPoint(x, y);
    ofLog(OF_LOG_NOTICE, "Point n" + ofToString(tuioCursor.getSessionId()) + " updated at " + ofToString(loc));
    
    currentSequencer->refreshCells();
}

//--------------------------------------------------------------
void App::tuioRemoved(ofxTuioCursor &tuioCursor){
    float x = ofMap(tuioCursor.getX(), 0.0, 1.0, 1.0, 0.0) * currentSequencer->getBoundingBox().getWidth();
    float y = tuioCursor.getY()*currentSequencer->getBoundingBox().getHeight();
    
	ofPoint loc = ofPoint(x, y);
	ofLog(OF_LOG_NOTICE, "Point n" + ofToString(tuioCursor.getSessionId()) + " remove at " + ofToString(loc));
    
    for (int i=0; i<currentSequencer->existingCursors.size(); i++) {
        if (tuioCursor.getSessionId() == currentSequencer->existingCursors[i]->getSessionId()){
            currentSequencer->existingCursors.erase(currentSequencer->existingCursors.begin() + i);
        }
    }
    
    currentSequencer->refreshCells();
}


#pragma mark - Themes

//--------------------------------------------------------------
void App::currentThemeIdChanged(int &newThemeId){
    
    currentTheme = &themes[newThemeId];
    currentSequencer = &sequencers[newThemeId];
    
    // Allocate draw FBO
    sequencerFbo.allocate(currentTheme->gridRect.getWidth(), currentTheme->gridRect.getHeight());
    sequencerFbo.begin();
    ofClear(0, 0, 0, 0);
    sequencerFbo.end();
    
    bpmTapper.setBpm(currentTheme->bpm);
}


#pragma mark - GUI

//--------------------------------------------------------------
void App::setupGUI(){
    
    gui.setup("Themes");
    
    currentThemeId.addListener(this, &App::currentThemeIdChanged);
    
    gui.add(bpm.set("BPM", 192.0f, 20.0f, 240.0f));
    bpm.addListener(this, &App::bpmChanged);
    
    // Load existing values
    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void App::clearGUI(){
    
    gui.saveToFile("settings.xml");
    
    currentThemeId.removeListener(this, &App::currentThemeIdChanged);
    bpm.removeListener(this, &App::bpmChanged);
}

//--------------------------------------------------------------
void App::bpmChanged(float &newBpm){
    bpmTapper.setBpm(newBpm);
}
