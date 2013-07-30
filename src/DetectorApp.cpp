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
    
    bInitGrid = true;
}

//--------------------------------------------------------------
void DetectorApp::update(){
    
    columns = (int)columns;
    rows = (int)rows;
    
    if (bInitGrid) {
        createGrid((int)columns, (int)rows);
        bInitGrid = false;
    }

#ifdef USE_KINECT
    kinect.update();
    
    // there is a new frame and we are connected
	if (kinect.isFrameNew()) {
        
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.getWidth(), kinect.getHeight());
        
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
		contourFinder.findContours(grayImage, 10, (kinect.getWidth() * kinect.getHeight()) * .5, 40, true);
        
        // Reset segment touch states
        vector<GridSegment>::iterator segment;
        for (segment = segments.begin(); segment != segments.end(); segment++){
            segment->bTouchesBlob = false;
        }
        
        if (contourFinder.nBlobs > 0){
            for (int i=0; i<segments.size(); i++) {
                GridSegment *segmentPtr = &segments[i];
                
                for(int i=0; i<contourFinder.blobs.size(); i++){
                    ofxCvBlob &aBlob = contourFinder.blobs[i];
                    
                    if (aBlob.boundingRect.intersects(segmentPtr->rect)){
                        segmentPtr->bTouchesBlob = true;
                    }
                }
            }
        }
	}
#else
    vidGrabber.update();
    
    flob.setMirror(bMirrorX, bMirrorY);
    
	if (vidGrabber.isFrameNew()){
        blobs = flob.calc(flob.binarize(vidGrabber.getPixels(), camWidth, camHeight));
	}
    
    if (blobs != NULL && blobs->size() > 0){
        for(int i=0; i<blobs->size();i++){
            ABlob &aBlob = *(blobs->at(i));
            
            vector<GridSegment>::iterator segment;
            for (segment = segments.begin(); segment != segments.end(); segment++){
                segment->checkIntersectsBlob(aBlob);
            }
        }
    }
#endif
    
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->update();
    }
    
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
    
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->draw();
    }
    
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
void DetectorApp::createGrid(int columns, int rows){
    
    segments.clear();
    
    float rectW = scanRect.getWidth() / rows;
    float rectH = scanRect.getHeight() / columns;
    
    for (int y = 0; y < columns; y++) {
        for (int x = 0; x < rows; x++) {
            
            ofRectangle rect;
            rect.setPosition(x * rectW, y * rectH);
            rect.setWidth(rectW);
            rect.setHeight(rectH);
            
            segments.push_back(GridSegment(rect));
        }
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
    gui->addSlider("Near Threshold", 40.0f, 250.0f, &nearThreshold);
    gui->addSlider("Far Threshold", 40.0f, 250.0f, &farThreshold);
#else
    gui->addLabel("Flob");
    gui->addSlider("threshold", 1.0f, 80.0f, &threshold);
    gui->addSlider("fade", 1.0f, 100.0f, &fade);
#endif
    gui->addToggle("mirror x", &bMirrorX);
    gui->addToggle("mirror y", &bMirrorY);
    gui->addToggle("draw blobs", &bDrawBlobs);
    gui->addToggle("draw video", &bDrawVideo);
    gui->addSpacer();
    
    gui->addFPSSlider("fps");
    gui->autoSizeToFitWidgets();
    
    gui->setPosition(10, 10);
    
    gui->loadSettings("GUI/guiSettings.xml");
    gui->setVisible(true);
    
    ofAddListener(gui->newGUIEvent, this, &DetectorApp::guiEvent);
}

//--------------------------------------------------------------
void DetectorApp::guiEvent(ofxUIEventArgs &e){
#ifndef USE_KINECT
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
            
#ifdef USE_KINECT
        case OF_KEY_UP:
			angle++;
			if(angle > 30) angle = 30;
			kinect.setCameraTiltAngle(angle);
			break;
            
		case OF_KEY_DOWN:
			angle--;
			if(angle < -30) angle = -30;
			kinect.setCameraTiltAngle(angle);
			break;
#endif
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
    kinect.setCameraTiltAngle(0);
	kinect.close();
#endif
    
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}