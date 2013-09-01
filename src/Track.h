//
//  Track.h
//  TUIOSequencer
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
    
public:
    ~Track();
    
    void setup(int trackId, const ofRectangle &boundingBox, int columns, const ofColor &color);
    void update(int step);
    void draw();
    
    void on(int x, int y);
    void offAll();
    void toggle(int x, int y);
    
    vector<TrackCell>   cells;
    vector<bool>        cellStates;
    bool                bIsReady = false;
};
