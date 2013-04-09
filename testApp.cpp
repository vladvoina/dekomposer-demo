#include "testApp.h"

 int plotter_height = 340;
 int plotter_y_pos = 220;//430;
//--------------------------------------------------------------
void testApp::setup(){
  ////////////////////////////////////////////////////////////////
  //                    AUDIO                                   //
  //////////////////////////////////////////////////////////////// 
  maxiSettings::setup(44100, 2, 512);
  sampl.load(ofToDataPath("track20.wav"));
  sampl2.load(ofToDataPath("Soundset/Kicks2_test.wav"));
  // ************************************************************
  /* make sure you call this last*/
  soundStream.setup(this, 2, 0, maxiSettings::sampleRate, maxiSettings::bufferSize, 4);
  
  const int HOP_SIZE = 256; // half the window size
  //fft.setup(HOP_SIZE*2, HOP_SIZE*2, HOP_SIZE);
  flux = new spectralFlux(HOP_SIZE, 1024);
  
  flux->computeFFTData(&sampl);
  flux->computeFluxOff();
  flux->computeFluxThreshold();
  flux->computePrunnedFlux();
  flux->findFluxOnsets();
  flux->computeOnsetsFFT();

  int max_frames = 3;
  const int fft_crop = 512;
  //data_chunk = MatrixXf(fft_crop, max_frames);
  data_chunk = *flux->getOnsetsFFT();

  data = MatrixXf(1, 5);
  data << 600, 470, 170, 430, 300;
	   //   3, 4  , 2  , 5, 6, 3  , 7  , 3, 8 , 9 ;
  cout << "STD dev is: " << PCA::getStdDev(&data) << endl;
 
/*
  for (int i=0; i<max_frames; i++)
  {
  cout << "TESTING SAMPLE " << i+1 << " >>>>>>>>" << endl;
  test_data = flux->getFFTData()->block<fft_crop, 1>(0, testvalues[i]);
  pca.projectData(&test_data);
  pca.computeDistances();
  pca.distanceToSpace(&test_data);
  cout << endl;
  }
*/
  
  ffts = flux->getFFTData()->col(7);
  ffts2 = data_chunk.col(0);
  ////////////////////////////////////////////////////////////////
  //                     PLOTTING                               //
  ////////////////////////////////////////////////////////////////
 
  plotter.setProperties(10, plotter_y_pos, 1000, plotter_height, ofColor(255, 100, 0), "spectral flux", flux->getFluxHistoryM()->size());
  plotter.autoScaleRange(flux->getFluxHistory());

  plotter2.setProperties(10, plotter_y_pos, 1000, plotter_height, ofColor(0, 60, 210), "spectral flux", flux->getFluxHistoryM()->size(), false);
  plotter2.setRange(plotter.lowRange, plotter.highRange);

  graph.setProperties(10, 10, 450, 450, ofColor(255, 0, 0), "reexpressed data", 10);
  graph.setRange(-10, 11);
  graph2.setProperties(470, 10, 450, 450, ofColor(0, 100, 200), "transformed data", 10);
  graph2.setRange(-10, 11);
 
  NR_OF_PLOTTERS = 1;
  ROWS = 1;
  NR_OF_PLOTTERS2 = 1;
  ROWS2 = 1;

   // --------------- GUI -----------------//
  ofSetVerticalSync(true); 
  ofEnableSmoothing(); 
  gui = new ofxUICanvas(10,ofGetHeight()-80,1100,100);
  gui->addSlider("slider",0.0, flux->getFFTData()->cols()-NR_OF_PLOTTERS*ROWS,0.0,600,20);
  gui->addWidgetRight(new ofxUISlider("slider2", 0.0, data_chunk.cols()-NR_OF_PLOTTERS2*ROWS2, 0.0, 300, 20));
  gui->addWidgetDown(new ofxUISlider("std precision", 0.0, 3.0, 0.0, 220, 20));
  gui->addWidgetRight(new ofxUISlider("std avg length", 3, 100, 30, 220, 20));
  gui->addWidgetRight(new ofxUISlider("mean mult", 0.0, 2.0, 1.0, 220, 20));
  gui->addWidgetRight(new ofxUISlider("mean avg length", 5, 60, 15, 220, 20));
  //gui->addWidgetDown(new ofxUIRangeSlider(990,7, 0.0, 100.0, 0.0, 100.0, "RSLIDER"));
  ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
  gui->setVisible(true);
  
  ///////////////////////////////////////////////////////////////
  //                      DRAW                                 //
  ///////////////////////////////////////////////////////////////
  plotters = new wavePlotter[NR_OF_PLOTTERS*ROWS];
  int offset = 7;
  float size_ = (ofGetWidth()-offset*(NR_OF_PLOTTERS+1))/NR_OF_PLOTTERS;
  float height = size_ * 0.2; cout << "height is:" << height  << endl; 
  float y_pos = 0;
  for(int j=0; j<ROWS; j++)
  {
	y_pos = offset + (offset + height)*j;
    for (int i=0; i<NR_OF_PLOTTERS; i++)
    {
	  int x = ofMap(i,0, NR_OF_PLOTTERS, offset, ofGetWidth()-offset);
	  plotters[NR_OF_PLOTTERS*j+i].setProperties(x, y_pos, size_, height, ofColor(255, 0, 0), "fft", flux->getFFTData()->rows());
	  plotters[NR_OF_PLOTTERS*j+i].setRange(0, 60);
	  plotters[NR_OF_PLOTTERS*j+i].setDisplayWindow(0, 100);
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
	  plotters2[NR_OF_PLOTTERS2*j+i].setProperties(x, y_pos, size_, height, ofColor(255, 0, 0), "onset ffts", data_chunk.rows());
	  plotters2[NR_OF_PLOTTERS2*j+i].setRange(0, 60);
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
  thresh_type = false;
  
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------------
void testApp::draw(){
	//ofEnableBlendMode(OF_BLENDMODE_ALPHA);   
	ofClear(0,0,0,0);
	fbo.draw(0, 0);
	//fbo2.draw(0, 215);
	plotter.draw(flux->getFluxHistory());
	plotter2.draw(flux->getFluxThresholdHistory());
	//plotter.draw(ffts);
	//graph.draw(&data);
	//graph2.draw(pca.getReexpressedData());

	needle_x = ofMap(sampl.position, 0, sampl.length, 10, 1010);
	ofSetColor(20, 255, 0);
	ofLine(needle_x, plotter_y_pos+plotter_height, needle_x, plotter_y_pos); 
	
	if (onsets)
	{
	ofSetColor(255);
	ofSetLineWidth(1);
	float y_ = plotter_y_pos + plotter_height;
	  for (int i=0; i<flux->getOnsets()->size(); i++)
	  {
		float x = ofMap((*flux->getOnsets())[i], 0, flux->getFluxHistoryM()->size(), 10, 1010);
		ofLine(x, y_, x, y_-plotter_height*0.7);	
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
	} else
    if (e.widget->getName() == "std precision")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		flux->setPrecision(slider->getScaledValue());
		flux->computeFluxThreshold();
		flux->computePrunnedFlux();
        flux->findFluxOnsets();
	} else
	if (e.widget->getName() == "std avg length")
	{
       ofxUISlider *slider = (ofxUISlider *) e.widget;
	   flux->setStdAvgLength((int)slider->getScaledValue());
	   flux->computeFluxThreshold();
	   flux->computePrunnedFlux();
       flux->findFluxOnsets();

	} else
	if (e.widget->getName() == "mean mult")
	{
       ofxUISlider *slider = (ofxUISlider *) e.widget;
	   flux->setMeanMult(slider->getScaledValue());
	   flux->computeFluxThreshold();
	   flux->computePrunnedFlux();
       flux->findFluxOnsets();

	} else
	if (e.widget->getName() == "mean avg length")
	{
       ofxUISlider *slider = (ofxUISlider *) e.widget;
	   flux->setMeanAvgLength((int)slider->getScaledValue());
	   flux->computeFluxThreshold();
	   flux->computePrunnedFlux();
       flux->findFluxOnsets();
	}
}


//--------------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){
	
	for (int i = 0; i<bufferSize; i++)
	{
		sample = sampl.play(0.5);
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

	if (key == 't')
	{
		thresh_type = !thresh_type;
		cout << "THRESH TYPE is " << thresh_type << endl;
	}

	if (key == 'c')
	{
	   flux->computeFluxThreshold(thresh_type);
	   flux->computePrunnedFlux();
       flux->findFluxOnsets();
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
	   ffts2 = data_chunk.col(offset+NR_OF_PLOTTERS2*j+i);  //pca.getReexpressedData()->col(offset+NR_OF_PLOTTERS2*j+i);//flux->getFFTData()->col((int)slider->getScaledValue()+NR_OF_PLOTTERS*j+i);
	   //pca.getTransformedData()->col((int)slider->getScaledValue()+NR_OF_PLOTTERS2*j+i);
	   plotters2[NR_OF_PLOTTERS2*j+i].draw(ffts2);
      }
    }
  fbo2.end();
}