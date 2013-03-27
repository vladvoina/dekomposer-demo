#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include <iostream>
#include "waveplotter.h"
#include "spectralFlux.h"
#include <Eigen\Dense>
#include "utils.cpp"
#include "PCA.h"
#include "ofxUI.h"
#include "Graph.h"

using namespace Eigen;
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

		void audioOut(float * input, int bufferSize, int nChannels);

		Matrix<float, 1, Dynamic> returnMatrix(int length);
		///////////////////////////////////////////////////////////
		//                    AUDIO MEMBERS                      //
		///////////////////////////////////////////////////////////
		MatrixXf data;

		ofSoundStream soundStream;
		int bufferSize, sampleRate;

		double sample;
		
		ofxMaxiOsc osc;
		ofxMaxiSample sampl;

		spectralFlux* flux;
		
		///////////////////////////////////////////////////////////
		//                     PLOTTING                          //
		///////////////////////////////////////////////////////////
		ofFbo fbo;
		wavePlotter plotter;
		wavePlotter plotter2;
		wavePlotter plotter3;
		bool onsets;
		float needle_x, needle_y;

		Graph graph, graph2;
		
		// -------- GUI ------- //
		ofxUICanvas *gui;   	
	    void guiEvent(ofxUIEventArgs &e);

		
		/////////////////////////////////////////////////////////////
		//                       OTHER                            //
		/////////////////////////////////////////////////////////////
		PCA pca;
		

};
