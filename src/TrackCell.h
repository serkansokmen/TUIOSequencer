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

#define INNER_STROKE_COUNT   8


enum TrackCellState {
    cellOff,
    cellActive,
    cellOn,
};


class TrackCell {
    
    ofRectangle     outerBox;
    ofRectangle     innerBox;
    
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
    
    void setState(TrackCellState s);
    const TrackCellState &getState();
    
    void    setColor(ofColor c);
    ofColor &getColor();
    
    ofRectangle     boundingBox;
    int             step;
};
