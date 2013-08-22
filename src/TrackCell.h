//
//  TrackCell.h
//  BodySequencerApp
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


enum TrackCellState {
    off = TRACK_CELL_OFF,
    active = TRACK_CELL_ACTIVE,
    on = TRACK_CELL_ON
};


class TrackCell {
    
    ofRectangle     outerBox;
    ofRectangle     innerBox;
    
    TrackCellState  state;
    float           hue, saturation, brightness;
    ofColor         color;
    float           alpha;
    
public:
    void setup(const ofRectangle &boundingBox, int step, const ofColor &color);
    void update();
    void draw();
    void toggle(int x, int y);
    
    void setState(TrackCellState s);
    const TrackCellState &getState();
    
    void setColor(ofColor c);
    ofColor &getColor();
    
    ofRectangle     boundingBox;
    int             step;
};
