//
//  SequencerTrack.h
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 14.08.2013.
//
//

#pragma once

#include "ofMain.h"
#include "TrackCell.h"


class SequencerTrack {
    
    float                   trackHeight;
    int                     columns;
    int                     trackId;
    
public:
    ~SequencerTrack();
    
    void setup(int trackId, const ofRectangle &boundingBox, int columns, const ofColor &color);
    void update(int step);
    void draw();
    
    vector<TrackCell>   cells;
};
