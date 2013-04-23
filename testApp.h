#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxTimeline.h"
#include "onsetClassification.h"
#include "ofxMidi.h"

#include <iostream>
using namespace std;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
		ofxTimeline timeline;
		void bangFired(ofxTLBangEventArgs& args);

		ofxMaxiSample sample1;

		onsetClassification* onsets;

		////// MIDI ///////
		ofxMidiOut midiOut;
		int channel;
		int note_shift;
		int* notes;
		int*  probabilities;
		long* incrementors;

};
