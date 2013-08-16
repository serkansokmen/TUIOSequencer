//
//  TrackCell.h
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#pragma once

#include "ofMain.h"


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
    
    TrackCellState state;
    
public:
    void setup(const ofRectangle &boundingBox, int step);
    void draw();
    void toggle(int x, int y);
    
    void setState(TrackCellState s);
    const TrackCellState &getState();
    
    ofRectangle     boundingBox;
    int             step;
};
