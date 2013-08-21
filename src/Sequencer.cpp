//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "Sequencer.h"


//--------------------------------------------------------------
Sequencer::Sequencer(){
    
}

//--------------------------------------------------------------
Sequencer::~Sequencer(){
    tracks.clear();
}

//--------------------------------------------------------------
void Sequencer::setup(const ofRectangle rect, int columCount, int rowCount){
    
    diffTime = 0;
    
    columns = columCount;
    rows = rowCount;
    
    // Dimensions
    boundingBox.set(rect);
    
    stepButtonWidth = rect.getWidth() / columns;
    stepButtonHeight = rect.getHeight() / rows;
    
    // Clear and re-assign tracks
    tracks.clear();
    tracks.assign(rows, Track());
    
    // Create tracks
    for (int i = 0; i < rows; i++) {
        tracks[i].setup(i,
                        ofRectangle(rect.getX(), stepButtonHeight * i, rect.getWidth(), stepButtonHeight),
                        columns);
    }
    
    scrubber.set(0, 0, stepButtonWidth, rect.getHeight());
};

//--------------------------------------------------------------
void Sequencer::update(int step){
    
    // Update tracks
    for (int i=0; i<tracks.size(); i++){
        tracks[i].update(step);
    }
    
    // Move indicator line
    scrubber.setX(step * stepButtonWidth);
};

//--------------------------------------------------------------
void Sequencer::draw(){
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::grey);
    ofRect(0, 0, stepButtonWidth * columns, stepButtonHeight * rows);
    ofPopStyle();
    
    for (int i=0; i<tracks.size(); i++){
        tracks[i].draw();
    }
    
    ofPushMatrix();
    ofTranslate(scrubber.getTopLeft());
    ofPushStyle();
    
    ofSetColor(ofColor::greenYellow, 200);
    // Draw circle pins
    ofSetLineWidth(1.0);
    
    
    float endSize = 8.0f;
    ofPoint lStart(0, -endSize*.2);
    ofPoint lEnd(0, stepButtonHeight * rows + endSize*.2);
    
    lStart -= ofPoint(0, endSize);
    
    ofRect(lStart, stepButtonWidth, endSize);
    ofRect(lEnd, stepButtonWidth, endSize);
    
    ofFill();
    
    ofSetColor(ofColor::greenYellow, 10);
    ofRect(0, 0, scrubber.getWidth(), scrubber.getHeight());
    
    ofPopStyle();
    ofPopMatrix();
};

//--------------------------------------------------------------
void Sequencer::reset(){
    diffTime = 0;
}

//--------------------------------------------------------------
void Sequencer::toggle(int x, int y){
    for (int i=0; i<tracks.size(); i++){
        tracks[i].toggle(x, y);
    }
}

//--------------------------------------------------------------
void Sequencer::toggleIndex(int i, int j){
    float x = boundingBox.getX() + i * stepButtonWidth;
    float y = boundingBox.getY() + j * stepButtonHeight;
    
    for (int i=0; i<tracks.size(); i++){
        tracks[i].toggle(x, y);
    }
}

const ofRectangle &Sequencer::getBoundingBox(){
    return boundingBox;
}


//--------------------------------------------------------------
//void Sequencer::checkSegments(const vector<OSCPoint> &points){
//    // Reset segment touch states
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
//};

