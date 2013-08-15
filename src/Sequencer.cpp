//
//  SegmentsContainer.cpp
//  PresenceDetectorGrid
//
//  Created by Serkan Sökmen on 31.07.2013.
//
//

#include "Sequencer.h"


#define COLUMNS 8
#define ROWS    8


//--------------------------------------------------------------
Sequencer::Sequencer(){
    
    bpm = 120;
	step = 0;
    rTimer = 0;
	diffTime = 0;
    
}

//--------------------------------------------------------------
Sequencer::~Sequencer(){
    tracks.clear();
}

//--------------------------------------------------------------
void Sequencer::setup(ofBaseApp *app, const ofRectangle rect, int columCount, int rowCount){
    
    step = 0;
    columns = columCount;
    rows = rowCount;
    
    // Dimensions
    boundingBox.set(rect);
    sliderArea = ofRectangle(0,
                             0,
                             220,
                             boundingBox.getHeight());
    
    stepButtonWidth = rect.getWidth() / columns;
    stepButtonHeight = rect.getHeight() / rows;
    
    // Clear and re-assign tracks
    tracks.clear();
    tracks.assign(rows, Track());
    
    // Create tracks
    for (int i = 0; i < rows; i++) {
        tracks[i].setup(i,
                        ofRectangle(rect.getX(), stepButtonHeight * i, rect.getWidth(), stepButtonHeight),
                        columns);
    }
    
    scrubber.set(0, 0, stepButtonWidth, rect.getHeight());
    
    ///////////////////////////////
    //      Audio Stuff
    ///////////////////////////////
    
    // initialize ofSoundStreamSetup
    ofSoundStreamSetup(2, 0, app, 44100, 256, 4);
    
    // synth paramters are like instance variables -- they're values you can set later, by
    // calling synth.setParameter()
    ControlGenerator bpm = synth.addParameter("tempo", 100).min(50).max(300);
    ControlGenerator transpose = synth.addParameter("transpose", 0).min(-6).max(6);
    
    // ControlMetro generates a "trigger" message at a given bpm. We multiply it by four because we
    // want four 16th notes for every beat
    ControlGenerator metro = ControlMetro().bpm(4 * bpm);
    
    // ControlStepper increments a value every time it's triggered, and then starts at the beginning again
    // Here, we're using it to move forward in the sequence
    ControlGenerator step = ControlStepper().end(COLUMNS).trigger(metro);
    
    // ControlSwitcher holds a list of ControlGenerators, and routes whichever one the inputIndex is pointing
    // to to its output.
    ControlSwitcher pitches = ControlSwitcher().inputIndex(step);
    ControlSwitcher cutoffs = ControlSwitcher().inputIndex(step);
    ControlSwitcher glides = ControlSwitcher().inputIndex(step);
    
    // stick a bunch of random values into the pitch and cutoff lists
    for(int i = 0; i < COLUMNS; i++){
        ControlGenerator pitchForThisStep = synth.addParameter("step" + ofToString(i) + "Pitch", randomFloat(10, 80)).min(10).max(80);
        pitches.addInput(pitchForThisStep);
        
        ControlGenerator cutoff = synth.addParameter("step" + ofToString(i) + "Cutoff", 240).min(0).max(1500);
        cutoffs.addInput(cutoff);
        
        ControlGenerator glide = synth.addParameter("step" + ofToString(i) + "Glide", 0).min(0).max(0.1);
        glides.addInput(glide);
        
    }
    
    // Define a scale according to steps in a 12-note octave. This is a pentatonic scale. Like using
    // just the black keys on a piano
    vector<float> scale;
    scale.push_back(0);
    scale.push_back(2);
    scale.push_back(3);
    scale.push_back(5);
    scale.push_back(7);
    scale.push_back(10);
    
    // ControlSnapToScale snaps a float value to the nearest scale value, no matter what octave its in
    ControlGenerator midiNote = transpose + ControlSnapToScale().setScale(scale).input(pitches);
    
    ControlGenerator frequencyInHertz = ControlMidiToFreq().input(midiNote);
    
    // now that we've done all that, we have a frequency signal that's changing 4x per beat
    Generator tone = RectWave().freq( frequencyInHertz.smoothed().length(glides) );
    
    // create an amplitude signal with an ADSR envelope, and scale it down a little so it's not scary loud
    Generator amplitude = ADSR(0.01, 0.1, 0,0).trigger(metro) * 0.3;
    
    // create a filter, and feed the cutoff sequence in to it
    LPF24 filter =  LPF24().cutoff(cutoffs).Q(0.1);
    filter.input(tone * amplitude);
    
    // rout the output of the filter to the synth's main output
    synth.setOutputGen( filter );
    
    // build a slider for each parameter
    vector<ControlParameter> synthParameters = synth.getParameters();
    for(int i = 0; i < synthParameters.size(); i++){
        sliders.push_back(ParameterSlider(synthParameters.at(i)));
    }
};

//--------------------------------------------------------------
void Sequencer::update(){
    
    // Update metronome
    aTimer = ofGetElapsedTimeMillis();
	rTimer = aTimer - diffTime;
	
	float bpMillis;
	bpMillis = (bpm / 60.0f) * 1000;
	float beatPulse;
	beatPulse = (60.0f / bpm / 4) * 1000;
    
    if (rTimer > beatPulse){
		diffTime = aTimer;
		step++;
		if(step == columns) step = 0;
	}
    
    // Update tracks
    for (int i=0; i<tracks.size(); i++){
        tracks[i].update(step);
    }
    
    // Move indicator line
    scrubber.setX(step * stepButtonWidth);
};

//--------------------------------------------------------------
void Sequencer::draw(){
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::darkGreen);
    ofRect(0, 0, stepButtonWidth * columns, stepButtonHeight * rows);
    ofPopStyle();
    
    for (int i=0; i<tracks.size(); i++){
        tracks[i].draw();
    }
    
    float endSize = 8.0f;
    ofPoint lStart(stepButtonWidth * (step + .5), -endSize*.2);
    ofPoint lEnd(stepButtonWidth * (step + .5), stepButtonHeight * rows + endSize*.2);
    
    lStart -= ofPoint(0, endSize);
    lEnd += ofPoint(0, endSize);
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor(ofColor::green, 200);
    // Draw circle pins
    ofSetLineWidth(1.0);
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofRect(lStart, stepButtonWidth, endSize);
    ofRect(lEnd, stepButtonWidth, endSize);
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofFill();
    
    ofPopStyle();
    ofPopMatrix();
    
    ofSetColor(ofColor::green, 10);
    ofRect(scrubber);
    
    ofPushMatrix();
    ofTranslate(-240, -20);
    // create some sliders to control those parameters
    float sliderHeight = sliderArea.getHeight() / sliders.size();
    for(int i = 0; i< sliders.size(); i++){
        ofPoint sliderStart = ofPoint(sliderArea.getX(), sliderArea.getY() + i * sliderHeight);
        ParameterSlider& slider = sliders.at(i);
        float sliderPosition = (slider.param.getValue() - slider.param.getMin()) / (slider.param.getMax() - slider.param.getMin());
        ofSetColor(50, 50, 50);
        ofRect(sliderStart.x, sliderStart.y, sliderArea.getWidth() * sliderPosition, sliderHeight - 3);
        ofSetColor(255, 255, 255);
        stringstream sliderLabel;
        sliderLabel << slider.param.getDisplayName()
        << ": "
        << slider.param.getValue();
        ofDrawBitmapString(sliderLabel.str(), sliderStart.x + 5, sliderStart.y + 20);
    }
    ofPopMatrix();
};

void Sequencer::mouseDragged(int x, int y){
    if (sliderArea.inside(x, y)) {
        float amountDown = (y - sliderArea.getY()) / sliderArea.getHeight();
        float amountAcross = (x - sliderArea.getX()) / sliderArea.getWidth();
        int sliderIndex = sliders.size() * amountDown;
        sliders.at(sliderIndex).param.setNormalizedValue(amountAcross);
    }
}

//--------------------------------------------------------------
void Sequencer::toggle(int x, int y){
    for (int i=0; i<tracks.size(); i++){
        tracks[i].toggle(x, y);
    }
}

//--------------------------------------------------------------
void Sequencer::toggleIndex(int i, int j){
    float x = boundingBox.getX() + i * stepButtonWidth;
    float y = boundingBox.getY() + j * stepButtonHeight;
    
    for (int i=0; i<tracks.size(); i++){
        tracks[i].toggle(x, y);
    }
}

//--------------------------------------------------------------
void Sequencer::setBPM(int _bpm){
    bpm = _bpm;
//    sliders.at(0).param.setNormalizedValue(bpm);
}


//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<ABlob *> *blobs){
    // Reset segment touch states
//    vector<GridSegment>::iterator segment;
//    for (segment = segments.begin(); segment != segments.end(); segment++){
//        segment->setState(off);
//    }
//    
//    for (int i=0; i<segments.size(); i++){
//        GridSegment *segmentPtr = &segments[i];
//        
//        for(int i=0; i<blobs->size(); i++){
//            ABlob &aBlob = *(blobs->at(i));
//            ofRectangle blobRect(aBlob.bx, aBlob.by, aBlob.dimx, aBlob.dimy);
//            if (segmentPtr->boundingBox.inside(blobRect.getCenter()) &&
//                segmentPtr->boundingBox.intersects(lineStartPos, lineEndPos)){
//                segmentPtr->setState(on);
//            }
//        }
//    }
}

//--------------------------------------------------------------
void Sequencer::checkSegments(const vector<OSCPoint> &points){
    // Reset segment touch states
//    vector<GridSegment>::iterator segment;
//    for (segment = segments.begin(); segment != segments.end(); segment++){
//        segment->setState(off);
//    }
//    
//    if (points.size() > 0){
//        for (int i=0; i<segments.size(); i++){
//            GridSegment *segmentPtr = &segments[i];
//            
//            for(int i=0; i<points.size(); i++){
//                segmentPtr->setState(active);
//                if (segmentPtr->boundingBox.inside(ofVec2f(points[i].position)) &&
//                    segmentPtr->boundingBox.intersects(lineStartPos, lineEndPos)){
//                    segmentPtr->setState(on);
//                }
//            }
//        }
//    }
};

//
////--------------------------------------------------------------
//void Sequencer::segmentOn(int x, int y){
//    int i = y * columns + x;
//    segments[i].bTouchesBlob = true;
//}
//
////--------------------------------------------------------------
//void Sequencer::segmentOff(int x, int y){
//    int i = y * columns + x;
//    segments[i].bTouchesBlob = false;
//}
