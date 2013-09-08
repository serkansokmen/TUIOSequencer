//
//  SequencerTrack.cpp
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 14.08.2013.
//
//

#include "SequencerTrack.h"


//--------------------------------------------------------------
SequencerTrack::~SequencerTrack(){
    cells.clear();
}


//--------------------------------------------------------------
void SequencerTrack::setup(int id, const ofRectangle &bb, int cols, const ofColor &color){
    
    trackId = id;
    columns = cols;
    
	float buttonWidth = bb.getWidth() / cols;
    trackHeight = bb.getHeight();
	
    cells.assign(cols, TrackCell());
	for(int i = 0; i<columns; i++){
        cells[i].setup(ofRectangle(buttonWidth * i, bb.getY(), buttonWidth, trackHeight), i, color);
	}
}

//--------------------------------------------------------------
void SequencerTrack::update(int step){
    for (int i=0; i<cells.size(); i++){
        cells[i].update();
    }
}

//--------------------------------------------------------------
void SequencerTrack::draw(){
	for(int i=0; i<cells.size(); i++){
        cells[i].draw();
	}
}
