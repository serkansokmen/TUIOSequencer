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
    scanRect.setFromCenter(ofPoint(ofGetWidth() * .5, ofGetHeight() * .5), camWidth, camHeight);
#endif
    
#ifdef USE_KINECT
    
    // enable depth->rgb image calibration
    kinect.setRegistration(true);
    
    kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
    kinect.open();
    
    angle = kinect.getCurrentCameraTiltAngle();
    
    blobFinder.init(&kinect, true); // standarized coordinate system: z in the direction of gravity
    blobFinder.setResolution(BF_LOW_RES);
    blobFinder.setRotation( ofVec3f( angle, 0, 0) );
    blobFinder.setTranslation(ofVec3f(0,0,0));
    blobFinder.setScale(ofVec3f(0.001, 0.001, 0.001)); // mm to meters
    
    // bind our kinect to the blob finder
    // in order to do this we need to declare in App.h: class App : public ofBaseApp, public ofxKinectBlobListener
    blobTracker.setListener(this);
    
    colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	bgImage.allocate(kinect.width, kinect.height);
    grayDiff.allocate(kinect.width, kinect.height);
    grayDiffOfImage.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
    
    numPixels = kinect.width*kinect.height;
    
    // NOTE: measurement units in meters!!!
	minBlobVol = 0.02f;
	maxBlobVol = 2.0f;
    // no cropping
//    cropBoxMin = ofVec3f(0, 0, 0);
//    cropBoxMax = ofVec3f(20, 1.6, 20);
    //
    thresh3D = ofVec3f(0.2f, 0.2f, 0.3f);
    // xy pixel search range
    thresh2D = 1;
    maxBlobs = 36;
    
    float sqrResolution = blobFinder.getResolution();
    sqrResolution *= sqrResolution;
    minBlobPoints = (int)(0.001*(float)numPixels/sqrResolution);
    
    bLearnBackground = true;
    
    // start from the front
	bDrawPointCloud = true;
	bDrawIDs = true;
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetWidth() * .5, ofGetHeight() * .5), kinect.getWidth(), kinect.getHeight());
#endif

#ifdef USE_FLOB
    camWidth = 320;
    camHeight = 240;
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetWidth() * .5, ofGetHeight() * .5), camWidth * 2.5, camHeight * 2.5);
    
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
void App::update(){
    
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
        
        cout << m.getAddress() << endl;

        if (addrs.size() == 5 && addrs[1] == "controller" && addrs[2] == "grid"){
            int gridPosX = ofToInt(addrs[4]) - 1;
            int gridPosY = 6 - ofToInt(addrs[3]);
            
            float val = m.getArgAsFloat(0);
            if (val == 1.0f){
//                sequencer->setSegmentOn(gridPosX, gridPosY);
            } else {
//                sequencer->setSegmentOff(gridPosX, gridPosY);
            }
            
            ofLog(OF_LOG_NOTICE, "OSC Blob: " + ofToString(gridPosX) + ", " + ofToString(gridPosY) + " value:" + ofToString(val));
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
        
        // load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        // background subtraction
        if (bLearnBackground) {
            bgImage = grayImage;   // let this frame be the background image from now on
            bLearnBackground = false;
        }
        
        cvAbsDiff(bgImage.getCvImage(), grayImage.getCvImage(), grayDiff.getCvImage());
        cvErode(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 2);
        cvDilate(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 1);
        // threshold ignoring little differences
        cvThreshold(grayDiff.getCvImage(), grayDiff.getCvImage(), 3, 255, CV_THRESH_BINARY);
        grayDiff.flagImageChanged();
        // update the ofImage to be used as background mask for the blob finder
        grayDiffOfImage.setFromPixels(grayDiff.getPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
        
        blobFinder.findBlobs(&grayDiffOfImage,
                             cropBoxMin, cropBoxMax,
                             thresh3D, thresh2D,
                             minBlobVol, maxBlobVol, minBlobPoints,  maxBlobs);
        blobTracker.trackBlobs(blobFinder.blobs);
        
        sequencer->checkSegments(blobFinder.blobs);
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
void App::draw(){
    
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
    if(bDrawPointCloud) {
//        camera.begin();
        drawPointCloud();
//        camera.end();
	} else {
		// draw from the live kinect
//		kinect.drawDepth(10, 10, 320, 240);
//		kinect.draw(340, 10, 320, 240);
        
//		grayImage.draw(10, 260, 320, 240);
//		grayDiff.draw(340, 260, 320, 240);
        grayImage.draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
        grayDiff.draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
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
    
    ofPopMatrix();
    gui->setVisible(bDebugMode);
    if (gui->isVisible()) {
        gui->draw();
    }
}

//--------------------------------------------------------------
#ifdef USE_KINECT
void App::drawPointCloud() {
    ofPushMatrix();
    ofScale(100.0, 100.0, 100.0);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3);
    
//    ofDrawGridPlane(2);
    
    // draw blobs
    for (unsigned int i=0; i < blobFinder.blobs.size(); i++) {
        ofSetColor(25*i, 25*i, 255-25*i);
        // draw blobs
        blobFinder.blobs[i].draw();
        
        // plot blobs IDs
        if (bDrawIDs) {
            ofPushMatrix();
            ofTranslate(blobTracker.blobs[i].massCenter.x, blobTracker.blobs[i].massCenter.y, blobTracker.blobs[i].maxZ.z);
            ofRotateX(-90);
            ofScale(0.01f, 0.01f, 0.01f);
            ofSetColor(255,255,255);
            ofDrawBitmapString(ofToString(blobTracker.blobs[i].id), 0, 0);
            ofPopMatrix();
        }
        // draw trajectory as a line
        vector <ofVec3f> trajectory;
        blobTracker.getTrajectoryById(blobTracker.blobs[i].id, trajectory);
        unsigned int trjSize = trajectory.size();
        if (trjSize > 1) {
            ofPushMatrix();
            ofSetColor(255, 255, 0);
            ofSetLineWidth(1);
            glBegin(GL_LINE);
            for (unsigned int j = 0; j < trjSize; j++) {
                glVertex3f(trajectory[j].x, trajectory[j].y, trajectory[j].z);
            }
            glEnd();
            ofPopMatrix();
            trajectory.clear();
        }
    }
    glDisable(GL_DEPTH_TEST);
    ofPopMatrix();
}

//--------------------------------------------------
void App::blobOn(ofVec3f centroid, int id, int order){
    cout << "blobOn() - id:" << id << " order:" << order << endl;
}

void App::blobMoved(ofVec3f centroid, int id, int order){
    cout << "blobMoved() - id:" << id << " order:" << order << endl;
    // full access to blob object (get a reference)
    ofxKinectTrackedBlob *blob = &blobTracker.blobs[blobTracker.getIndexById(id)];
    cout << "volume: " << blob->volume << "," << blob->massCenter << endl;
    
    float x = ofMap(centroid.x, -.5, .5, 0, scanRect.getWidth());
    float y = ofMap(centroid.z, -.5, .5, 0, scanRect.getHeight());
    
    ofSetColor(ofColor::red);
    ofCircle(x, y, 40);
    sequencer->toggleSegment(x, y);
}

void App::blobOff(ofVec3f centroid, int id, int order){
    cout << "blobOff() - id:" << id << " order:" << order << endl;
}
#endif

//--------------------------------------------------------------
void App::initGUI(){
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
    gui->addButton("Learn Background", &bLearnBackground);
    gui->addToggle("Draw Point Cloud", &bDrawPointCloud);
    gui->addToggle("Draw Blob IDs", &bDrawIDs);
    gui->addSlider("tilt angle", -30.0f, 30.0f, &angle);
    gui->addSpacer();
    gui->addSlider("crop box min X", -20.0f, 20.0f, &cropBoxMin.x);
    gui->addSlider("crop box min Y", 0.0f, 2.0f, &cropBoxMin.y);
    gui->addSlider("crop box min Z", -20.0f, 20.0f, &cropBoxMin.z);
    gui->addSpacer();
    gui->addSlider("crop box max X", -20.0f, 20.0f, &cropBoxMax.x);
    gui->addSlider("crop box max Y", 0.0f, 2.0f, &cropBoxMax.y);
    gui->addSlider("crop box max Z", -20.0f, 20.0f, &cropBoxMax.z);
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
    
    ofAddListener(gui->newGUIEvent, this, &App::guiEvent);
}

//--------------------------------------------------------------
void App::guiEvent(ofxUIEventArgs &e){
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
void App::keyPressed(int key){
    
    switch (key) {
        case 's':
            gui->toggleVisible();
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
        
        sequencer->toggleSegment(rx, ry);
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
