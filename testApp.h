#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include <iostream>
#include "waveplotter.h"
#include "spectralFlux.h"
#include <Eigen\Dense>
#include "utils.cpp"

using namespace Eigen;
using namespace std;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void audioOut(float * input, int bufferSize, int nChannels);
		///////////////////////////////////////////////////////////
		//                    AUDIO MEMBERS                      //
		///////////////////////////////////////////////////////////
		ofSoundStream soundStream;
		int bufferSize, sampleRate;

		double sample;
		
		ofxMaxiOsc osc;
		ofxMaxiSample sampl;

		//ofxMaxiFFT fft;
		spectralFlux* flux;

		//float* flux_history;
		///////////////////////////////////////////////////////////
		//                     PLOTTING                          //
		///////////////////////////////////////////////////////////
		ofFbo fbo;

		wavePlotter plotter;
		wavePlotter plotter2;

		float needle_x, needle_y;

		

		

		/////////////////////////////////////////////////////////////
		//                       OTHER                            //
		/////////////////////////////////////////////////////////////
		

};
