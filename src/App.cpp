//
//  sequencer.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#include "App.h"


//--------------------------------------------------------------
void App::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLineWidth(2.0);
    
    // Scan rect
    float w = 640;
    float h = 480;
    scanRect.set(ofPoint(ofGetWidth() - w - 20, 20), w, h);
    
    // Initialize Sequencer
    sequencer = new Sequencer();
    sequencer->setup(scanRect, COLUMNS, ROWS);
    
    // initialize ofSoundStreamSetup
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
    
    // Setup GUIs
    setupGUIMain();
    
    // BPM
    bpm = 120.0f;
    totalSteps = 4;
    currentStep = 0;
    
    // Debug bools
    bDebugMode = true;
    bInitGrid = false;
    
    // Load GUI Settings
    loadGUISettings();
    
    // Hide all guis
    for (int i=0; i<guihash.size(); i++) {
        guihash[ofToString(i+1)]->setVisible(false);
    }
    guihash["1"]->setVisible(true);
    
    // Setup OSC
    oscReceiver.setup(OSC_RECEIVE_PORT);
    oscSender.setup(OSC_HOST, OSC_SEND_PORT);
    
    oscPoints.assign(OSC_POINT_COUNT, OSCPoint());
}

//--------------------------------------------------------------
void App::update(){
    
    // Update sound
    ofSoundUpdate();
    
    // Update bpm
    bpmTapper.update();
    currentStep = (int)bpmTapper.beatTime() % totalSteps;
    if (currentStep > totalSteps) currentStep = 0;
    
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
//            if (val == 1.0f){
            sequencer->toggleIndex(gridIndX, gridIndY);
//            }
            
            ofLog(OF_LOG_NOTICE, "OSC Blob: " + ofToString(gridIndX) + ", " + ofToString(gridIndY) + " value:" + ofToString(val));
        }
        
        // check for position messages
        for (int i=0; i<OSC_POINT_COUNT; i++) {
            string indexStr = ofToString(i);
            OSCPoint &p = oscPoints[i];
            
            if (m.getAddress() == "/simulate/blobs/" + indexStr){

                float posX = ofMap(m.getArgAsFloat(1), 0.0f, 1.0f, 0.0f, scanRect.getWidth());
                float posY = ofMap(m.getArgAsFloat(0), 0.0f, 1.0f, 0.0f, scanRect.getHeight());
                
                p.isOn = true;
                p.position.set(posX, posY);
                
                ofLog(OF_LOG_NOTICE, "Watching Blob " + indexStr);
                ofLog(OF_LOG_NOTICE, "X: " + ofToString(posX));
                ofLog(OF_LOG_NOTICE, "Y: " + ofToString(posY));
                
                bCheckPoints = true;
            }
            if (bCheckPoints) {
                sequencer->checkSegments(oscPoints);
            }
        }
        
        // check for bpm message
		if (m.getAddress() == "/controller/bpm"){
            float tempo = m.getArgAsFloat(0);
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

    // Restart Sequencer
    if (bInitGrid) {
        sequencer->setup(scanRect, COLUMNS, ROWS);
        bInitGrid = false;
    }
    
    // Update sequencer
    sequencer->update(currentStep);
}

//--------------------------------------------------------------
void App::draw(){
    
    // Draw scan rect
    ofPushMatrix();
    ofTranslate(scanRect.getPositionRef());
    
    sequencer->draw();
    
    if (bDebugMode){
        float s = 12;
        ofSetColor(ofColor::gray);
        ofRect(0, scanRect.getHeight() + s + 10, s * totalSteps, s);
        ofSetColor(ofColor::greenYellow);
        ofRect(currentStep * s, scanRect.getHeight() + s + 10, s, s);
    }
    
    ofPopMatrix();
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
        guiMain->addLabel("MAIN");
        guiMain->addSpacer();
        guiMain->addFPSSlider("FPS");
        guiMain->addSpacer();
        guiMain->addMinimalSlider("BPM", 32.0f, 240.0f, &bpm);
        guiMain->addLabelToggle("RESET", &bInitGrid);
        guiMain->addToggle("DEBUG MODE", &bDebugMode);
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
        case '2':
        {
            // Hide all guis
            for (int i=0; i<guihash.size(); i++) {
                guihash[ofToString(i+1)]->setVisible(false);
            }
            guihash["2"]->toggleVisible();
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
    
    if (bDebugMode){
        float rx = x - scanRect.getX();
        float ry = y - scanRect.getY();
        sequencer->toggle(rx, ry);
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

    saveGUISettings();
    
    for(int i = 0; i < guis.size(); i++)
    {
        ofxUICanvas *gui = guis[i];
        delete gui;
    }
    guis.clear();
    
    delete sequencer;
}


//--------------------------------------------------------------
void App::audioRequested(float * output, int bufferSize, int nChannels){
    
}
