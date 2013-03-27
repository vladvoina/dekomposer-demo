 #include "waveplotter.h"


void wavePlotter::setProperties(float x1, float y1, float w1, float h1, ofColor col1, string s, int data_size, bool back)
{
  line_weight = 1;

  background = back;

  x = x1;
  y = y1;
  width = w1;
  height = h1;
  label = s;
  DATA_SIZE = data_size;
  // set def range
  lowRange = -1;
  highRange = 1;

 //  <!> this assignment might pe problematic
  color = col1;

  data_start_index = 0;
  data_end_index = DATA_SIZE;
}

void wavePlotter::setRange(float low, float high)
{
  lowRange = low;
  highRange = high;
}

// pass in percentages
void wavePlotter::setDisplayWindow(float low, float high)
{
	data_start_index = ofMap(low, 0, 100.0, 0, DATA_SIZE, true);
	data_end_index   = ofMap(high, 0, 100.0, 0, DATA_SIZE, true); 
}

void wavePlotter::autoScaleRange(short * data)
{
	lowRange = Utils::ofMin(data, DATA_SIZE);
	highRange = Utils::ofMax(data, DATA_SIZE);
}

void wavePlotter::autoScaleRange(float * data)
{
	lowRange = Utils::ofMin(data, DATA_SIZE);
	highRange = Utils::ofMax(data, DATA_SIZE);
}

void wavePlotter::autoScaleRange(double * data)
{
	lowRange = Utils::ofMin(data, DATA_SIZE);
	highRange = Utils::ofMax(data, DATA_SIZE);
}

void wavePlotter::startStyle()
{
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(x, y, 0); // position		
		// background
	    if (background)
		{
		ofSetColor(20, 20, 20);
		ofFill();
		ofRect(0, 0, width, height);
		}
		// frame
		ofSetColor(255);
		ofSetLineWidth(2);
		ofNoFill();
		ofRect(0, 0, width, height);	
		// content
		ofSetColor(color);
		ofSetLineWidth(line_weight);
}

void wavePlotter::endStyle()
{
	     // text
		 ofSetColor(225);
		 ofDrawBitmapString(label, 0, -2);

		ofPopMatrix();
	ofPopStyle();
}

void wavePlotter::draw(double * data)
{
	startStyle();
			ofBeginShape();
			for (int i=0; i<width; i++)
			{
				int index = (int) ofMap(i, 0, width, data_start_index, data_end_index);
				float ty =  ofMap(data[index], lowRange, highRange, height, 0, false);
				ofVertex(i, ty);
			}
			/*
			for (int i = 0; i < DATA_SIZE; i++)
			{
				float tx =  ofMap(i, 0, DATA_SIZE, 0, width, true);
				float ty =  ofMap(data[i], lowRange, highRange, height, 0, false);
				
				ofVertex(tx, ty);
			}*/
			ofEndShape(false);		
	endStyle();
}

void wavePlotter::draw(float * data)
{
	startStyle();
			ofBeginShape();
			for (int i=0; i<width; i++)
			{
				int index = (int) ofMap(i, 0, width, data_start_index, data_end_index);
				float ty =  ofMap(data[index], lowRange, highRange, height, 0, false);
				ofVertex(i, ty);
			}
			/*
			for (int i = 0; i < DATA_SIZE; i++)
			{
				float tx =  ofMap(i, 0, DATA_SIZE, 0, width, true);
				float ty =  ofMap(data[i], lowRange, highRange, height, 0, false);
				
				ofVertex(tx, ty);
			}
			*/
			ofEndShape(false);
	endStyle();
}

// <?> how to overload method to avoid code duplication in this particular case when dealing with pointer - casting would not work
void wavePlotter::draw (short * data)
{
	startStyle();		
			ofBeginShape();
			for (int i=0; i<width; i++)
			{
				int index = (int) ofMap(i, 0, width, data_start_index, data_end_index);
				float ty =  ofMap(data[index], lowRange, highRange, height, 0, false);
				ofVertex(i, ty);
			}
			/*
			for (int i = 0; i < DATA_SIZE; i++)
			{
				float tx =  ofMap(i, 0, DATA_SIZE, 0, width, true);
				float ty =  ofMap(data[i], lowRange, highRange, height, 0, false);
				
				ofVertex(tx, ty);
			}
			*/
			ofEndShape(false);
	endStyle();
}