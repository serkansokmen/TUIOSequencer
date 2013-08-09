//
//  sequencer.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#include "SynthApp.h"


//--------------------------------------------------------------
void SynthApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_SILENT);
    ofSetLineWidth(2.0);
    
    // Setup GUI first for loading initial values from previously saved XML
    initGUI();
    bDebugMode = true;
    speed = 1000;
    
    Tweener.setMode(TWEENMODE_OVERRIDE);
    
    // Creates initial grid
    bInitGrid = true;
    // Initialize segments container
    sequencer = new Sequencer();
    
#ifdef USE_OSC
    camWidth = 798;
    camHeight = 600;
    // Setup OSC
    oscReceiver.setup(OSC_RECEIVE_PORT);
    oscSender.setup(OSC_HOST, OSC_SEND_PORT);
    
    oscPoints.assign(OSC_POINT_COUNT, OSCPoint());
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetScreenWidth() * .5, ofGetScreenHeight() * .5), camWidth, camHeight);
#endif
    
#ifdef USE_KINECT
    
    // Kinect setup
    //    kinect.init(true, true, false);
    kinect.init();
    kinect.open();
    
    colorImg.allocate(kinect.getWidth(), kinect.getHeight());
	grayImage.allocate(kinect.getWidth(), kinect.getHeight());
	grayThreshNear.allocate(kinect.getWidth(), kinect.getHeight());
	grayThreshFar.allocate(kinect.getWidth(), kinect.getHeight());
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetScreenWidth() * .5, ofGetScreenHeight() * .5), kinect.getWidth(), kinect.getHeight());
#endif

#ifdef USE_FLOB
    camWidth = 320;
    camHeight = 240;
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetScreenWidth() * .5, ofGetScreenHeight() * .5), camWidth * 2.5, camHeight * 2.5);
    
    vidGrabber.setVerbose(true);
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(camWidth, camHeight);
    
    // Flob setup
    blobs = NULL;
    flob.setup(camWidth, camHeight, scanRect.getWidth(), scanRect.getHeight());
	flob.setOm(Flob::CONTINUOUS_DIFFERENCE)
        ->setColormode(Flob::LUMA601)
        ->setFade(fade)
        ->setThresh(threshold)
        ->setThresholdmode(Flob::ABSDIF);
#endif
}

//--------------------------------------------------------------
void SynthApp::update(){
    
    sequenceDirection direction = SEQ_DIRECTION_HORIZONTAL;
    
    Tweener.update();
    
#ifdef USE_OSC
    
    bool bCheckPoints = false;
    
    // check for waiting messages
    while (oscReceiver.hasWaitingMessages()){
        
        // get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage(&m);
        
        // check for grid message
        vector<string> addrs = ofSplitString(m.getAddress(), "/");

        if (addrs.size() == 5 && addrs[1] == "grid" && addrs[2] == "sequencer"){
            int gridPosX = ofToInt(addrs[4]) - 1;
            int gridPosY = 6 - ofToInt(addrs[3]);
            
            float val = m.getArgAsFloat(0);
            if (val == 1.0f){
                sequencer->segmentOn(gridPosX, gridPosY);
            } else {
                sequencer->segmentOff(gridPosX, gridPosY);
            }
            
            ofLog(OF_LOG_NOTICE, "OSC Blob: " + ofToString(gridPosX) + ", " + ofToString(gridPosY) + " value:" + ofToString(val));
        }
        
        // check for position messages
        for (int i=0; i<OSC_POINT_COUNT; i++) {
            string indexStr = ofToString(i);
            OSCPoint &p = oscPoints[i];
            
            if (m.getAddress() == "/controller/blobs/" + indexStr){

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
            Tweener.addTween(p.alpha, (p.isOn ? 1.0f : 0.0f), .1, &ofxTransitions::easeOutSine);
        }
        
        // check for horizontal message
		if (m.getAddress() == "/controller/direction"){
            switch ((int)m.getArgAsFloat(0)) {
                case 0:
                    direction = SEQ_DIRECTION_HORIZONTAL;
                    break;
                case 1:
                    direction = SEQ_DIRECTION_VERTICAL;
                    break;
                default:
                    break;
            }
            bInitGrid = true;
            ofLog(OF_LOG_NOTICE, "Direction changed");
		}
        
        // check for speed message
		else if (m.getAddress() == "/controller/speed"){
            speed = m.getArgAsFloat(0);
            sequencer->setSpeed(speed);
            ofLog(OF_LOG_NOTICE, "Speed: " + ofToString(speed));
		}
        
        // check debug message
		else if (m.getAddress() == "/controller/debug"){
			bDebugMode = !(m.getArgAsFloat(0) == 0.0f);
            ofLog(OF_LOG_NOTICE, "Debug mode:" + ofToString(bDebugMode));
		}
        
        // check for init message
		else if (m.getAddress() == "/controller/init"){
			bInitGrid = !(m.getArgAsFloat(0) == 0.0f);
            ofLog(OF_LOG_NOTICE, "initializing grid...");
		}
	}
#endif

#ifdef USE_KINECT
    kinect.update();
    
    // there is a new frame and we are connected
	if (kinect.isFrameNew()) {
        
        colorImg.setFromPixels(kinect.getPixels(), kinect.getWidth(), kinect.getHeight());
        colorImg.mirror(bMirrorY, bMirrorX);
        colorImg.blurGaussian();
        
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.getWidth(), kinect.getHeight());
        grayImage.mirror(bMirrorY, bMirrorX);
        grayImage.blurGaussian();
        
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        
		// update the cv images
		grayImage.flagImageChanged();
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 10, (kinect.getWidth() * kinect.getHeight()) * .5, 20, false);
        
        sequencer->checkSegments(contourFinder.blobs);
	}
#endif

#ifdef USE_FLOB
    vidGrabber.update();
    
    flob.setMirror(bMirrorX, bMirrorY);
    
	if (vidGrabber.isFrameNew()){
        blobs = flob.calc(flob.binarize(vidGrabber.getPixels(), camWidth, camHeight));
	}
    
    if (blobs != NULL && blobs->size() > 0){
        sequencer->checkSegments(blobs);
    }
#endif
    
    // Re-init sequencer
    if (bInitGrid) {
        sequencer->setup(scanRect, COLUMNS, ROWS, speed, direction);
        bInitGrid = false;
    }
    // Update sequencer
    sequencer->update();
}

//--------------------------------------------------------------
void SynthApp::draw(){
    
//    ofBackgroundGradient(ofColor::black, ofColor::darkBlue, OF_GRADIENT_LINEAR);
    
    // Draw scan rect
    ofPushMatrix();
    ofTranslate(scanRect.getPositionRef());
    
    if (bDrawVideo) {
#ifdef USE_KINECT
        ofSetColor(ofColor::white);
        //grayImage.draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
        colorImg.draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
#endif
        
#ifdef USE_FLOB
        ofSetColor(ofColor::white);
        flob.videotex->draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
#endif
    }
    
#ifdef USE_KINECT
    if (bDrawBlobs && contourFinder.nBlobs > 0) {
        contourFinder.draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
    }
#endif
    
#ifdef USE_FLOB
    if (bDrawBlobs && blobs != NULL && blobs->size() > 0){
		for(int i=0; i<blobs->size();i++){
			ABlob &aBlob = *(blobs->at(i));
			ofSetColor(ofColor::pink, 100);
			ofRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
			ofSetColor(ofColor::plum, 200);
			ofRect(aBlob.cx, aBlob.cy, 10, 10);
		}
	}
#endif
    
    sequencer->draw();
    
#ifdef USE_OSC
//    if (bDebugMode) {
//        vector<OSCPoint>::iterator point;
//        for (point = oscPoints.begin(); point != oscPoints.end(); *point++){
//            ofPushMatrix();
//            ofPushStyle();
//            ofSetColor(0, 255, 0, point->alpha);
//            ofCircle(point->position, 44);
//            ofPopStyle();
//            ofPopMatrix();
//        }
//    }
#endif
    
    ofPopMatrix();
    gui->setVisible(bDebugMode);
    if (gui->isVisible()) {
        gui->draw();
    }
}

//--------------------------------------------------------------
void SynthApp::initGUI(){
    gui = new ofxUICanvas();
    gui->setFont("GUI/EnvyCodeR.ttf");
    gui->addLabel("MotionSynth");
    gui->addSpacer();
    gui->addFPSSlider("fps");
    gui->addSpacer();
    gui->addSlider("speed", 100.0f, 2000.0f, &speed);
    gui->addLabelToggle("start", &bInitGrid);

#ifdef USE_KINECT
    gui->addSpacer();
    gui->addLabel("Kinect");
    gui->addSlider("near threshold", 40.0f, 250.0f, &nearThreshold);
    gui->addSlider("far threshold", 40.0f, 250.0f, &farThreshold);
    gui->addSlider("tilt angle", -30.0f, 30.0f, &angle);
#endif
    
#ifdef USE_FLOB
    gui->addSpacer();
    gui->addLabel("Flob");
    gui->addSlider("threshold", 1.0f, 80.0f, &threshold);
    gui->addSlider("fade", 1.0f, 100.0f, &fade);
#endif
    
#ifndef USE_OSC
    gui->addSpacer();
    gui->addToggle("mirror x", &bMirrorX);
    gui->addToggle("mirror y", &bMirrorY);
    gui->addSpacer();
    gui->addLabel("Debug");
    gui->addToggle("draw blobs", &bDrawBlobs);
    gui->addToggle("draw video", &bDrawVideo);
#endif
    
    gui->autoSizeToFitWidgets();
    gui->setPosition(10, 10);

#ifdef USE_KINECT
    gui->loadSettings("GUI/guiSettingsKinect.xml");
#endif
    
#ifdef USE_FLOB
    gui->loadSettings("GUI/guiSettingsFlob.xml");
#endif

#ifdef USE_OSC
    gui->loadSettings("GUI/guiSettingsOSC.xml");
#endif
    
#ifndef USE_OSC
    gui->setVisible(true);
#endif
    
    ofAddListener(gui->newGUIEvent, this, &SynthApp::guiEvent);
}

//--------------------------------------------------------------
void SynthApp::guiEvent(ofxUIEventArgs &e){
#ifdef USE_KINECT
    if (e.widget->getName() == "tilt angle"){
        kinect.setCameraTiltAngle(angle);
    }
#endif
    
    if (e.widget->getName() == "speed"){
        sequencer->setSpeed(speed);
    }
    
#ifdef USE_FLOB
    if (e.widget->getName() == "threshold"){
        flob.setThresh(threshold);
    }
    
    if (e.widget->getName() == "fade") {
        flob.setFade(fade);
    }
#endif
}

//--------------------------------------------------------------
void SynthApp::keyPressed(int key){
    
    switch (key) {
        case 's':
            gui->toggleVisible();
            break;
        
        default:
            break;
    }
}

//--------------------------------------------------------------
void SynthApp::keyReleased(int key){

}

//--------------------------------------------------------------
void SynthApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void SynthApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void SynthApp::mousePressed(int x, int y, int button){
    if (bDebugMode){
        float rx = x - scanRect.getX();
        float ry = y - scanRect.getY();
        
        sequencer->toggleSegment(rx, ry);
    }
}

//--------------------------------------------------------------
void SynthApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void SynthApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void SynthApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void SynthApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void SynthApp::exit(){
    
#ifdef USE_KINECT
	kinect.close();
    
    gui->saveSettings("GUI/guiSettingsKinect.xml");
#endif
    
#ifdef USE_FLOB
    gui->saveSettings("GUI/guiSettingsFlob.xml");
#endif
    
#ifdef USE_OSC
    gui->saveSettings("GUI/guiSettingsOSC.xml");
#endif
    
    delete sequencer;
    delete gui;
}
