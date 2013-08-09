//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "Sequencer.h"


//--------------------------------------------------------------
void Sequencer::setup(const ofRectangle rect, int columCount, int rowCount, float speed, sequenceDirection dir){
    
    columns = columCount;
    rows = rowCount;
    
    rectW = rect.getWidth() / rows;
    rectH = rect.getHeight() / columns;
    
    trackSpeed = speed;
    direction = dir;
    
	// Clear segments vector
    segments.clear();
    
    // Create segments
    for (int y = 0; y < columns; y++) {
        for (int x = 0; x < rows; x++) {
            
            int index = y + x * rows;
            
            ofRectangle rect;
            rect.setPosition(x * rectW, y * rectH);
            rect.setWidth(rectW);
            rect.setHeight(rectH);
            
            GridSegment seg;
            seg.setup(rect, index);
            
            segments.push_back(seg);
        }
    }
    
    // Init timer
    initTimer();
};

//--------------------------------------------------------------
void Sequencer::update(){
    
    // update the timer this frame
    float timer = ofGetElapsedTimeMillis() - startTime;
    // get the percantage of the timer
    float percentage = ofMap(timer, 0.0, endTime, 0.0, 1.0, true);
    
    if(timer >= endTime) {
        initTimer();
    }
    
    switch (direction) {
        case SEQ_DIRECTION_HORIZONTAL:
        {
            lineStartPos.x = rectW * rows * percentage;
            lineStartPos.y = 0;
            lineEndPos.x = lineStartPos.x;
            lineEndPos.y = rectH * columns;
        }
            break;
            
        case SEQ_DIRECTION_VERTICAL:
        {
            lineStartPos.x = 0;
            lineStartPos.y = rectH * columns * percentage;
            lineEndPos.x = rectW * rows;
            lineEndPos.y = lineStartPos.y;
        }
            break;
            
        default:
            break;
    }
};

//--------------------------------------------------------------
void Sequencer::draw(){
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->draw();
    }
    
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255, 0, 0);
    // TODO: draw a prettier track line
    ofLine(lineStartPos, lineEndPos);
    ofPopStyle();
    ofPopMatrix();
};

//--------------------------------------------------------------
void Sequencer::setSpeed(float speed){
    trackSpeed = speed;
}

//--------------------------------------------------------------
void Sequencer::initTimer(){
    startTime = ofGetElapsedTimeMillis();   // get the start time
    switch (direction) {
        case SEQ_DIRECTION_HORIZONTAL:
            endTime = rows * trackSpeed;          // in milliseconds
            break;
            
        case SEQ_DIRECTION_VERTICAL:
            endTime = columns * trackSpeed;       // in milliseconds
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<ofxCvBlob> &blobs){
    // Reset segment touch states
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->bTouchesBlob = false;
    }
    
    if (blobs.size() > 0){
        for (int i=0; i<segments.size(); i++){
            GridSegment *segmentPtr = &segments[i];
            
            for(int i=0; i<blobs.size(); i++){
                if (segmentPtr->rect.inside(blobs[i].boundingRect.getCenter()) && segmentPtr->rect.intersects(lineStartPos, lineEndPos)){
                    segmentPtr->bTouchesBlob = true;
                }
            }
        }
    }
};

//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<ABlob *> *blobs){
    // Reset segment touch states
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->bTouchesBlob = false;
    }
    
    for (int i=0; i<segments.size(); i++){
        GridSegment *segmentPtr = &segments[i];
        
        for(int i=0; i<blobs->size(); i++){
            ABlob &aBlob = *(blobs->at(i));
            ofRectangle blobRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
            if (segmentPtr->rect.inside(blobRect.getCenter()) && segmentPtr->rect.intersects(lineStartPos, lineEndPos)){
                segmentPtr->bTouchesBlob = true;
            }
        }
    }
}

//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<OSCPoint> &points){
    // Reset segment touch states
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->bTouchesBlob = false;
    }
    
    if (points.size() > 0){
        for (int i=0; i<segments.size(); i++){
            GridSegment *segmentPtr = &segments[i];
            
            for(int i=0; i<points.size(); i++){
                if (segmentPtr->rect.inside(ofVec2f(points[i].position)) &&
                    segmentPtr->rect.intersects(lineStartPos, lineEndPos) &&
                    points[i].alpha > 0){
                    segmentPtr->bTouchesBlob = true;
                }
            }
        }
    }
};

//--------------------------------------------------------------
void Sequencer::toggleSegment(int x, int y){
    vector<GridSegment>::iterator segment;
    for (int i=0; i<segments.size(); i++){
        GridSegment *segmentPtr = &segments[i];
        
        if (segmentPtr->rect.inside(x, y)) {
            segmentPtr->bTouchesBlob = !segmentPtr->bTouchesBlob;
        }
    }
}

//--------------------------------------------------------------
void Sequencer::segmentOn(int x, int y){
    int i = y * columns + x;
    segments[i].bTouchesBlob = true;
}

//--------------------------------------------------------------
void Sequencer::segmentOff(int x, int y){
    int i = y * columns + x;
    segments[i].bTouchesBlob = false;
}


//--------------------------------------------------------------

//void Sequencer::sendGridStateViaOSC(){
//    
//    for (int y=0; y<columns; y++) {
//        for (int x=0; x<rows; x++) {
//            
//            int index = y + x * rows;
//            GridSegment *segment = &segments[index];
//            
//            ofxOscMessage m;
//            string address = "/grid/toggle_" + ofToString(y+1) + "_" + ofToString(x+1);
//            cout << address << endl;
//            m.setAddress(address);
//            
//            if (segment->bTouchesBlob){
//                m.addIntArg(1);
//            } else {
//                m.addIntArg(0);
//            }
//            
//            oscSender.sendMessage(m);
//        }
//    }
//}
