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
    cellStates.clear();
    
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
    
    cellStates.assign(columns, bool(false));
    bIsReady = true;
}

//--------------------------------------------------------------
void SequencerTrack::update(int step){
    
    for (int i=0; i<cells.size(); i++){
        
        cells[i].update();
        
        if (step == cells[i].step){
            if (cells[i].getState() == cellActive){
                cells[i].setState(cellOn);
            }
        } else {
            if (cells[i].getState() == cellOn){
                cells[i].setState(cellActive);
            }
        }
    }
    
    for (int i=0; i<cellStates.size(); i++) {
        cellStates[i] = (cells[i].getState() != cellOff);
    }
}

//--------------------------------------------------------------
void SequencerTrack::draw(){
	for(int i=0; i<cells.size(); i++){
        cells[i].draw();
	}
}

//--------------------------------------------------------------
void SequencerTrack::on(int x, int y){
	for(int i=0; i<cells.size(); i++){
		cells[i].on(x, y);
	}
}

//--------------------------------------------------------------
void SequencerTrack::offAll(){
	for(int i=0; i<cells.size(); i++){
		cells[i].setState(cellOff);
	}
}

//--------------------------------------------------------------
void SequencerTrack::toggle(int x, int y){
	for(int i=0; i<cells.size(); i++){
		cells[i].toggle(x, y);
	}
}
