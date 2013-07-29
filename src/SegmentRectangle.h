//
//  SegmentRectangle.h
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 29.07.2013.
//
//

#ifndef PresenceDetectorGrid_SegmentRectangle_h
#define PresenceDetectorGrid_SegmentRectangle_h

class SegmentRectangle {
    
public:
    SegmentRectangle();
    SegmentRectangle(const ofRectangle &r){
        rect.set(r);
        bContainsBlob = false;
    }
    
    void checkIntersectsBlob(const ABlob &blob){
        
        ofRectangle *blobRect = new ofRectangle(blob.bx, blob.by, blob.dimx, blob.dimy);
        bContainsBlob = blobRect->intersects(rect);
        
        if (bContainsBlob){
            color = ofColor(ofColor::blue);
        } else {
            color = ofColor(ofColor::green);
        }
        
        delete blobRect;
    };
    
    void draw(){
        ofPushStyle();
        if (bContainsBlob) {
            ofFill();
            ofSetColor(color, 200);
        } else {
            ofNoFill();
            ofSetColor(color, 50);
        }
        ofRect(rect);
        ofPopStyle();
    };
    
    ofRectangle rect;
    bool        bContainsBlob;
    ofColor     color;
};


#endif
