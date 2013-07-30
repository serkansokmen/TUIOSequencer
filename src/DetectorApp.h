#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxFlob.h"
#include "ofxKinect.h"
#include "ofxUI.h"
#include "GridSegment.h"

using namespace std;

#define USE_KINECT

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
    
    // Creates segmented grid
    void createGrid(int columns, int rows);
    
    void guiEvent(ofxUIEventArgs &e);
    void initGUI();
    
    vector<GridSegment>     segments;
    ofRectangle             scanRect;
    float                   columns;
    float                   rows;
    
#ifdef USE_KINECT

    // OpenCV
	ofxCvGrayscaleImage grayImage;      // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar;  // the far thresholded image
    
    ofxCvContourFinder  contourFinder;  // contour finder
    
    float               nearThreshold;
	float               farThreshold;
    float               angle;
    
    // Kinect
    ofxKinect           kinect;
#else
    ofVideoGrabber 		vidGrabber;
    int                 camWidth;
    int                 camHeight;
    
    // ofxFlob
    Flob                flob;
	vector<ABlob*>      *blobs;
    float               threshold;
    float               fade;
#endif
    
    // GUI
    ofxUICanvas         *gui;
    bool                bInitGrid;
    bool                bMirrorX;
    bool                bMirrorY;
    bool                bDrawBlobs;
    bool                bDrawVideo;
};
