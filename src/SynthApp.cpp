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
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLineWidth(0.1);
    
    // Setup GUI first for loading initial values from previously saved XML
    initGUI();
    
    // Initialize segments container
    sequencer = new Sequencer();
    
#ifdef USE_KINECT
    // Kinect setup
    kinect.init(true, true, false);
    kinect.open();
    
	grayImage.allocate(kinect.getWidth(), kinect.getHeight());
	grayThreshNear.allocate(kinect.getWidth(), kinect.getHeight());
	grayThreshFar.allocate(kinect.getWidth(), kinect.getHeight());
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetScreenWidth() * .5, ofGetScreenHeight() * .5), kinect.getWidth(), kinect.getHeight());
#else
    
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
    
    // Create initial grid on update
    bInitGrid = true;
}

//--------------------------------------------------------------
void SynthApp::update(){
    
    columns = (int)columns;
    rows = (int)rows;
    
    if (bInitGrid) {
        sequencer->setup(scanRect, (int)columns, (int)rows, speed, SEQ_DIRECTION_HORIZONTAL);
        bInitGrid = false;
    }

#ifdef USE_KINECT
    kinect.update();
    
    // there is a new frame and we are connected
	if (kinect.isFrameNew()) {
        
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
		contourFinder.findContours(grayImage, 10, (kinect.getWidth() * kinect.getHeight()) * .5, 20, true);
        
        sequencer->checkSegments(contourFinder.blobs);
	}
#else
    vidGrabber.update();
    
    flob.setMirror(bMirrorX, bMirrorY);
    
	if (vidGrabber.isFrameNew()){
        blobs = flob.calc(flob.binarize(vidGrabber.getPixels(), camWidth, camHeight));
	}
    
    if (blobs != NULL && blobs->size() > 0){
        sequencer->checkSegments(blobs);
    }
#endif
    
    sequencer->update();
}

//--------------------------------------------------------------
void SynthApp::draw(){
    
    // Draw scan rect
    ofPushMatrix();
    ofTranslate(scanRect.getPositionRef());
    
    if (bDrawVideo) {
        ofSetColor(ofColor::white);
#ifdef USE_KINECT
        grayImage.draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
#else
        flob.videotex->draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
#endif
    }
    
    sequencer->draw();
    
#ifdef USE_KINECT
    if (bDrawBlobs && contourFinder.nBlobs > 0) {
        contourFinder.draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
    }
#else
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

    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::greenYellow);
    ofRect(0, 0, scanRect.getWidth(), scanRect.getHeight());
    ofPopStyle();
    ofPopMatrix();
    
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
    gui->addSlider("columns", 1.0f, 15.0f, &columns);
    gui->addSlider("rows", 1.0f, 15.0f, &rows);
    gui->addLabelToggle("init grid", &bInitGrid);
    gui->addSpacer();

#ifdef USE_KINECT
    gui->addLabel("Kinect");
    gui->addSlider("near threshold", 40.0f, 250.0f, &nearThreshold);
    gui->addSlider("far threshold", 40.0f, 250.0f, &farThreshold);
    gui->addSlider("tilt angle", -30.0f, 30.0f, &angle);
#else
    gui->addLabel("Flob");
    gui->addSlider("threshold", 1.0f, 80.0f, &threshold);
    gui->addSlider("fade", 1.0f, 100.0f, &fade);
#endif
    gui->addToggle("mirror x", &bMirrorX);
    gui->addToggle("mirror y", &bMirrorY);
    gui->addSpacer();
    gui->addLabel("Debug");
    gui->addToggle("draw blobs", &bDrawBlobs);
    gui->addToggle("draw video", &bDrawVideo);
    gui->addSpacer();
    
    gui->addFPSSlider("fps");
    gui->autoSizeToFitWidgets();
    
    gui->setPosition(10, 10);

#ifdef USE_KINECT
    gui->loadSettings("GUI/guiSettingsKinect.xml");
#else
    gui->loadSettings("GUI/guiSettingsFlob.xml");
#endif
    gui->setVisible(true);
    
    ofAddListener(gui->newGUIEvent, this, &SynthApp::guiEvent);
}

//--------------------------------------------------------------
void SynthApp::guiEvent(ofxUIEventArgs &e){
#ifdef USE_KINECT
    if (e.widget->getName() == "tilt angle"){
        kinect.setCameraTiltAngle(angle);
    }
#else
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
#else
    gui->saveSettings("GUI/guiSettingsFlob.xml");
#endif
    
    delete sequencer;
    delete gui;
}
