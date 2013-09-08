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
    
    people.clear();
};

//--------------------------------------------------------------
void Sequencer::update(int step){
    
    currentStep = step;
    
    for (int trackIndex=0; trackIndex<tracks.size(); trackIndex++){

        tracks[trackIndex].update(step);
        
        for (int cellIndex=0; cellIndex<tracks[trackIndex].cells.size(); cellIndex++){
            
            TrackCell &cell = tracks[trackIndex].cells[cellIndex];
            
            cell.update();
            cell.setState(cellOff);
            
            for(int personIndex=0; personIndex<people.size(); personIndex++){
                ofPoint cursorPos(people[personIndex]->centroid.x * boundingBox.getWidth(), people[personIndex]->centroid.y * boundingBox.getHeight());
                
                if (cell.boundingBox.inside(cursorPos)){
                    if (step == cell.step){
                        cell.setState(cellOn);
                    } else {
                        cell.setState(cellActive);
                    }
                }
            }
        }
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
    people.clear();
}


