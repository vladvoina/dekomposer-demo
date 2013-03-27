#include "testApp.h"

float testvalues[] = {1,2,1,2,1,2};

//--------------------------------------------------------------
void testApp::setup(){
  ////////////////////////////////////////////////////////////////
  //                    AUDIO                                   //
  //////////////////////////////////////////////////////////////// 
  maxiSettings::setup(44100, 2, 512);
  sampl.load(ofToDataPath("track02.wav"));

  // ************************************************************
  /* make sure you call this last*/
  soundStream.setup(this, 2, 0, maxiSettings::sampleRate, maxiSettings::bufferSize, 4);
  
  const int HOP_SIZE = 512; // half the window size
  //fft.setup(HOP_SIZE*2, HOP_SIZE*2, HOP_SIZE);
  flux = new spectralFlux(HOP_SIZE);
  
  flux->computeFlux(&sampl);
  flux->computeFluxThreshold();
  flux->computePrunnedFlux();
  flux->findFluxOnsets();
 
  data = MatrixXf(2, 14);
  data << 2.5, 0.5, 2.2, 1.9, 3.1, 2.3, 2.0, 1.0, 1.5, 1.1, 8, -1, 3.2, -1.2,
	     // 0.1, -1.5, 2.0, 2.3, 3.9, 2.5, 1.4, 0.5, 0.7, 0.3,   
	      2.4, 0.7, 2.9, 2.2, 3.0, 2.7, 1.6, 1.1, 1.6, 0.9, 2, -2, 4.5, -2.1;
		 // 5.0, 4.3, 0.0, 1.2, -5.3, 2.4, 5.2, -3.2, 1.2, -0.4;
  //data <<
   
  pca.computeCovarianceMatrix(&data);
  cout << "Covariance Matrix: " << endl << *pca.getCovarianceCorrelationMatrix() << endl;
  pca.computeFeatureVector(-1);

  pca.transformData(&data);
  cout << "Transformed data: " << endl << *pca.getTransformedData() << endl;
  
  pca.reexpressData();
  cout << "Reexpressed data: " << *pca.getReexpressedData() << endl;
  

  /*
  vector<int> vec = *flux->getOnsets();
  for (int i=0; i<vec.size(); i++)
  {
	  cout << " " << vec[i];
  }
  cout << " SIZE: " << vec.size() << endl;
  */
  /*
  float* p = flux->getPrunnedFlux();

  for(int i=0; i<(int)(sampl.length/HOP_SIZE); i++)
  {
	cout << p[i] << " ";
  }
  */

  //cout << *flux->getFluxHistoryM() << endl; 
  ////////////////////////////////////////////////////////////////
  //                     PLOTTING                               //
  ////////////////////////////////////////////////////////////////
  plotter.setProperties(10, 10, 1000, 300, ofColor(255, 0, 0), "flux", (int)(sampl.length/HOP_SIZE));
  plotter2.setProperties(10, 10, 1000, 300, ofColor(0, 255, 0), " ", (int)(sampl.length/HOP_SIZE),false);
  plotter3.setProperties(10, 330, 1000, 300, ofColor(255, 0, 0), "flux", (int)(sampl.length/HOP_SIZE));

  plotter.autoScaleRange(flux->getFluxHistory());
  plotter2.setRange(plotter.lowRange, plotter.highRange);
  plotter3.setRange(plotter.lowRange, plotter.highRange);

  graph.setProperties(10, 10, 450, 450, ofColor(255, 0, 0), "reexpressed data", 10);
  graph.setRange(-3, 10);

  graph2.setProperties(470, 10, 450, 450, ofColor(0, 100, 200), "transformed data", 10);
  graph2.setRange(-3, 10);

  // --------------- GUI -----------------//
  ofSetVerticalSync(true); 
  ofEnableSmoothing(); 

  gui = new ofxUICanvas(10,310,1100,100);
  gui->addWidgetDown(new ofxUIRangeSlider(990,7, 0.0, 100.0, 0.0, 100.0, "RSLIDER"));
  ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
  gui->setVisible(false);
  
  ///////////////////////////////////////////////////////////////
  //                      DRAW                                 //
  ///////////////////////////////////////////////////////////////
  ofBackground(0);
  /*
  fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
  fbo.begin();
  ofClear(0, 0, 0, 0);
  ofBackground(0);

  //plotter.draw(flux->getFluxHistory());
  //plotter2.draw(flux->getFluxThresholdHistory());
  plotter3.draw(flux->getPrunnedFlux());

  fbo.end();
  */
  needle_y = 310;
  onsets = false;

}

Matrix<float, 1, Dynamic> testApp::returnMatrix(int l)
{
 return Map<Matrix<float, 1, Dynamic> >(testvalues, l);
}

//--------------------------------------------------------------
void testApp::update(){

	float start = ofMap(mouseX, 0, ofGetWidth(), 0, 100.0);
	float end   = ofMap(mouseY, 0, ofGetHeight(), 0, 100.0);

	//plotter.setDisplayWindow(start, end);

}

//--------------------------------------------------------------------
void testApp::draw(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);   
	//fbo.draw(0, 0);
	//plotter.draw(flux->getFluxHistory());

	graph.draw(&data);
	graph2.draw(pca.getReexpressedData());

	needle_x = ofMap(sampl.position, 0, sampl.length, 10, 1010);

	//ofSetColor(255, 255, 0);
	//ofFill();
	//ofCircle(needle_x, needle_y, 5);
	
	if (onsets)
	{
	ofSetColor(255);
	ofSetLineWidth(1);

	float y_ = 310;

	  for (int i=0; i<flux->getOnsets()->size(); i++)
	  {
		float x = ofMap((*flux->getOnsets())[i], 0, flux->getFluxHistoryM()->size(), 10, 1010);
		ofLine(x, y_, x, y_-80);	
	  }
	}
	
		
}

void testApp::guiEvent(ofxUIEventArgs &e) 
{
	if(e.widget->getName() == "RSLIDER")
	{
		ofxUIRangeSlider *rslider = (ofxUIRangeSlider *) e.widget;
     	plotter.setDisplayWindow(rslider->getScaledValueLow(), rslider->getScaledValueHigh());		
	}
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

	if (key == 'o')
	{
		onsets = !onsets;
	}
	
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

	/*
	float prec = ofMap(y, 0, ofGetHeight(), -2, 2);
	flux->setPrecision(prec);
	flux->computeFluxThreshold();

	fbo.begin();
    ofClear(0, 0, 0, 0);
    ofBackground(0);

    plotter.draw(flux->getFluxHistory());
    plotter2.draw(flux->getFluxThresholdHistory());

    fbo.end();
	*/
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

void testApp::exit()
{
	delete gui;
}