//
//  OSCPoint.h
//  MotionSynth
//
//  Created by Serkan Sökmen on 08.08.2013.
//
//

#pragma once

#include "ofPoint.h"

#define OSC_POINT_DRAW_RADIUS 20

class OSCPoint {
public:
    OSCPoint(){
        isOn = false;
    };
    ofPoint position;
    bool    isOn;
};
