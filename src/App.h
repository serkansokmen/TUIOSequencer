//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sškmen on 29.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxTweener.h"
#include "Sequencer.h"

// Use Kinect to track blobs
//#define USE_KINECT
// Use ofxFlob Blob Tracker
//#define USE_FLOB
// Use OSC Controller
#define USE_OSC


#ifdef USE_KINECT
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxKinectBlobFinder.h"
#include "ofxKinectBlobTracker.h"
#endif

#ifdef USE_FLOB
#include "ofxFlob.h"
#endif

#ifdef USE_OSC
#include "OSCPoint.h"
#define OSC_HOST            "192.168.1.100"
#define OSC_RECEIVE_PORT    8000
#define OSC_SEND_PORT       9000
#define OSC_POINT_COUNT     5
#endif

#define COLUMNS 6
#define ROWS    6


#ifdef USE_KINECT
class App : public ofBaseApp, public ofxKinectBlobListener {
#else
class App : public ofBaseApp {
#endif
    
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
#ifdef USE_KINECT
    void drawPointCloud();
    void blobOn(ofVec3f centroid, int id, int order);
    void blobMoved(ofVec3f centroid, int id, int order);
    void blobOff(ofVec3f centroid, int id, int order);
#endif
    
    void guiEvent(ofxUIEventArgs &e);
    void initGUI();
    
    Sequencer           *sequencer;
    ofRectangle         scanRect;
    float               sequencerBPM;
    
#ifdef USE_OSC
    // OSC
    ofxOscReceiver      oscReceiver;
    ofxOscSender        oscSender;
    
    vector <OSCPoint>   oscPoints;
#endif
    
#ifdef USE_KINECT
    
    // Kinect
    ofxKinect kinect;
    ofxKinectBlobFinder  blobFinder;
    ofxKinectBlobTracker blobTracker;
    
    // OpenCV
    ofxCvColorImage     colorImg;
    ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage bgImage;
    ofxCvGrayscaleImage grayDiff;
    
    ofImage             grayDiffOfImage;    // For ofxKinectBlobTracker
    
    ofVec3f             cropBoxMin;
    ofVec3f             cropBoxMax;
    ofVec3f             thresh3D;
    
    float               minBlobVol;
	float               maxBlobVol;
    float               angle;
    int                 numPixels;
    int                 minBlobPoints;
    int                 thresh2D;
    unsigned int        maxBlobs;
    
    bool                bDrawPointCloud;
	bool                bDrawIDs;
	bool                bLearnBackground;
    
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
    bool                bInitGrid;
    
    // GUI
    ofxUICanvas         *gui;
};
