//
//  TrackCell.cpp
//  TUIOSequencer
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
    
    state = cellOff;
    color.set(hue, saturation, brightness);
    
    boundingBox.set(bb);
    outerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding*.2, boundingBox.getHeight() - padding*.2);
    innerBox.setFromCenter(boundingBox.getCenter(), boundingBox.getWidth() - padding, boundingBox.getHeight() - padding);
}

//--------------------------------------------------------------
void TrackCell::update(){
    color.set(hue, saturation, brightness);
}

//--------------------------------------------------------------
void TrackCell::draw(){
    
    ofPushMatrix();
    ofSetLineWidth(1.0f);
    
    switch (state) {
        case cellOff:
            ofPushStyle();
            ofSetColor(color, 255);
            ofNoFill();
            ofRect(innerBox);
            ofPopStyle();
            break;
        case cellActive:
            ofPushStyle();
            ofSetColor(color, alpha);
            ofRect(innerBox);
            ofPopStyle();
            break;
        case cellOn:
            ofPushStyle();
            ofSetColor(color, alpha);
            ofRect(innerBox);
            ofPopStyle();
            break;
            
        default:
            break;
    }
    
    ofPopMatrix();
};

//--------------------------------------------------------------
void TrackCell::loadSound(string path){
    soundPlayer.loadSound(path);
    soundPath = path;
}

//--------------------------------------------------------------
void TrackCell::play(){
    soundPlayer.loadSound(soundPath);
    soundPlayer.play();
}

//--------------------------------------------------------------
void TrackCell::setState(TrackCellState s){
    
    state = s;
    
    switch (state) {
        case cellOff:
            Tweener.addTween(alpha, 50, .2);
            break;
        case cellActive:
            Tweener.addTween(alpha, 150, .2);
            break;
        case cellOn:
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
void TrackCell::on(int x, int y){
    if (boundingBox.inside(x, y)){
        setState(cellActive);
    }
}

//--------------------------------------------------------------
void TrackCell::off(int x, int y){
    if (boundingBox.inside(x, y)){
        setState(cellOff);
    }
}

//--------------------------------------------------------------
void TrackCell::toggle(int x, int y){
    if (boundingBox.inside(x, y)){
        switch (getState()) {
            case cellOff:
                setState(cellActive);
                break;
            case cellOn:
            case cellActive:
                setState(cellOff);
                break;
        }
    }
}
