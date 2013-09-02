//
//  TrackCell.h
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"
#include "ofxTweener.h"


#define TRACK_CELL_OFF       0
#define TRACK_CELL_ACTIVE    1
#define TRACK_CELL_ON        2

#define INNER_STROKE_COUNT   8


enum TrackCellState {
    cellOff = TRACK_CELL_OFF,
    cellActive = TRACK_CELL_ACTIVE,
    cellOn = TRACK_CELL_ON
};


class TrackCell {
    
    ofRectangle     outerBox;
    ofRectangle     innerBox;
    ofSoundPlayer   soundPlayer;
    
    TrackCellState  state;
    float           hue, saturation, brightness;
    ofColor         color;
    float           alpha;
    
    vector<float>   rectSizes;
    
public:
    void setup(const ofRectangle &boundingBox, int step, const ofColor &color);
    void update();
    void draw();
    void on(int x, int y);
    void off(int x, int y);
    void toggle(int x, int y);
    void loadSound(string path);
    void play();
    
    void setState(TrackCellState s);
    const TrackCellState &getState();
    
    void setColor(ofColor c);
    ofColor &getColor();
    
    ofRectangle     boundingBox;
    string          soundPath;
    int             step;
};
