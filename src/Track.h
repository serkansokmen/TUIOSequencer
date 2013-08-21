//
//  Track.h
//  MotionSynth
//
//  Created by Serkan Sokmen on 14.08.2013.
//
//

#pragma once

#include "ofMain.h"
#include "TrackCell.h"


class Track {
    
    float                   trackHeight;
    int                     columns;
    int                     trackId;
    
    vector<TrackCell>       cells;
    
public:
    ~Track();
    
    void setup(int trackId, const ofRectangle &boundingBox, int columns);
    void update(int step);
    void draw();
    void toggle(int x, int y);
    
    vector<bool>    cellStates;
};
