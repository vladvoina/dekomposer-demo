#include "testApp.h"

float testvalues[] = {1,2,1,2,1,2};

//--------------------------------------------------------------
void testApp::setup(){
  ////////////////////////////////////////////////////////////////
  //                    AUDIO                                   //
  //////////////////////////////////////////////////////////////// 
  maxiSettings::setup(44100, 2, 512);
  sampl.load(ofToDataPath("track10.wav"));

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

  flux->computeFFTData(&sampl);

  int max_frames = 300;
  data_chunk = flux->getFFTData()->block<100, 300>(0,0);

  //cout << "FRAME 1 is: " << (*flux->getFFTData(&sampl)).col(0) << endl;

  //cout << "FRAME 2 is: " << (*flux->getFFTData(&sampl)).col(1) << endl;

  data = MatrixXf(2, 14);
  data << 2.5, 0.5, 2.2, 1.9, 3.1, 2.3, 2.0, 1.0, 1.5, 1.1, 8, -1, 3.2, -1.2,
	     // 0.1, -1.5, 2.0, 2.3, 3.9, 2.5, 1.4, 0.5, 0.7, 0.3,   
	      2.4, 0.7, 2.9, 2.2, 3.0, 2.7, 1.6, 1.1, 1.6, 0.9, 2, -2, 4.5, -2.1;
		 // 5.0, 4.3, 0.0, 1.2, -5.3, 2.4, 5.2, -3.2, 1.2, -0.4;
  //data <<
   
  cout << "computing covariance matrix..." << endl;
  pca.computeCovarianceMatrix(&data_chunk);
  //cout << "Covariance Matrix: " << endl << *pca.getCovarianceCorrelationMatrix() << endl;
  cout << "computing feature vector..." << endl;
  pca.computeFeatureVector(5);
  cout << "transforming data..." << endl;
  pca.transformData(&data_chunk);
  cout << "reexpressing data..." << endl;
  pca.reexpressData();
  //cout << "Transformed data: " << endl << *pca.getTransformedData() << endl;
  
  //pca.reexpressData();
  //cout << "Reexpressed data: " << *pca.getReexpressedData() << endl;
  
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

  ffts = flux->getFFTData()->col(7);
  ffts2 = pca.getReexpressedData()->col(0);
  //ffts2 = pca.getTransformedData()->col(0);

  ////////////////////////////////////////////////////////////////
  //                     PLOTTING                               //
  ////////////////////////////////////////////////////////////////
  plotter.setProperties(10, 10, 1000, 300, ofColor(255, 0, 0), "flux", flux->getFFTData()->rows());
  plotter2.setProperties(10, 10, 1000, 300, ofColor(0, 255, 0), " ", (int)(sampl.length/HOP_SIZE),false);
  plotter3.setProperties(10, 330, 1000, 300, ofColor(255, 0, 0), "flux", (int)(sampl.length/HOP_SIZE));

  plotter.setDisplayWindow(0, 15);

  plotter.autoScaleRange(ffts);
  plotter2.setRange(plotter.lowRange, plotter.highRange);
  plotter3.setRange(plotter.lowRange, plotter.highRange);

  graph.setProperties(10, 10, 450, 450, ofColor(255, 0, 0), "reexpressed data", 10);
  graph.setRange(-3, 10);

  graph2.setProperties(470, 10, 450, 450, ofColor(0, 100, 200), "transformed data", 10);
  graph2.setRange(-3, 10);

  // --------------- GUI -----------------//
  NR_OF_PLOTTERS = 2;
  ROWS = 2;
  NR_OF_PLOTTERS2 = 2;
  ROWS2 = 2;

  ofSetVerticalSync(true); 
  ofEnableSmoothing(); 
  gui = new ofxUICanvas(10,ofGetHeight()-40,1100,100);
  gui->addSlider("slider",0.0, flux->getFFTData()->cols()-NR_OF_PLOTTERS*ROWS,0.0,300,20);
  gui->addWidgetRight(new ofxUISlider("slider2", 0.0, max_frames-NR_OF_PLOTTERS2*ROWS2, 0.0, 300, 20));
  //gui->addWidgetDown(new ofxUIRangeSlider(990,7, 0.0, 100.0, 0.0, 100.0, "RSLIDER"));
  ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
  gui->setVisible(true);
  
  ///////////////////////////////////////////////////////////////
  //                      DRAW                                 //
  ///////////////////////////////////////////////////////////////
  plotters = new wavePlotter[NR_OF_PLOTTERS*ROWS];
  int offset = 7;
  float size_ = (ofGetWidth()-offset*(NR_OF_PLOTTERS+1))/NR_OF_PLOTTERS;
  float height = size_ * 0.2;  
  float y_pos = 0;
  for(int j=0; j<ROWS; j++)
  {
	y_pos = offset + (offset + height)*j;
    for (int i=0; i<NR_OF_PLOTTERS; i++)
    {
	  int x = ofMap(i,0, NR_OF_PLOTTERS, offset, ofGetWidth()-offset);
	  plotters[NR_OF_PLOTTERS*j+i].setProperties(x, y_pos, size_, height, ofColor(255, 0, 0), "flux", flux->getFFTData()->rows());
	  plotters[NR_OF_PLOTTERS*j+i].setRange(0, 35);
	  plotters[NR_OF_PLOTTERS*j+i].setDisplayWindow(0, 20);
    }
  }
  //////////////////////////////////////////////////////////
  plotters2 = new wavePlotter[NR_OF_PLOTTERS2*ROWS2];
  for(int j=0; j<ROWS2; j++)
  {
	y_pos = offset + (offset + height)*j;
    for (int i=0; i<NR_OF_PLOTTERS2; i++)
    {
	  int x = ofMap(i,0, NR_OF_PLOTTERS2, offset, ofGetWidth()-offset);
	  plotters2[NR_OF_PLOTTERS2*j+i].setProperties(x, y_pos, size_, height, ofColor(255, 0, 0), "flux", pca.getTransformedData()->rows());
	  plotters2[NR_OF_PLOTTERS2*j+i].setRange(0, 35);
	  plotters2[NR_OF_PLOTTERS2*j+i].setDisplayWindow(0, 100);
    }
  }

  ofBackground(0);
  fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
  fbo2.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

  drawFBO1(0);
  drawFBO2(0);
 
  
  needle_y = 310;
  onsets = false;
  //cout << "------" << ffts.transpose() << endl;

}

Matrix<float, 1, Dynamic> testApp::returnMatrix(int l)
{
 return Map<Matrix<float, 1, Dynamic> >(testvalues, l);
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------------
void testApp::draw(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);   
	fbo.draw(0, 0);
	fbo2.draw(0, 300);
	
	//plotter.draw(flux->getFluxHistory());
	//plotter.draw(ffts);
	//graph.draw(&data);
	//graph2.draw(pca.getReexpressedData());

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
	if(e.widget->getName() == "slider")
	{
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
     	//plotter.setDisplayWindow(rslider->getScaledValueLow(), rslider->getScaledValueHigh())
		drawFBO1((int)rslider->getScaledValue());
	} else
	if (e.widget->getName() == "slider2")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		drawFBO2((int)slider->getScaledValue());
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

void testApp::drawFBO1(int offset)
{
  fbo.begin();
   ofClear(0, 0, 0, 0);
   ofBackground(0);
   for(int j=0; j<ROWS; j++)
   {
     for (int i=0; i<NR_OF_PLOTTERS; i++)
     {
	  ffts = flux->getFFTData()->col(offset+NR_OF_PLOTTERS*j+i);
	  plotters[NR_OF_PLOTTERS*j+i].draw(ffts);
     }
   }
  fbo.end();
}

void testApp::drawFBO2(int offset)
{
  fbo2.begin();
    ofClear(0, 0, 0, 0);
    ofBackground(0);
    for(int j=0; j<ROWS2; j++)
    {
      for (int i=0; i<NR_OF_PLOTTERS2; i++)
      {
	   ffts2 = pca.getReexpressedData()->col(offset+NR_OF_PLOTTERS2*j+i);//flux->getFFTData()->col((int)slider->getScaledValue()+NR_OF_PLOTTERS*j+i);
	   //pca.getTransformedData()->col((int)slider->getScaledValue()+NR_OF_PLOTTERS2*j+i);
	   plotters2[NR_OF_PLOTTERS2*j+i].draw(ffts2);
      }
    }
  fbo2.end();
}