#include "DetectorApp.h"

int camWidth = 320;
int camHeight = 240;

//--------------------------------------------------------------
void DetectorApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLineWidth(0.1);
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetScreenWidth() * .5, ofGetScreenHeight() * .5), camWidth * 2.5, camHeight * 2.5);
    
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(camWidth, camHeight);
    vidGrabber.setVerbose(true);
    
    initGUI();
    
    blobs = NULL;
    flob.setup(camWidth, camHeight, scanRect.getWidth(), scanRect.getHeight());
	flob.setOm(Flob::CONTINUOUS_DIFFERENCE)
        ->setColormode(Flob::LUMA601)
        ->setFade(fade)
        ->setThresh(threshold)
        ->setThresholdmode(Flob::ABSDIF)
        ->setMirror(true, false);
    
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
    
    vidGrabber.update();
    
	if (vidGrabber.isFrameNew()){
        blobs = flob.calc(flob.binarize(vidGrabber.getPixels(), camWidth, camHeight));
	}
    
    vector<SegmentRectangle>::iterator segment;
    if (blobs != NULL && blobs->size() > 0){
        for(int i=0; i<blobs->size();i++){
            ABlob &aBlob = *(blobs->at(i));
            
            for (segment = segments.begin(); segment != segments.end(); segment++){
                segment->checkIntersectsBlob(aBlob);
            }
        }
    }
}

//--------------------------------------------------------------
void DetectorApp::draw(){
    
    // Draw scan rect
    ofPushMatrix();
    ofTranslate(scanRect.getPositionRef());
    
    if (bDrawVideo) {
        ofSetColor(ofColor::white);
        flob.videotex->draw(0, 0, scanRect.getWidth(), scanRect.getHeight());
    }
    
    vector<SegmentRectangle>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->draw();
    }
    
    if (blobs != NULL && blobs->size() > 0 && bDrawBlobs){
		for(int i=0; i<blobs->size();i++){
			ABlob &aBlob = *(blobs->at(i));
			ofSetColor(ofColor::pink, 100);
			ofRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
			ofSetColor(ofColor::plum, 200);
			ofRect(aBlob.cx, aBlob.cy, 10, 10);
		}
	}
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
            
            segments.push_back(SegmentRectangle(rect));
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
    gui->addToggle("init grid", &bInitGrid);
    
    gui->addSpacer();
    gui->addFPSSlider("fps");
    
    gui->autoSizeToFitWidgets();
    
    gui->addToggle("mirror x", true);
    gui->addSlider("threshold", 1.0f, 80.0f, &threshold);
    gui->addSlider("fade", 1.0f, 100.0f, &fade);
    gui->addToggle("draw video", &bDrawVideo);
    gui->addToggle("draw blobs", &bDrawBlobs);
    gui->addSpacer();
    
    gui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent, this, &DetectorApp::guiEvent);
    
    gui->setPosition(10, 10);
    
    gui->loadSettings("GUI/guiSettings.xml");
    gui->setVisible(true);
}

//--------------------------------------------------------------
void DetectorApp::guiEvent(ofxUIEventArgs &e){
    if (e.widget->getName() == "mirror x") {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        flob.setMirror(toggle->getValue(), false);
    }
    
    if (e.widget->getName() == "threshold")
        flob.setThresh(threshold);
    
    if (e.widget->getName() == "fade")
        flob.setFade(fade);
}

//--------------------------------------------------------------
void DetectorApp::keyPressed(int key){
    if (key=='s') {
        gui->toggleVisible();
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
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}