//
//  Sequencer.cpp
//  TUIOSequencer
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#include "Sequencer.h"

//--------------------------------------------------------------
void Sequencer::setup(const ofRectangle rect, int columCount, int rowCount){
    
    diffTime = 0;
    
    columns = columCount;
    rows = rowCount;
    
    // Dimensions
    boundingBox.set(rect);
    
    currentStep = 0;
    stepButtonWidth = rect.getWidth() / columns;
    stepButtonHeight = rect.getHeight() / rows;
    
    // Clear and re-assign tracks
    tracks.clear();
    tracks.assign(rows, SequencerTrack());
    
    // Create tracks
    for (int i = 0; i < rows; i++) {
        
        float val = ofMap(i, 0, rows, 0.0f, 255.0f);
        
        tracks[i].setup(i,
                        ofRectangle(rect.getX(), stepButtonHeight * i, rect.getWidth(), stepButtonHeight),
                        columns,
                        ofColor(val, 255, 255));
    }
};

//--------------------------------------------------------------
void Sequencer::update(int step){
    
    currentStep = step;
    
    // Update tracks
    for (int i=0; i<tracks.size(); i++){
        tracks[i].update(step);
    }
};

//--------------------------------------------------------------
void Sequencer::draw(){
    
    ofPushMatrix();
    ofPushStyle();
    
    // Draw ends
    float endSize = 20.0f;
    
    // Draw scrubber
    ofRectangle scrubberRect(currentStep * stepButtonWidth, 0, stepButtonWidth, stepButtonHeight * rows);
    ofRectangle scrubberRectTop(currentStep * stepButtonWidth, -24, stepButtonWidth, 20);
    ofRectangle scrubberRectBottom(currentStep * stepButtonWidth, stepButtonHeight * rows + 4, stepButtonWidth, 20);
    
    ofNoFill();
    ofSetColor(ofColor::greenYellow, 200);
    ofRect(scrubberRectTop);
    ofRect(scrubberRectBottom);
    
    ofFill();
    ofSetColor(ofColor::greenYellow, 100);
    ofRect(scrubberRect);
    
    ofPopStyle();
    ofPopMatrix();
    
    // Draw tracks
    for (int i=0; i<tracks.size(); i++){
        tracks[i].draw();
    }
};

//--------------------------------------------------------------
void Sequencer::clear(){
    tracks.clear();
    existingCursors.clear();
}

//--------------------------------------------------------------
void Sequencer::reset(){
    diffTime = 0;
    setup(boundingBox, columns, rows);
}

//--------------------------------------------------------------
void Sequencer::toggle(int x, int y){
    for (int i=0; i<tracks.size(); i++){
        tracks[i].toggle(x, y);
    }
}

//--------------------------------------------------------------
void Sequencer::randomize(float rate){
    
    reset();
    
    for (int r=0; r<(int)rate; r++) {
        int x = (int)ofRandom(0, boundingBox.getX() + boundingBox.getWidth());
        int y = (int)ofRandom(0, boundingBox.getY() + boundingBox.getHeight());
        
        toggle(x, y);
    }
}

//--------------------------------------------------------------
void Sequencer::refreshCells(){
    for (int i=0; i<tracks.size(); i++){
        tracks[i].offAll();
    }
    for (int i=0; i<tracks.size(); i++){
        vector<ofxTuioCursor *>::iterator it = existingCursors.begin();
        for(; it != existingCursors.end(); ++it){
            ofxTuioCursor *cursor = *it;
            tracks[i].on(cursor->getX() * boundingBox.getWidth(), cursor->getY() * boundingBox.getHeight());
        }
    }
}
