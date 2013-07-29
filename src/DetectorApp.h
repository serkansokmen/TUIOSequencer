#pragma once

#include "ofMain.h"
#include "ofxFlob.h"
#include "ofxUI.h"

class SegmentRectangle {
    
public:
    SegmentRectangle();
    SegmentRectangle(const ofRectangle &r){
        rect.set(r);
        bContainsBlob = false;
    }
    
    void checkIntersectsBlob(const ABlob &blob){
        
        bContainsBlob = ofRectangle(blob.bx, blob.by, blob.dimx, blob.dimy).intersects(rect);
        
        if (bContainsBlob){
            color = ofColor(ofColor::blue);
        } else {
            color = ofColor(ofColor::green);
        }
    };
    
    ofRectangle rect;
    bool        bContainsBlob;
    ofColor     color;
};

class DetectorApp : public ofBaseApp {
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void createGrid(int columns, int rows);
    
    void guiEvent(ofxUIEventArgs &e);
    void initGUI();
    
    vector<SegmentRectangle>    segments;
    ofRectangle                 scanRect;
    
    // used by the gui
    float   columns, rows;
    bool    bInitGrid;
    
    ofVideoGrabber 		vidGrabber;
    Flob                flob;
	vector<ABlob*>      *blobs;
    
    ofxUICanvas         *gui;
};
