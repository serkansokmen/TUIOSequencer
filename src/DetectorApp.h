#pragma once

#include "ofMain.h"
#include "ofxFlob.h"
#include "ofxUI.h"
#include "SegmentRectangle.h"

using namespace std;

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
    bool    bDrawVideo;
    
    ofVideoGrabber 		vidGrabber;
    Flob                flob;
	vector<ABlob*>      *blobs;
    float               threshold;
    float               fade;
    bool                bDrawBlobs;
    
    ofxUICanvas         *gui;
};
