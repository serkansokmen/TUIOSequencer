//
//  TrackCell.cpp
//  BodySequencerApp
//
//  Created by Serkan Sokmen on 31.07.2013.
//
//

#include "TrackCell.h"


//--------------------------------------------------------------
void TrackCell::setup(const ofRectangle &bb, int s, const ofColor &c){
    
    float padding = 8.0f;
    
    step = s;
    
    hue = c.getHue();
    saturation = c.getSaturation();
    brightness = 255;
    
    state = off;
    color.set(c);
    
    boundingBox.set(bb);
    outerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding*.2, boundingBox.getHeight() - padding*.2);
    innerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding, boundingBox.getHeight() - padding);
}

void TrackCell::update(){
    color.setHsb(hue, saturation, brightness);
}

//--------------------------------------------------------------
void TrackCell::draw(){
    
    ofPushMatrix();
    ofPushStyle();
    ofSetLineWidth(2.0);
    
    switch (state) {
        case off:
            ofSetColor(color, 255);
            ofNoFill();
            ofRectRounded(innerBox, 16);
//            ofRect(innerBox);
            break;
        case active:
//            ofSetColor(color, alpha * .5);
//            ofNoFill();
//            ofRect(outerBox);
            ofSetColor(color, alpha);
            ofFill();
            ofRectRounded(innerBox, 16);
            break;
        case on:
//            ofSetColor(color, alpha * .5);
//            ofNoFill();
//            ofRect(outerBox);
            ofSetColor(color, alpha);
            ofFill();
            ofRectRounded(innerBox, 16);
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
    
    switch (state) {
        case off:
            Tweener.addTween(alpha, 50, .2);
            break;
        case active:
            Tweener.addTween(alpha, 150, .2);
            break;
        case on:
            Tweener.addTween(alpha, 255, .2);
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
const TrackCellState &TrackCell::getState(){
    return state;
}

//--------------------------------------------------------------
void TrackCell::setColor(ofColor c){
    Tweener.addTween(hue, c.getHue(), .2);
    Tweener.addTween(saturation, c.getSaturation(), .2);
//    Tweener.addTween(brightness, 255, .2);
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
