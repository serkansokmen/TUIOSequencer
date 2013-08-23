//
//  App.cpp
//  BodySequencerApp
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#include "App.h"


App::App(){
    sequencer = new Sequencer();
}


//--------------------------------------------------------------
void App::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_WARNING);
    ofSetLineWidth(2.0);
    
    ofSetWindowTitle("TUIO Sequencer");
    ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())*.5, (ofGetScreenHeight()-ofGetHeight())*.5);
    
    tuioClient.start(3333);
    
    Tweener.setMode(TWEENMODE_OVERRIDE);
    
    // Setup GUIs
    setupGUIMain();
    
    // Set initial values
    bDebugMode = true;
    bInitGrid = false;
    randomizeRate = 1.0f;
    
    // Load GUI Settings
    loadGUISettings();
    
    columns = 8;
    rows = 8;
    
    // Initialize Sound Bank
    loadSoundBank();
    
    // Hide all guis
    for (int i=0; i<guihash.size(); i++) {
        guihash[ofToString(i+1)]->setVisible(false);
    }
    guihash["1"]->setVisible(true);
    
    // BPM
    bpm = 120.0f;
    totalSteps = columns;
    currentStep = 0;
    lastStep = 0;
    
    
    // Scan rect
    float w = 640;
    float h = 480;
    ofRectangle scanRect(0, 0, w, h);
    
    // Initialize Sequencer
    sequencer->setup(scanRect, columns, rows);
    // Allocate draw FBO
    sequencerFbo.allocate(scanRect.getWidth(), scanRect.getHeight());
    
    // Setup OSC
    oscReceiver.setup(OSC_RECEIVE_PORT);
    oscSender.setup(OSC_HOST, OSC_SEND_PORT);
    
    oscPoints.assign(OSC_POINT_COUNT, OSCPoint());
}

//--------------------------------------------------------------
void App::update(){
    
    // Get TUIO messages
    tuioClient.getMessage();
    
    // Update sound
    ofSoundUpdate();
    
    // Update Tweener
    Tweener.update();
    
    // Convert to int
    columns = (int)columns;
    rows = (int)rows;
    
    // Update bpm
    bpmTapper.update();
    currentStep = (int)bpmTapper.beatTime() % totalSteps;
    if (currentStep > totalSteps){
        // Testing automated play
        bRandomizeSequencer = true;
        currentStep = 0;
    }
    
    // Restart sequencer when needed
    if (bInitGrid) {
        sequencer->setup(sequencer->getBoundingBox(), columns, rows);
        bInitGrid = false;
    }
    // Randomize sequencer when needed
    if (bRandomizeSequencer){
        sequencer->reset();
        sequencer->randomize(randomizeRate);
        bRandomizeSequencer = false;
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
    
    
    // OSC Messages
    bool bCheckPoints = false;
    // check for waiting messages
    while (oscReceiver.hasWaitingMessages()){
        
        // get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage(&m);
        
        // check for grid message
        vector<string> addrs = ofSplitString(m.getAddress(), "/");
        
        if (addrs.size() == 5 && addrs[1] == "controller" && addrs[2] == "grid"){
            int gridIndX = ofToInt(addrs[4]) - 1;
            int gridIndY = 6 - ofToInt(addrs[3]);
            
            float val = m.getArgAsFloat(0);
//          if (val == 1.0f){}
            sequencer->toggleIndex(gridIndX, gridIndY);
            ofLog(OF_LOG_NOTICE, "OSC Blob: " + ofToString(gridIndX) + ", " + ofToString(gridIndY) + " value:" + ofToString(val));
        }
        
        // check for position messages
        for (int i=0; i<OSC_POINT_COUNT; i++) {
            string indexStr = ofToString(i);
            OSCPoint &p = oscPoints[i];
            
            if (m.getAddress() == "/simulate/blobs/" + indexStr){

                float posX = ofMap(m.getArgAsFloat(1), 0.0f, 1.0f, 0.0f, sequencer->getBoundingBox().getWidth());
                float posY = ofMap(m.getArgAsFloat(0), 0.0f, 1.0f, 0.0f, sequencer->getBoundingBox().getHeight());
                
                p.isOn = true;
                p.position.set(posX, posY);
                
                ofLog(OF_LOG_NOTICE, "Watching Blob " + indexStr);
                ofLog(OF_LOG_NOTICE, "X: " + ofToString(posX));
                ofLog(OF_LOG_NOTICE, "Y: " + ofToString(posY));
                
                bCheckPoints = true;
            }
            if (bCheckPoints) {
#warning needs OSC - check cells implementation.
//                sequencer->checkSegments(oscPoints);
            }
        }
        
        // check for bpm message
		if (m.getAddress() == "/controller/bpm"){
            float tempo = m.getArgAsFloat(0);
#warning handle OSC bpm message.
//            synth.setParameter("tempo", tempo);
            ofLog(OF_LOG_NOTICE, "BPM: " + ofToString(tempo));
		}
        
        // check debug message
		if (m.getAddress() == "/controller/debug"){
			bDebugMode = !(m.getArgAsFloat(0) == 0.0f);
            ofLog(OF_LOG_NOTICE, "Debug mode:" + ofToString(bDebugMode));
		}
        
        // check for init message
		if (m.getAddress() == "/controller/init"){
			bInitGrid = !(m.getArgAsFloat(0) == 0.0f);
            ofLog(OF_LOG_NOTICE, "initializing grid...");
		}
	}

    sequencerFbo.begin();
    ofBackground(0);
    sequencer->draw();
    sequencerFbo.end();
}

//--------------------------------------------------------------
void App::draw(){
    
    // Draw scan rect
    ofPushMatrix();
    ofTranslate(sequencer->getBoundingBox().getPosition());
    sequencerFbo.draw(sequencerPosition.x, sequencerPosition.y);
    ofPopMatrix();
    
    if (bDebugMode){
        ofPushStyle();
        float sx = 10;
        float sy = 320;
        float sw = 10;
        float sh = 10;
        ofSetColor(ofColor::grey);
        ofRect(sx, sy, sw * totalSteps, sh);
        ofSetColor(ofColor::greenYellow);
        ofRect((currentStep + 1) * sx, sy, sw, sh);
        ofSetColor(ofColor::darkGrey);
        ofDrawBitmapString(ofToString(currentStep+1), (currentStep + 1) * sx + 1, sy + sh*2+4);
        ofPopStyle();
        
        tuioClient.drawCursors();
    }
}

//--------------------------------------------------------------
void App::tuioAdded(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " add at " << loc << endl;
}

void App::tuioUpdated(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " updated at " << loc << endl;
}

void App::tuioRemoved(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " remove at " << loc << endl;
}

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

//--------------------------------------------------------------
void App::setupGUIMain(){
    
    // ---
    ofxUICanvas *guiMain = new ofxUICanvas();
    guiMain->setName("MAIN");
    guiMain->setFont("GUI/EnvyCodeR.ttf");
    guiMain->setColorFill(ofxUIColor(200));
    guiMain->setColorFillHighlight(ofxUIColor(255));
    guiMain->setColorBack(ofxUIColor(20, 20, 20, 150));
    {
        guiMain->addLabel("MAIN SETTINGS");
        guiMain->addSpacer();
        guiMain->addSlider("COLUMNS", 4, 16, &columns);
        guiMain->addSlider("ROWS", 4, 16, &rows);
        guiMain->addSpacer();
        guiMain->addSlider("BPM", 32.0f, 240.0f, &bpm);
        guiMain->addSpacer();
        guiMain->addToggle("RESET", &bInitGrid);
        guiMain->addSpacer();
        guiMain->addSlider("RANDOMIZE RATE", 0, 100, &randomizeRate);
        guiMain->addToggle("RANDOMIZE", &bRandomizeSequencer);
        guiMain->addToggle("DEBUG MODE", &bDebugMode);
        guiMain->addSpacer();
        guiMain->addFPSSlider("FPS");
    }
    guiMain->autoSizeToFitWidgets();
    
    // add gui to a c++ stl vector
    guis.push_back(guiMain);
    // add gui to a c++ stl map
    guihash["1"] = guiMain;
    
    ofAddListener(guiMain->newGUIEvent, this, &App::guiEvent);
}

//--------------------------------------------------------------
void App::loadGUISettings()
{
    vector<ofxUICanvas *>::iterator it;
    for(it = guis.begin(); it != guis.end(); it++)
    {
        (*it)->loadSettings("GUI/"+(*it)->getName()+"Settings.xml");
    }
}
//--------------------------------------------------------------
void App::saveGUISettings()
{
    vector<ofxUICanvas *>::iterator it;
    for(it = guis.begin(); it != guis.end(); it++)
    {
        (*it)->saveSettings("GUI/"+(*it)->getName()+"Settings.xml");
    }
}


//--------------------------------------------------------------
void App::guiEvent(ofxUIEventArgs &e){
    
    ofxUIMinimalSlider *slider;
    
    // Tempo
    if (e.widget->getName() == "BPM"){
        slider = (ofxUIMinimalSlider *) e.widget;
        bpmTapper.setBpm(slider->getScaledValue());
        bpmTapper.startFresh();
        sequencer->reset();
    }
    
    // Columns and rows
    if (e.widget->getName() == "COLUMNS" || e.widget->getName() == "ROWS"){
        if (sequencer != NULL) {
            loadSoundBank();
            sequencer->setup(ofRectangle(0, 0, sequencer->getBoundingBox().getWidth(), sequencer->getBoundingBox().getHeight()), columns, rows);
            totalSteps = columns;
        }
    }
}

//--------------------------------------------------------------
void App::keyPressed(int key){
    
    switch (key)
    {
        
        case '1':
        {
            // Hide all guis
            for (int i=0; i<guihash.size(); i++) {
                guihash[ofToString(i+1)]->setVisible(false);
            }
            guihash["1"]->toggleVisible();
        }
            break;
            
        case 'p':
        {
            vector<ofxUICanvas *>::iterator it;
            for(it = guis.begin(); it != guis.end(); it++)
            {
                (*it)->setDrawWidgetPadding(true);
            }
        }
            break;
            
        case 'P':
        {
            vector<ofxUICanvas *>::iterator it;
            for(it = guis.begin(); it != guis.end(); it++)
            {
                (*it)->setDrawWidgetPadding(false);
            }
        }
            break;
        
        case 'd':
            bDebugMode = !bDebugMode;
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
    currentRect->setPosition(sequencerPosition);
    
    if (bDebugMode && currentRect->inside(x, y)){
        float rx = x - sequencerPosition.x;
        float ry = y - sequencerPosition.y;
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
    // Re-layout Sequencer
    sequencerPosition.set((ofGetWidth() - sequencer->getBoundingBox().getWidth()) * .5,
                          (ofGetHeight() - sequencer->getBoundingBox().getHeight()) * .5);
}

//--------------------------------------------------------------
void App::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void App::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void App::exit(){

    saveGUISettings();
    
    for(int i = 0; i < guis.size(); i++)
    {
        ofxUICanvas *gui = guis[i];
        gui = 0;
        delete gui;
    }
    
    guis.clear();
//    samples.clear();
    soundPlayers.clear();
    
    delete sequencer;
}


//--------------------------------------------------------------
//void App::audioRequested(float *output, int bufferSize, int nChannels){
//    
//    for (int i = 0; i < bufferSize; i++){
//        
//        compositeSample = 0;
//        
//        // Check on/off states
//        for (int j=0; j<sequencer->tracks.size(); j++) {
//            
//            Track *track = &sequencer->tracks[j];
//            maxiSample *ms = &samples[j];
//            
//            if (track->cellStates[currentStep] > 0){
//                compositeSample += ms->play();
//            }
//        }
//		
//        mix.stereo(compositeSample, outputs, 0.5);
//		
//        lAudioOut[i] = output[i*nChannels    ] = outputs[0];
//        rAudioOut[i] = output[i*nChannels + 1] = outputs[1];
//	}
//}
