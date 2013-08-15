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
    bpm = 120;
	step = 0;
    rTimer = 0;
	diffTime = 0;
}

//--------------------------------------------------------------
Sequencer::~Sequencer(){
    tracks.clear();
}


//--------------------------------------------------------------
void Sequencer::mouseDown(int x, int y){
    for (int i=0; i<tracks.size(); i++){
        tracks[i].mouseDown(x, y);
    }
}

//--------------------------------------------------------------
void Sequencer::setBPM(int _bpm){
    bpm = _bpm;
}


//--------------------------------------------------------------
void Sequencer::setup(const ofRectangle rect, int columCount, int rowCount){
    
    step = 0;
    
    columns = columCount;
    rows = rowCount;
    
    stepButtonWidth = rect.getWidth() / columns;
    stepButtonHeight = rect.getHeight() / rows;
    
    nTracks = soundBank.listDir("sounds");
    
    // Clear and re-assign tracks
    tracks.clear();
    tracks.assign(rows, Track());
    
    // Create tracks
    for (int y = 0; y < rows; y++) {
        tracks[y].setup(ofRectangle(rect.getX(), stepButtonHeight * y, rect.getWidth(), stepButtonHeight),
                        columns,
                        soundBank.getPath(y));
    }
    
    scannerRect.set(0, 0, stepButtonWidth, rect.getHeight());
};

//--------------------------------------------------------------
void Sequencer::update(){
    
    aTimer = ofGetElapsedTimeMillis();
	rTimer = aTimer - diffTime;
	
	float bpMillis;
	bpMillis = (bpm / 60.0f)*1000;
	float beatPulse;
	beatPulse = (60.0f / bpm/4) * 1000;
    
    if (rTimer > beatPulse){
		
        for(int i =0; i< nTracks; i++){
//			track[i].bPlayOnce = true;
		}
		
		diffTime = aTimer;
		step++;
		if(step == columns) step = 0;
	}
    
    for (int i=0; i<tracks.size(); i++){
        tracks[i].update(step);
    }
    
    // Move indicator line
    scannerRect.setX(step * stepButtonWidth);
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
    
    float endSize = 8.0f;
    ofPoint lStart(stepButtonWidth * (step + .5), -endSize*.2);
    ofPoint lEnd(stepButtonWidth * (step + .5), stepButtonHeight * rows + endSize*.2);
    
    lStart -= ofPoint(0, endSize);
    lEnd += ofPoint(0, endSize);
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor(ofColor::red);
    // Draw circle pins
    ofSetLineWidth(1.0);
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofRect(lStart, stepButtonWidth, endSize);
    ofRect(lEnd, stepButtonWidth, endSize);
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofFill();
    
    ofPopStyle();
    ofPopMatrix();
    
    ofSetColor(ofColor::green, 10);
    ofRect(scannerRect);
};

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
