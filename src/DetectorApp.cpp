//
//  SegmentsContainer.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#include "DetectorApp.h"


//--------------------------------------------------------------
void DetectorApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLineWidth(0.1);
    
    // Setup GUI first for loading initial values from previously saved XML
    initGUI();
    
    // Initialize segments container
    segmentsContainer = new SegmentsContainer;
    
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
void DetectorApp::update(){
    
    columns = (int)columns;
    rows = (int)rows;
    
    if (bInitGrid) {
        segmentsContainer->initGrid(scanRect, (int)columns, (int)rows);
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
        
        segmentsContainer->checkSegments(contourFinder.blobs);
	}
#else
    vidGrabber.update();
    
    flob.setMirror(bMirrorX, bMirrorY);
    
	if (vidGrabber.isFrameNew()){
        blobs = flob.calc(flob.binarize(vidGrabber.getPixels(), camWidth, camHeight));
	}
    
    if (blobs != NULL && blobs->size() > 0){
        segmentsContainer->checkSegments(blobs);
    }
#endif
    
    segmentsContainer->update();
    
}

//--------------------------------------------------------------
void DetectorApp::draw(){
    
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
    
    segmentsContainer->draw();
    
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
void DetectorApp::initGUI(){
    gui = new ofxUICanvas();
    gui->setFont("GUI/EnvyCodeR.ttf");
    gui->addLabel("Presence Detector Grid");
    gui->addSpacer();
    gui->addSlider("columns", 1.0f, 10.0f, &columns);
    gui->addSlider("rows", 1.0f, 10.0f, &rows);
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
    
    ofAddListener(gui->newGUIEvent, this, &DetectorApp::guiEvent);
}

//--------------------------------------------------------------
void DetectorApp::guiEvent(ofxUIEventArgs &e){
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
void DetectorApp::keyPressed(int key){
    
    switch (key) {
        case 's':
            gui->toggleVisible();
            break;
        
        default:
            break;
    }
}

//--------------------------------------------------------------
void DetectorApp::keyReleased(int key){

}

//--------------------------------------------------------------
void DetectorApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void DetectorApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void DetectorApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void DetectorApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void DetectorApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void DetectorApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void DetectorApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void DetectorApp::exit(){
    
#ifdef USE_KINECT
	kinect.close();
    
    gui->saveSettings("GUI/guiSettingsKinect.xml");
#else
    gui->saveSettings("GUI/guiSettingsFlob.xml");
#endif
    
    delete segmentsContainer;
    delete gui;
}
