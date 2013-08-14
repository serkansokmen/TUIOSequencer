//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "Sequencer.h"


//--------------------------------------------------------------
Sequencer::~Sequencer(){
    tracks.clear();
}


//--------------------------------------------------------------
void Sequencer::setup(const ofRectangle rect, int columCount, int rowCount, float speed, sequenceDirection dir){
    
    columns = columCount;
    rows = rowCount;
    
    stepButtonWidth = rect.getWidth() / columns;
    stepButtonHeight = rect.getHeight() / rows;
    
    trackSpeed = speed;
    direction = dir;
    
	// Clear and re-assign tracks
    tracks.clear();
    tracks.assign(rows, Track());
    
    // Create tracks
    for (int y = 0; y < rows; y++) {
        tracks[y].setup("sounds/rotate.mp3",
                    ofRectangle(rect.getX(), stepButtonHeight * y, rect.getWidth(), stepButtonHeight),
                    columns);
    }
    
    startScan(true);
};

//--------------------------------------------------------------
void Sequencer::startScan(bool fromBeginning){
    
    float lsx, lsy, lex, ley;
    float duration;
    float percentPassed = 0.0f;
    
    if (fromBeginning)
        Tweener.removeAllTweens();
    
    switch (direction) {
        case SEQ_DIRECTION_HORIZONTAL:
        {
            
            if (fromBeginning){
                lineStartPos.set(0, 0);
                lineEndPos.set(0, stepButtonHeight * rows);
            } else {
                percentPassed = ofNormalize(lineStartPos.x, 0, (stepButtonWidth * columns));
            }
            
            lsx = stepButtonWidth * columns;
            lsy = 0;
            lex = lsx;
            ley = stepButtonHeight * rows;
            duration = columns * trackSpeed * (1 - percentPassed);
            break;
        }
            
        case SEQ_DIRECTION_VERTICAL:
        {
            if (fromBeginning){
                lineStartPos.set(0, 0);
                lineEndPos.set(stepButtonWidth * columns, 0);
            } else {
                percentPassed = ofNormalize(lineStartPos.y, 0, (stepButtonHeight * rows));
            }
            
            lsx = 0;
            lsy = stepButtonHeight * rows;
            lex = stepButtonWidth * columns;
            ley = lsy;
            duration = rows * trackSpeed * (1 - percentPassed);
            break;
        }
            
        default:
            break;
    }
    
    duration *= 0.001;
    
    Tweener.addTween(lineStartPos.x, lsx, duration, &ofxTransitions::linear);
    Tweener.addTween(lineStartPos.y, lsy, duration, &ofxTransitions::linear);
    Tweener.addTween(lineEndPos.x, lex, duration, &ofxTransitions::linear);
    Tweener.addTween(lineEndPos.y, ley, duration, &ofxTransitions::linear);
}

//--------------------------------------------------------------
void Sequencer::update(){
    
    if ((direction == SEQ_DIRECTION_HORIZONTAL && lineStartPos.x >= int(stepButtonWidth * columns)) ||
        (direction == SEQ_DIRECTION_VERTICAL && lineStartPos.y >= int(stepButtonHeight * rows))) {
        startScan(true);
    }
    
    for (int i=0; i<tracks.size(); i++){
        tracks[i].update(16);
    }
};

//--------------------------------------------------------------
void Sequencer::draw(){
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::darkGreen);
    ofRect(0, 0, stepButtonWidth * columns, stepButtonHeight * rows);
    ofPopStyle();
    
    for (int i=0; i<tracks.size(); i++){
        tracks[i].draw();
    }
    
    ofPoint lStart = lineStartPos;
    ofPoint lEnd = lineEndPos;
    float endSize = 8.0f;
    
    switch (direction) {
        case SEQ_DIRECTION_HORIZONTAL:
        {
            lStart -= ofPoint(0, endSize);
            lEnd += ofPoint(0, endSize);
        }
            break;
            
        case SEQ_DIRECTION_VERTICAL:
        {
            lStart -= ofPoint(endSize, 0);
            lEnd += ofPoint(endSize, 0);
        }
            break;
            
        default:
            break;
    }
    
    ofPushMatrix();
    ofPushStyle();
    // Draw line
    ofSetLineWidth(4.0);
    ofSetColor(ofColor::darkRed);
    ofLine(lStart, lEnd);
    
    ofSetColor(ofColor::red);
    // Draw circle pins
    ofSetLineWidth(1.0);
    ofCircle(lStart, endSize * .4);
    ofCircle(lEnd, endSize * .4);
    ofFill();
    
    ofPopStyle();
    ofPopMatrix();
};

//--------------------------------------------------------------
void Sequencer::setSpeed(float speed){
    trackSpeed = speed;
    startScan(false);
}

//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<ABlob *> *blobs){
    // Reset segment touch states
//    vector<GridSegment>::iterator segment;
//    for (segment = segments.begin(); segment != segments.end(); segment++){
//        segment->setState(off);
//    }
//    
//    for (int i=0; i<segments.size(); i++){
//        GridSegment *segmentPtr = &segments[i];
//        
//        for(int i=0; i<blobs->size(); i++){
//            ABlob &aBlob = *(blobs->at(i));
//            ofRectangle blobRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
//            if (segmentPtr->boundingBox.inside(blobRect.getCenter()) &&
//                segmentPtr->boundingBox.intersects(lineStartPos, lineEndPos)){
//                segmentPtr->setState(on);
//            }
//        }
//    }
}

//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<OSCPoint> &points){
    // Reset segment touch states
//    vector<GridSegment>::iterator segment;
//    for (segment = segments.begin(); segment != segments.end(); segment++){
//        segment->setState(off);
//    }
//    
//    if (points.size() > 0){
//        for (int i=0; i<segments.size(); i++){
//            GridSegment *segmentPtr = &segments[i];
//            
//            for(int i=0; i<points.size(); i++){
//                segmentPtr->setState(active);
//                if (segmentPtr->boundingBox.inside(ofVec2f(points[i].position)) &&
//                    segmentPtr->boundingBox.intersects(lineStartPos, lineEndPos)){
//                    segmentPtr->setState(on);
//                }
//            }
//        }
//    }
};

//
////--------------------------------------------------------------
//void Sequencer::segmentOn(int x, int y){
//    int i = y * columns + x;
//    segments[i].bTouchesBlob = true;
//}
//
////--------------------------------------------------------------
//void Sequencer::segmentOff(int x, int y){
//    int i = y * columns + x;
//    segments[i].bTouchesBlob = false;
//}
