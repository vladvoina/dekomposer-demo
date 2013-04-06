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
		/// extra functions //
		void drawFBO1(int offset);
		void drawFBO2(int offset);

		Matrix<float, 1, Dynamic> returnMatrix(int length);
		///////////////////////////////////////////////////////////
		//                    AUDIO MEMBERS                      //
		///////////////////////////////////////////////////////////
		MatrixXf data;

		ofSoundStream soundStream;
		int bufferSize, sampleRate;

		double sample;
		
		ofxMaxiOsc osc;
		ofxMaxiSample sampl, sampl2;

		spectralFlux* flux;
		
		///////////////////////////////////////////////////////////
		//                     PLOTTING                          //
		///////////////////////////////////////////////////////////
		ofFbo fbo, fbo2;
		wavePlotter plotter;
		wavePlotter plotter2;
		wavePlotter plotter3;
		bool onsets;
		float needle_x, needle_y;

		Graph graph, graph2;
		/////////////////////
		int NR_OF_PLOTTERS;
        int ROWS;
        wavePlotter* plotters;//[NR_OF_PLOTTERS*ROWS];
		int NR_OF_PLOTTERS2, ROWS2;
		wavePlotter* plotters2;
	
		// -------- GUI ------- //
		ofxUICanvas *gui;   	
	    void guiEvent(ofxUIEventArgs &e);

		
		/////////////////////////////////////////////////////////////
		//                       OTHER                            //
		/////////////////////////////////////////////////////////////
		PCA pca;
		MatrixXf data_chunk;
		MatrixXf test_data;
		VectorXf ffts;
		VectorXf ffts2;
		

};
