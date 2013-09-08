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
    ofSetLogLevel(OF_LOG_NOTICE);
    
    ofSetWindowTitle("Body Sequencer");
    
    // Setup OpenTSPS
    tspsReceiver.connect(12000);
//    ofxAddTSPSListeners(this);
    
    // Setup Tweener
    Tweener.setMode(TWEENMODE_OVERRIDE);
    
    columns         = 4;
    rows            = 4;
    
    currentThemeId.addListener(this, &App::currentThemeIdChanged);
    bpm.addListener(this, &App::bpmChanged);
    
    // Initialize themes
    themes.clear();
    
    AppTheme    theme0, theme1;
    
    theme0.setup("themes/pack_1/sounds/", 224.0f, ofRectangle(322, 174, 632, 620), "themes/pack_1/images/interface.png");
    theme1.setup("themes/pack_2/sounds/", 192.0f, ofRectangle(322, 174, 632, 620), "themes/pack_2/images/interface.png");
    
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
    totalSteps = columns;
    currentStep = 0;
    lastStep = 0;
}

//--------------------------------------------------------------
void App::update(){
    
    // Update Tweener
    Tweener.update();
    
    // Update bpm
    bpmTapper.update();
    currentStep = (int)bpmTapper.beatTime() % totalSteps;
    if (currentStep > totalSteps){
        currentStep = 0;
    }
    
    vector<ofxTSPS::Person*> people = tspsReceiver.getPeople();
//    ofxTSPS::Scene *scene = tspsReceiver.getScene();
//    for (int i=0; i<scene->getGridValues().size(); i++) {
//        cout << "Scene value " << scene->getGridValues()[i] << endl;
//    }

    for (int i=0; i<sequencers.size(); i++) {
        sequencers[i].people.clear();
        for (int p=0; p<people.size(); p++) {
            sequencers[i].people.push_back(people[p]);
        }
    }
    
    // Update sequencer
    currentSequencer->update(currentStep);
    
    // Check on/off states and play cell sound
    for (int i=0; i<currentSequencer->tracks.size(); i++) {
        SequencerTrack *track = &currentSequencer->tracks[i];
        if (track->cells[currentStep].getState() != cellOff && lastStep != currentStep){
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
    
    currentThemeId.removeListener(this, &App::currentThemeIdChanged);
    bpm.removeListener(this, &App::bpmChanged);
    
    ofxRemoveTSPSListeners(this);
    themes.clear();
    
    currentTheme = 0;
    currentSequencer = 0;
    
    delete currentTheme;
    delete currentSequencer;
}

#pragma mark - OpenTSPS Listeners

void App::onPersonEntered( ofxTSPS::EventArgs & tspsEvent ){
    float x = tspsEvent.person->centroid.x;
    float y = tspsEvent.person->centroid.y;
    
	ofPoint loc = ofPoint(x, y);
    ofLog(OF_LOG_NOTICE, "Person entered");
}

//--------------------------------------------------------------
void App::onPersonUpdated( ofxTSPS::EventArgs & tspsEvent ){
    
    float x = tspsEvent.person->centroid.x;
    float y = tspsEvent.person->centroid.y;
    
//    ofLog(OF_LOG_NOTICE, "Person updated!");
}

//--------------------------------------------------------------
void App::onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent ){
    
    float x = tspsEvent.person->centroid.x;
    float y = tspsEvent.person->centroid.y;
    
	ofPoint loc = ofPoint(x, y);
	ofLog(OF_LOG_NOTICE, "Person left");
}


#pragma mark - ofParameter Event Listeners

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

//--------------------------------------------------------------
void App::bpmChanged(float &newBpm){
    bpmTapper.setBpm(newBpm);
}
