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
    
    rectW = rect.getWidth() / columns;
    rectH = rect.getHeight() / rows;
    
    trackSpeed = speed;
    direction = dir;
    
	// Clear segments vector
    segments.clear();
    
    // Create segments
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < columns; x++) {
            
            int index = y + x * columns;
            
            ofRectangle rect;
            rect.setPosition(x * rectW, y * rectH);
            rect.setWidth(rectW);
            rect.setHeight(rectH);
            
            GridSegment seg;
            seg.setup(rect, index, 12.0f);
            
            segments.push_back(seg);
        }
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
                lineEndPos.set(0, rectH * rows);
            } else {
                percentPassed = ofNormalize(lineStartPos.x, 0, (rectW * columns));
            }
            
            lsx = rectW * columns;
            lsy = 0;
            lex = lsx;
            ley = rectH * rows;
            duration = columns * trackSpeed * (1 - percentPassed);
            break;
        }
            
        case SEQ_DIRECTION_VERTICAL:
        {
            if (fromBeginning){
                lineStartPos.set(0, 0);
                lineEndPos.set(rectW * columns, 0);
            } else {
                percentPassed = ofNormalize(lineStartPos.y, 0, (rectH * rows));
            }
            
            lsx = 0;
            lsy = rectH * rows;
            lex = rectW * columns;
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
    
    if ((direction == SEQ_DIRECTION_HORIZONTAL && lineStartPos.x >= int(rectW * columns)) ||
        (direction == SEQ_DIRECTION_VERTICAL && lineStartPos.y >= int(rectH * rows))) {
        startScan(true);
    }
};

//--------------------------------------------------------------
void Sequencer::draw(){
    
//    ofPushStyle();
//    ofNoFill();
//    ofSetColor(ofColor::darkGreen);
//    ofRectRounded(0, 0, rectW * columns, rectH * rows, 12);
//    ofPopStyle();
    
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->draw();
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
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->setState(off);
    }
    
    for (int i=0; i<segments.size(); i++){
        GridSegment *segmentPtr = &segments[i];
        
        for(int i=0; i<blobs->size(); i++){
            ABlob &aBlob = *(blobs->at(i));
            ofRectangle blobRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
            if (segmentPtr->boundingBox.inside(blobRect.getCenter()) &&
                segmentPtr->boundingBox.intersects(lineStartPos, lineEndPos)){
                segmentPtr->setState(on);
            }
        }
    }
}

//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<OSCPoint> &points){
    // Reset segment touch states
    vector<GridSegment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->setState(off);
    }
    
    if (points.size() > 0){
        for (int i=0; i<segments.size(); i++){
            GridSegment *segmentPtr = &segments[i];
            
            for(int i=0; i<points.size(); i++){
                if (segmentPtr->boundingBox.inside(ofVec2f(points[i].position)) &&
                    segmentPtr->boundingBox.intersects(lineStartPos, lineEndPos)){
                    segmentPtr->setState(on);
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
        
        if (segmentPtr->boundingBox.inside(x, y)) {
//            segmentPtr->bTouchesBlob = !segmentPtr->bTouchesBlob;
        }
    }
}
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
