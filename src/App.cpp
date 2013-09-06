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
    sequencer = new Sequencer();
}


//--------------------------------------------------------------
void App::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetWindowTitle("Body Sequencer");
    
    // Setup Tweener
    Tweener.setMode(TWEENMODE_OVERRIDE);
    
    columns         = 4;
    rows            = 4;
    
    // Setup GUI
    setupGUI();
    
    // Initialize themes
    themes.clear();
    
    AppTheme    theme0, theme1;
    
//    theme0.setup("themes/pack_1/sounds/", 224.0f, ofRectangle(257, 138, 502, 494), "themes/pack_1/images/interface.png");
//    theme1.setup("themes/pack_2/sounds/", 192.0f, ofRectangle(259, 140, 502, 494), "themes/pack_2/images/interface.png");
    theme0.setup("themes/pack_1/sounds/", 224.0f, ofRectangle(360, 162, 707, 582), "themes/pack_1/images/interface.png");
    theme1.setup("themes/pack_2/sounds/", 192.0f, ofRectangle(364, 162, 707, 582), "themes/pack_2/images/interface.png");
    
    themes.push_back(theme0);
    themes.push_back(theme1);
    
    // Initialize Sequencer
    sequencer->setup(currentTheme->gridRect, columns, rows);
    
    setTheme0();
    
    // Set initial values
    bHideGui = false;
    
    totalSteps = columns;
    currentStep = 0;
    lastStep = 0;
}

//--------------------------------------------------------------
void App::update(){
    
    // Update Tweener
    Tweener.update();
    
    // Update bpm
    if (bPlay){
        bpmTapper.update();
        currentStep = (int)bpmTapper.beatTime() % totalSteps;
        if (currentStep > totalSteps){
            currentStep = 0;
        }
        
        // Update sequencer
        sequencer->update(currentStep);
        
        // Check on/off states and play cell sound
        for (int i=0; i<sequencer->tracks.size(); i++) {
            SequencerTrack *track = &sequencer->tracks[i];
            if (track->cellStates[currentStep] > 0 && lastStep != currentStep){
                int j = currentStep + i * columns;
                if (themes[currentThemeId].players[j].isLoaded())
                    themes[currentThemeId].players[j].play();
            }
        }
        lastStep = currentStep;
    }
    
    sequencerFbo.begin();
    ofClear(255);
    sequencer->draw();
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
        sequencer->toggle(rx, ry);
    }
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
    
    clearGUI();
    
    themes.clear();
    
    currentTheme = 0;
    sequencer = 0;
    
    delete currentTheme;
    delete sequencer;
}


#pragma mark - Sequencer

////--------------------------------------------------------------
//void App::randomizeSequencer(){
//    sequencer->reset();
//    sequencer->randomize(randomizeRate);
//    sequencer->loadSounds(themes[currentThemeId].soundPath);
//}
//
////--------------------------------------------------------------
//void App::resetSequencer(){
//    sequencer->reset();
//    sequencer->setup(sequencer->getBoundingBox(), columns, rows);
//    bpmTapper.startFresh();
//}


#pragma mark - Themes

//--------------------------------------------------------------
void App::currentThemeIdChanged(int &newThemeId){
    
    currentTheme = &themes[newThemeId];
    
    // Allocate draw FBO
    sequencerFbo.allocate(currentTheme->gridRect.getWidth(), currentTheme->gridRect.getHeight());
    sequencerFbo.begin();
    ofClear(0, 0, 0, 0);
    sequencerFbo.end();
    
    sequencer->setup(currentTheme->gridRect, columns, rows);
    
    bpmTapper.setBpm(currentTheme->bpm);
}

//--------------------------------------------------------------
void App::setTheme0(){
    currentThemeId = 0;
}

//--------------------------------------------------------------
void App::setTheme1(){
    currentThemeId = 1;
}


#pragma mark - GUI

//--------------------------------------------------------------
void App::setupGUI(){
    
    gui.setup("Themes");
    
    //    gui.add(columns.set("Columns", 8, 4, 20));
    //    gui.add(rows.set("Rows", 8, 4, 20));
    columns.addListener(this, &App::columnsChanged);
    rows.addListener(this, &App::rowsChanged);
    
    //    gui.add(randomizeRate.set("Randomize Rate", 1, 1, 100));
    //    gui.add(randomizeButton.setup("Randomize"));
    //    gui.add(resetButton.setup("Reset"));
    
    //    randomizeButton.addListener(this, &App::randomizeSequencer);
    //    resetButton.addListener(this, &App::resetSequencer);
    
    gui.add(themeButton0.setup("Pack 1"));
    gui.add(themeButton1.setup("Pack 2"));
    
    themeButton0.addListener(this, &App::setTheme0);
    themeButton1.addListener(this, &App::setTheme1);
    
    currentThemeId.addListener(this, &App::currentThemeIdChanged);
    
    gui.add(bpm.set("BPM", 192.0f, 20.0f, 240.0f));
    bpm.addListener(this, &App::bpmChanged);
    
    gui.add(bPlay.set("Play", true));
    
    // Load existing values
    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void App::clearGUI(){
    
    gui.saveToFile("settings.xml");
    
    columns.removeListener(this, &App::columnsChanged);
    rows.removeListener(this, &App::rowsChanged);
    themeButton0.removeListener(this, &App::setTheme0);
    themeButton1.removeListener(this, &App::setTheme1);
    currentThemeId.removeListener(this, &App::currentThemeIdChanged);
    bpm.removeListener(this, &App::bpmChanged);
    //    randomizeButton.removeListener(this, &App::randomizeSequencer);
    //    resetButton.removeListener(this, &App::resetSequencer);
}

//--------------------------------------------------------------
void App::columnsChanged(int &newColumns){
    sequencer->setup(ofRectangle(0, 0, sequencer->getBoundingBox().getWidth(), sequencer->getBoundingBox().getHeight()), columns, rows);
    totalSteps = columns;
    cout << newColumns << endl;
}

//--------------------------------------------------------------
void App::rowsChanged(int &newRows){
    sequencer->setup(ofRectangle(0, 0, sequencer->getBoundingBox().getWidth(), sequencer->getBoundingBox().getHeight()), columns, rows);
}

//--------------------------------------------------------------
void App::bpmChanged(float &newBpm){
    bpmTapper.setBpm(newBpm);
}