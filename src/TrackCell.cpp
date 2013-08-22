//
//  TrackCell.cpp
//  MotionSynth
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#include "TrackCell.h"


//--------------------------------------------------------------
void TrackCell::setup(const ofRectangle &bb, int s, const ofColor &c){
    
    float padding = 8.0f;
    
    state = off;
    step = s;
    
    red = 255.0;
    green = 255.0;
    blue = 255.0;
    
    setColor(c);
    
    boundingBox.set(bb);
    outerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding*.2, boundingBox.getHeight() - padding*.2);
    innerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding, boundingBox.getHeight() - padding);
}

//--------------------------------------------------------------
void TrackCell::draw(){
    
    ofPushMatrix();
    ofPushStyle();
    ofSetLineWidth(2.0);
    
    float roundedRadius = innerBox.getArea() * 0.01f;
    cout << roundedRadius << endl;
    
    switch (state) {
        case off:
//            ofSetColor(ofColor::white);
//            ofNoFill();
//            ofRect(outerBox);
            ofSetColor(ofColor(red, green, blue), 50);
            ofFill();
//            ofRect(innerBox);
            ofRectRounded(innerBox, roundedRadius);
            break;
        case active:
            ofSetColor(ofColor(red, green, blue));
            ofNoFill();
//            ofRect(outerBox);
            ofRectRounded(outerBox, roundedRadius);
            ofSetColor(ofColor(red, green, blue), 150);
            ofFill();
//            ofRect(innerBox);
            ofRectRounded(innerBox, roundedRadius);
            break;
        case on:
            ofSetColor(ofColor(red, green, blue));
            ofNoFill();
//            ofRect(outerBox);
            ofRectRounded(outerBox, roundedRadius);
            ofSetColor(ofColor(red, green, blue));
            ofFill();
//            ofRect(innerBox);
            ofRectRounded(innerBox, roundedRadius);
            break;
            
        default:
            break;
    }
    ofPopStyle();
    ofPopMatrix();
};

//--------------------------------------------------------------
void TrackCell::setState(TrackCellState s){
    state = s;
}

//--------------------------------------------------------------
const TrackCellState &TrackCell::getState(){
    return state;
}

//--------------------------------------------------------------
void TrackCell::setColor(ofColor c){
    Tweener.addTween(red, c.r, .2);
    Tweener.addTween(green, c.g, .2);
    Tweener.addTween(blue, c.b, .2);
}

//--------------------------------------------------------------
ofColor &TrackCell::getColor(){
    return color;
}

//--------------------------------------------------------------
void TrackCell::toggle(int x, int y){
    if (boundingBox.inside(x, y)){
        switch (getState()) {
            case off:
                setState(active);
                break;
            case on:
            case active:
                setState(off);
                break;
        }
    }
}
