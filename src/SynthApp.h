//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxTweener.h"
#include "ofxUI.h"
#include "Sequencer.h"

// Use Kinect to track blobs
//#define USE_KINECT

#ifdef USE_KINECT
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#endif

// Use ofxFlob Blob Tracker
//#define USE_FLOB

#ifdef USE_FLOB
#include "ofxFlob.h"
#endif

// Use OSC Controller
#define USE_OSC

#ifdef USE_OSC
#include "OSCPoint.h"
#define OSC_HOST            "192.168.1.103"
#define OSC_RECEIVE_PORT    8000
#define OSC_SEND_PORT       9000
#define OSC_POINT_COUNT     5
#endif

class SynthApp : public ofBaseApp {
    
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
    
    Sequencer           *sequencer;
    ofRectangle         scanRect;
    float               speed;
    
#ifdef USE_OSC
    // OSC
    ofxOscReceiver      oscReceiver;
    ofxOscSender        oscSender;
    
    vector <OSCPoint>   oscPoints;
#endif
    
#ifdef USE_KINECT    
    // OpenCV
    ofxCvColorImage     colorImg;       // color image
	ofxCvGrayscaleImage grayImage;      // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar;  // the far thresholded image
    
    ofxCvContourFinder  contourFinder;  // contour finder
    
    float               nearThreshold;
	float               farThreshold;
    float               angle;
    
    // Kinect
    ofxKinect           kinect;
#endif
    
#ifdef USE_FLOB
    ofVideoGrabber 		vidGrabber;
    
    // ofxFlob
    Flob                flob;
	vector<ABlob*>      *blobs;
    float               threshold;
    float               fade;
#endif
    
    int                 camWidth;
    int                 camHeight;
    bool                bMirrorX;
    bool                bMirrorY;
    bool                bDrawBlobs;
    bool                bDrawVideo;
    bool                bDebugMode;
    
    // GUI
    ofxUICanvas         *gui;
    float               columns;
    float               rows;
    bool                bInitGrid;
};
