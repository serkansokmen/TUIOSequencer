//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxFlob.h"
#include "ofxKinect.h"
#include "ofxUI.h"
#include "SegmentsContainer.h"


// Comment to use video camera input
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
    
    void guiEvent(ofxUIEventArgs &e);
    void initGUI();
    
    SegmentsContainer       *segmentsContainer;
    ofRectangle             scanRect;
    
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
    float               columns;
    float               rows;
    bool                bInitGrid;
    bool                bMirrorX;
    bool                bMirrorY;
    bool                bDrawBlobs;
    bool                bDrawVideo;
};
