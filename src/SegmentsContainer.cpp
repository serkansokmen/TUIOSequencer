//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "SegmentsContainer.h"


//--------------------------------------------------------------
void SegmentsContainer::update(){
    vector<Segment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->update();
    }
};

//--------------------------------------------------------------
void SegmentsContainer::draw(){
    vector<Segment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->draw();
    }
};

//--------------------------------------------------------------
void SegmentsContainer::initGrid(const ofRectangle rect, int columns, int rows){
    
    segments.clear();
    
    float rectW = rect.getWidth() / rows;
    float rectH = rect.getHeight() / columns;
    
    for (int y = 0; y < columns; y++) {
        for (int x = 0; x < rows; x++) {
            
            int index = y + x * rows;
            
            ofRectangle rect;
            rect.setPosition(x * rectW, y * rectH);
            rect.setWidth(rectW);
            rect.setHeight(rectH);
            
            Segment seg(rect);
            seg.segmentId = index;
            segments.push_back(seg);
        }
    }
};

//--------------------------------------------------------------
void SegmentsContainer::checkSegments(const vector<ofxCvBlob> &blobs){
    // Reset segment touch states
    vector<Segment>::iterator segment;
    for (segment = segments.begin(); segment != segments.end(); segment++){
        segment->bTouchesBlob = false;
    }
    
    if (blobs.size() > 0){
        for (int i=0; i<segments.size(); i++){
            Segment *segmentPtr = &segments[i];
            
            for(int i=0; i<blobs.size(); i++){
                if (blobs[i].boundingRect.intersects(segmentPtr->rect)){
                    segmentPtr->bTouchesBlob = true;
                }
            }
        }
    }
};

//--------------------------------------------------------------
void SegmentsContainer::checkSegments(const vector<ABlob *> *blobs){
    for (int i=0; i<segments.size(); i++){
        Segment *segmentPtr = &segments[i];
        
        for(int i=0; i<blobs->size(); i++){
            ABlob &aBlob = *(blobs->at(i));
            ofRectangle blobRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
            if (blobRect.intersects(segmentPtr->rect)){
                segmentPtr->bTouchesBlob = true;
            }
        }
    }
}