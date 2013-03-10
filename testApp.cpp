#include "testApp.h"

short testvalues[] = {0, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 2, 3, 4, 5};

//--------------------------------------------------------------
void testApp::setup(){
  ////////////////////////////////////////////////////////////////
  //                    AUDIO                                   //
  //////////////////////////////////////////////////////////////// 
  maxiSettings::setup(44100, 2, 512);
  sampl.load(ofToDataPath("track04.wav"));

  // ************************************************************
  const int HOP_SIZE = 1024; // half the window size
  //fft.setup(HOP_SIZE*2, HOP_SIZE*2, HOP_SIZE);
  flux = new spectralFlux(HOP_SIZE);

  /* make sure you call this last*/
  soundStream.setup(this, 2, 0, maxiSettings::sampleRate, maxiSettings::bufferSize, 4);
  flux->processFlux(&sampl);
  
  ////////////////////////////////////////////////////////////////
  //                     PLOTTING                               //
  ////////////////////////////////////////////////////////////////
  plotter.setProperties(10, 10, 780, 240, ofColor(255, 0, 0), "fft", sampl.length);
  plotter2.setProperties(10, 20 + 240, 780, 240, ofColor(0, 255, 0), "flux", HOP_SIZE);
  //plotter.autoScaleRange(fft.magsToDB());
  plotter.setRange(0, 40);
  plotter2.setRange(0, 40);
  cout << "" << endl;
  
  //plotter.autoScaleRange(sampl.temp);
  plotter.setProperties(10, 10, 780, 240, ofColor(255, 0, 0), "fft", (int)(sampl.length/HOP_SIZE));
  plotter.setRange(0, 5);

  ///////////////////////////////////////////////////////////////
  //                      DRAW                                 //
  ///////////////////////////////////////////////////////////////
  ofBackground(0);
  fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
  fbo.begin();
  ofClear(0, 0, 0, 0);
  ofBackground(0);
  plotter.draw(flux->getFluxHistory());
  fbo.end();

  needle_y = 260;
  
  

}

//--------------------------------------------------------------
void testApp::update(){


}

//--------------------------------------------------------------------
void testApp::draw(){

	fbo.draw(0, 0);

	needle_x = ofMap(sampl.position, 0, sampl.length, 10, 790);

	ofSetColor(255, 255, 0);
	ofFill();
	ofCircle(needle_x, needle_y, 5);
	
	//plotter.draw(fft.magsToDB());
	
	//plotter2.draw( flux->getFlux(fft.magsToDB()) );
	
		
}

//--------------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){
	
	for (int i = 0; i<bufferSize; i++)
	{
		sample = sampl.play();
		//fft.process(sample);
		//sample = 0;

		output[i*nChannels    ] = sample * 0.5;
		output[i*nChannels + 1] = sample * 0.5;
	}

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}