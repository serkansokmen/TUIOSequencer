#include "DetectorApp.h"

int camWidth = 320;
int camHeight = 240;

//--------------------------------------------------------------
void DetectorApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(ofColor::black);
    ofEnableAlphaBlending();
    
    // Center app window
    ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) * .5,
                        (ofGetScreenHeight() - ofGetHeight()) * .5);
    
    
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(camWidth, camHeight);
    vidGrabber.setVerbose(true);
    
    // Create scan grid
//    columns = 3.0;
//    rows = 3.0;
    bInitGrid = false;
    createGrid((int)columns, (int)rows);
    
    blobs = NULL;

    flob.setup(camWidth, camHeight, scanRect.getWidth(), scanRect.getHeight());
	flob.setOm(Flob::CONTINUOUS_DIFFERENCE)
                ->setColormode(Flob::LUMA601)
                ->setFade(25)->setThresh(12)
                ->setThresholdmode(Flob::ABSDIF)
                ->setMirror(true, false);
    
    initGUI();
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

        if (blobs != NULL){
            for(int i=0; i<blobs->size();i++){
                ABlob &aBlob = *(blobs->at(i));
                
                std::vector<SegmentRectangle>::iterator segment;
                for (segment = segments.begin(); segment != segments.end(); segment++){
                    segment->checkIntersectsBlob(aBlob);
                }
            }
        }
	}
}

//--------------------------------------------------------------
void DetectorApp::draw(){
    
    // Draw scan rect
    ofPushMatrix();
    ofTranslate(scanRect.getPositionRef());
    
    ofPushStyle();
    ofSetColor(ofColor::whiteSmoke);
    ofNoFill();
    ofRect(0, 0, scanRect.getWidth(), scanRect.getHeight());
    ofPopStyle();
    
	std::vector<SegmentRectangle>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        ofPushStyle();
        ofNoFill();
        ofSetColor(segment->color);
        if (segment->bContainsBlob) {
            ofFill();
        } else {
            ofNoFill();
        }
        ofRect(segment->rect);
        ofPopStyle();
    }
    
    if (blobs != NULL){
		for(int i=0; i<blobs->size();i++){
			ABlob &aBlob = *(blobs->at(i));
			ofSetColor(0,0,255,100);
			ofRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
			ofSetColor(0,255,0,200);
			ofRect(aBlob.cx, aBlob.cy, 10, 10);
		}
	}
    ofPopMatrix();
    
    if (gui->isVisible()) {
        gui->draw();
        flob.videotexmotion.draw(10, 240, camWidth/2, camHeight/2);
    }
}

//--------------------------------------------------------------
void DetectorApp::createGrid(int columns, int rows){
    
    segments.clear();
    
    // Init scan rect
    scanRect.setFromCenter(ofPoint(ofGetWidth()/2, ofGetHeight()/2), camWidth * 2, camHeight * 2);
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
    gui->addLabel("PRESENCE DETECTOR GRID");
    gui->addSpacer();
    gui->addSlider("columns", 1.0f, 10.0f, &columns);
    gui->addSlider("rows", 1.0f, 10.0f, &rows);
    gui->addToggle("init grid", &bInitGrid);
    
    gui->addSpacer();
    gui->addFPSSlider("fps");
    
    gui->autoSizeToFitWidgets();
    
    gui->addToggle("mirror x", true);
    gui->addSpacer();
    
    gui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent, this, &DetectorApp::guiEvent);
    
    gui->loadSettings("GUI/guiSettings.xml");
    gui->setVisible(true);
}

//--------------------------------------------------------------
void DetectorApp::guiEvent(ofxUIEventArgs &e){
    if (e.widget->getName() == "mirror x") {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        flob.setMirror(toggle->getValue(), false);
    }
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