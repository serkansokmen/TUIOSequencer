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
    
    //------------------------------------------
    // Initialize Sequencer
    //------------------------------------------
    sequencer = new Sequencer();
    
    // initialize ofSoundStreamSetup
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
    
    // Setup synthesizer
    setupSynth();
    
    //------------------------------------------
    // Setup GUIs
    //------------------------------------------
    // Re-order these to change where they are shown
    setupGUIMain();
#ifdef USE_KINECT
    setupGUIKinect();
#endif
#ifdef USE_FLOB
    setupGUIFlob();
#endif
    setupGUITonic();
    
    // Load GUI Settings
    loadGUISettings();
    
    // Hide all guis
    for (int i=0; i<guihash.size(); i++) {
        guihash[ofToString(i+1)]->setVisible(false);
    }
    guihash["1"]->setVisible(true);
    
    
    
    bDebugMode = true;
    bInitGrid = true;
    
    // Setup Tweener
    Tweener.setMode(TWEENMODE_OVERRIDE);
    
#ifdef USE_OSC
    camWidth = 798;
    camHeight = 600;
    // Setup OSC
    oscReceiver.setup(OSC_RECEIVE_PORT);
    oscSender.setup(OSC_HOST, OSC_SEND_PORT);
    
    oscPoints.assign(OSC_POINT_COUNT, OSCPoint());
    
    // Reset scan rect
    resetScanRect();
#endif
    
#ifdef USE_KINECT
    camWidth = kinect.width;
    camHeight = kinect.height;
    
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
    
    // Reset scan rect
    resetScanRect();
#endif

#ifdef USE_FLOB
    camWidth = 320;
    camHeight = 240;
    
    // Reset scan rect
    resetScanRect();
    
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
    
    Tweener.update();
    ofSoundUpdate();
    
#ifdef USE_OSC
    
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
            tonicTempo = m.getArgAsFloat(0);
            ofLog(OF_LOG_NOTICE, "BPM: " + ofToString(tonicTempo));
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
    
    // Restart Sequencer
    if (bInitGrid) {
        sequencer->setup(scanRect, COLUMNS, ROWS);
        bInitGrid = false;
    }
    
    // Update sequencer
    sequencer->update();
    
    // Get active track cells
    
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
    ofLog(OF_LOG_VERBOSE, "blob on - id:" + ofToString(id) << " order:" << ofToString(order));
}

void App::blobMoved(ofVec3f centroid, int id, int order){
    ofLog(OF_LOG_VERBOSE, "blob moved - id: " + ofToString(id) + " order: " + ofToString(order));
    
    // full access to blob object (get a reference)
    ofxKinectTrackedBlob *blob = &blobTracker.blobs[blobTracker.getIndexById(id)];
    
    float x = ofMap(centroid.x, -.5, .5, 0, scanRect.getWidth());
    float y = ofMap(centroid.z, -.5, .5, 0, scanRect.getHeight());
    
    ofSetColor(ofColor::red);
    ofCircle(x, y, 40);
    sequencer->toggleSegment(x, y);
}

void App::blobOff(ofVec3f centroid, int id, int order){
    ofLog(OF_LOG_VERBOSE, "blob off - id:" + ofToString(id) << " order:" << ofToString(order));
}
#endif


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
        guiMain->addLabelToggle("RESET", &bInitGrid);
        guiMain->addSpacer();
        guiMain->addToggle("mirror x", &bMirrorX);
        guiMain->addToggle("mirror y", &bMirrorY);
        guiMain->addSpacer();
        guiMain->addLabel("Debug");
        guiMain->addToggle("draw blobs", &bDrawBlobs);
        guiMain->addToggle("draw video", &bDrawVideo);
    }
    guiMain->autoSizeToFitWidgets();
    
    // add gui to a c++ stl vector
    guis.push_back(guiMain);
    // add gui to a c++ stl map
    guihash["1"] = guiMain;
    
    ofAddListener(guiMain->newGUIEvent, this, &App::guiEvent);
}

//--------------------------------------------------------------
void App::setupGUITonic(){

    ofxUIScrollableCanvas *guiTonic = new ofxUIScrollableCanvas();
    guiTonic->setName("TONIC SEQUENCER");
    guiTonic->setFont("GUI/EnvyCodeR.ttf");
    guiTonic->addLabel("TONIC");
    guiTonic->addSpacer();
    
    // create some sliders to control those parameters
    vector<ControlParameter> synthParameters = synth.getParameters();
    
    // Add Tempo Slider
    ControlParameter &param = synthParameters.at(0);
    tonicTempo = param.getValue();
    guiTonic->addSlider("TEMPO", param.getMin(), param.getMax(), &tonicTempo);
    
    // Add Transpose Slider
    param = synthParameters.at(1);
    tonicTranspose = param.getValue();
    guiTonic->addSlider("TRANSPOSE", param.getMin(), param.getMax(), &tonicTranspose);
    
    guiTonic->addSpacer();
    
    // Pitch - Cutoff - Glide for every step
    int pitchStartIndex = 2;
    int cutoffStartIndex = 3;
    int glideStartIndex = 4;
    float rotarySize = 44;
    
    tonicPitches.assign(COLUMNS, float());
    tonicCutoffs.assign(COLUMNS, float());
    tonicGlides.assign(COLUMNS, float());
    
    // Pitch
    guiTonic->addLabel("PITCH");
    for (int i=0; i<tonicPitches.size(); i++) {
        param = synthParameters.at(pitchStartIndex + i * 3);
        guiTonic->addRotarySlider(param.getName(), param.getMin(), param.getMax(), &tonicPitches.at(i), rotarySize, rotarySize);
    }
    guiTonic->addSpacer();
    
    // Cutoff
    guiTonic->addLabel("CUTOFF");
    for (int i=0; i<tonicCutoffs.size(); i++) {
        param = synthParameters.at(cutoffStartIndex + i * 3);
        guiTonic->addMinimalSlider(param.getName(), param.getMin(), param.getMax(), &tonicCutoffs.at(i));
    }
    guiTonic->addSpacer();
    
    // Glide
    guiTonic->addLabel("GLIDE");
    for (int i=0; i<tonicGlides.size(); i++) {
        param = synthParameters.at(glideStartIndex + i * 3);
        guiTonic->addRotarySlider(param.getName(), param.getMin(), param.getMax(), &tonicGlides.at(i), rotarySize, rotarySize);
    }
    
    guiTonic->autoSizeToFitWidgets();
    
    // add gui to a c++ stl vector
    guis.push_back(guiTonic);
    // add gui to a c++ stl map
    guihash["2"] = guiTonic;
    
    ofAddListener(guiTonic->newGUIEvent, this, &App::guiEvent);
}

#ifdef USE_KINECT
//--------------------------------------------------------------
void App::setupGUIKinect(){
    ofxUICanvas *guiKinect = new ofxUICanvas();
    guiKinect->setName("Kinect");
    guiKinect->setFont("GUI/EnvyCodeR.ttf");
    guiKinect->setColorFill(ofxUIColor(200));
    guiKinect->setColorFillHighlight(ofxUIColor(255));
    guiKinect->setColorBack(ofxUIColor(20, 20, 20, 150));
    {
        guiKinect->addLabel("Kinect");
        guiKinect->addSpacer();
        guiKinect->addButton("Learn Background", &bLearnBackground);
        guiKinect->addToggle("Draw Point Cloud", &bDrawPointCloud);
        guiKinect->addToggle("Draw Blob IDs", &bDrawIDs);
        guiKinect->addSlider("tilt angle", -30.0f, 30.0f, &angle);
        guiKinect->addSpacer();
        guiKinect->addSlider("crop box min X", -20.0f, 20.0f, &cropBoxMin.x);
        guiKinect->addSlider("crop box min Y", 0.0f, 2.0f, &cropBoxMin.y);
        guiKinect->addSlider("crop box min Z", -20.0f, 20.0f, &cropBoxMin.z);
        guiKinect->addSpacer();
        guiKinect->addSlider("crop box max X", -20.0f, 20.0f, &cropBoxMax.x);
        guiKinect->addSlider("crop box max Y", 0.0f, 2.0f, &cropBoxMax.y);
        guiKinect->addSlider("crop box max Z", -20.0f, 20.0f, &cropBoxMax.z);
    }
    guiKinect->autoSizeToFitWidgets();
    
    // add gui to a c++ stl vector
    guis.push_back(guiKinect);
    // add gui to a c++ stl map
    guihash["3"] = guiKinect;
    
    ofAddListener(guiKinect->newGUIEvent, this, &App::guiEvent);
}
#endif


#ifdef USE_FLOB
//--------------------------------------------------------------
void App::setupGUIFlob(){
    ofxUICanvas *guiFlob = new ofxUICanvas();
    guiFlob->setName("Flob");
    guiFlob->setFont("GUI/EnvyCodeR.ttf");
    guiFlob->setColorFill(ofxUIColor(200));
    guiFlob->setColorFillHighlight(ofxUIColor(255));
    guiFlob->setColorBack(ofxUIColor(20, 20, 20, 150));
    {
        guiKinect->addLabel("Flob");
        guiKinect->addSpacer();
        guiFlob->addSlider("threshold", 1.0f, 80.0f, &threshold);
        guiFlob->addSlider("fade", 1.0f, 100.0f, &fade);
    }
    guiFlob->autoSizeToFitWidgets();
    
    // add gui to a c++ stl vector
    guis.push_back(guiFlob);
    // add gui to a c++ stl map
    guihash["4"] = guiFlob;
    
    ofAddListener(guiFlob->newGUIEvent, this, &App::guiEvent);
}
#endif

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
    
    vector<ControlParameter> synthParameters = synth.getParameters();

    ControlParameter *param;
    if (synthParameters.size()){
        
        // Tempo
        if (e.widget->getName() == "TEMPO"){
            param = &synthParameters.at(0);
            TonicFloat val = ofNormalize(tonicTempo, param->getMin(), param->getMax());
            param->setNormalizedValue(val);
            
            synth.forceNewOutput();
            sequencer->reset();
        }
        
        // Transpose
        if (e.widget->getName() == "TRANSPOSE"){
            param = &synthParameters.at(1);
            TonicFloat val = ofNormalize(tonicTranspose, param->getMin(), param->getMax());
            param->setNormalizedValue(val);
        }
        
        // Pitch - Cutoff - Glide for every step
        int pitchStartIndex = 2;
        int cutoffStartIndex = 3;
        int glideStartIndex = 4;
        
        // Pitch
        for (int i=0; i<tonicPitches.size(); i++) {
            param = &synthParameters.at(pitchStartIndex + i * 3);
            if (e.widget->getName() == param->getName()){
                TonicFloat pitchVal = ofNormalize(tonicPitches.at(i), param->getMin(), param->getMax());
                param->setNormalizedValue(pitchVal);
            }
        }
        
        // Cutoff
        for (int i=0; i<tonicCutoffs.size(); i++) {
            param = &synthParameters.at(cutoffStartIndex + i * 3);
            if (e.widget->getName() == param->getName()){
                TonicFloat cutoffVal = ofNormalize(tonicCutoffs.at(i), param->getMin(), param->getMax());
                param->setNormalizedValue(cutoffVal);
            }
        }
        
        // Glide
        for (int i=0; i<tonicGlides.size(); i++) {
            param = &synthParameters.at(glideStartIndex + i * 3);
            if (e.widget->getName() == param->getName()){
                TonicFloat glideVal = ofNormalize(tonicGlides.at(i), param->getMin(), param->getMax());
                param->setNormalizedValue(glideVal);
            }
        }
    }
    
#ifdef USE_KINECT
    if (e.widget->getName() == "tilt angle"){
        kinect.setCameraTiltAngle(angle);
    }
#endif
    
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
#ifdef USE_KINECT
        case '3':
        {
            // Hide all guis
            for (int i=0; i<guihash.size(); i++) {
                guihash[ofToString(i+1)]->setVisible(false);
            }
            guihash["3"]->toggleVisible();
        }
            break;
#endif
            
#ifdef USE_FLOB
        case '4':
        {
            // Hide all guis
            for (int i=0; i<guihash.size(); i++) {
                guihash[ofToString(i+1)]->setVisible(false);
            }
            guihash["4"]->toggleVisible();
        }
            break;
#endif
        
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
void App::resetScanRect(){
    scanRect.set(ofPoint(ofGetWidth() - camWidth - 20, 20), camWidth, camHeight);
}

//--------------------------------------------------------------
void App::exit(){
    
#ifdef USE_KINECT
	kinect.close();
#endif

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
////////////////////////////////////////////////////////////////
// Audio Stuff
////////////////////////////////////////////////////////////////
//--------------------------------------------------------------
void App::setupSynth(){
    
    // synth paramters are like instance variables -- they're values you can set later, by
    // calling synth.setParameter()
    ControlGenerator bpm = synth.addParameter("tempo", tonicTempo).min(16).max(96);
    ControlGenerator transpose = synth.addParameter("transpose", tonicTranspose).min(-6).max(6);
    
    // ControlMetro generates a "trigger" message at a given bpm. We multiply it by NOTE_MULTIPLIER because we
    // want four NOTE_MULTIPLIER * 4th notes for every beat
    ControlGenerator metro = ControlMetro().bpm(bpm * NOTE_MULTIPLIER);
    
    // ControlStepper increments a value every time it's triggered, and then starts at the beginning again
    // Here, we're using it to move forward in the sequence
    ControlGenerator step = ControlStepper().end(COLUMNS).trigger(metro);
    
    ofEvent<float> *stepEvent = synth.createOFEvent(step);
    ofAddListener(*stepEvent, sequencer, &Sequencer::stepEvent);
    
    // ControlSwitcher holds a list of ControlGenerators, and routes whichever one the inputIndex is pointing
    // to to its output.
    ControlSwitcher pitches = ControlSwitcher().inputIndex(step);
    ControlSwitcher cutoffs = ControlSwitcher().inputIndex(step);
    ControlSwitcher glides = ControlSwitcher().inputIndex(step);
    
    // stick a bunch of random values into the pitch and cutoff lists
    for(int i = 0; i < COLUMNS; i++){
        ControlGenerator pitchForThisStep = synth.addParameter("Step " + ofToString(i) + " Pitch", ofRandom(10, 80)).min(10).max(80);
        pitches.addInput(pitchForThisStep);
        
        ControlGenerator cutoff = synth.addParameter("Step " + ofToString(i) + " Cutoff", 240).min(0).max(1500);
        cutoffs.addInput(cutoff);
        
        ControlGenerator glide = synth.addParameter("Step " + ofToString(i) + " Glide", 0.02).min(0).max(0.1);
        glides.addInput(glide);
    }
    
    // Define a scale according to steps in a 12-note octave.
    // This is a pentatonic scale.
    // Like using just the black keys on a piano
    vector<float> scale;
    scale.push_back(0);
    scale.push_back(2);
    scale.push_back(3);
    scale.push_back(5);
    scale.push_back(7);
    scale.push_back(10);
    
    // ControlSnapToScale snaps a float value to the nearest scale value, no matter what octave its in
    ControlGenerator midiNote = transpose + ControlSnapToScale().setScale(scale).input(pitches);
    
    ControlGenerator frequencyInHertz = ControlMidiToFreq().input(midiNote);
    
    // now that we've done all that, we have a frequency signal that's changing 4x per beat
    Generator tone = RectWave().freq( frequencyInHertz.smoothed().length(glides) );
    
    // create an amplitude signal with an ADSR envelope, and scale it down a little so it's not scary loud
    Generator amplitude = ADSR(0.01, 0.1, 0,0).trigger(metro) * 0.3;
    
    // create a filter, and feed the cutoff sequence in to it
    LPF24 filter =  LPF24().cutoff(cutoffs).Q(0.1);
    filter.input(tone * amplitude);
    
    // rout the output of the filter to the synth's main output
    synth.setOutputGen(filter);
}

//--------------------------------------------------------------
void App::audioRequested(float * output, int bufferSize, int nChannels){
    synth.fillBufferOfFloats(output, bufferSize, nChannels);
}
