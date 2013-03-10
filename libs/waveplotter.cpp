 #include "waveplotter.h"


void wavePlotter::setProperties(float x1, float y1, float w1, float h1, ofColor col1, string s, int data_size)
{
  line_weight = 1;

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

}

void wavePlotter::setRange(float low, float high)
{
  lowRange = low;
  highRange = high;
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

void wavePlotter::draw(double * data)
{

	ofPushStyle();
		ofPushMatrix();
		ofTranslate(x, y, 0); // position
			
		ofSetColor(225);
		ofDrawBitmapString(label, 0, -2);
		
		// background
	
		ofSetColor(34, 34, 34);
		ofFill();
		ofRect(0, 0, width, height);

		// frame
		ofSetColor(255);
		ofSetLineWidth(2);
		ofNoFill();
		ofRect(0, 0, width, height);
		
		// content
		ofSetColor(color);
		ofSetLineWidth(line_weight);
					
			ofBeginShape();
			for (int i = 0; i < DATA_SIZE; i++)
			{
				float tx =  ofMap(i, 0, DATA_SIZE, 0, width, true);
				float ty =  ofMap(data[i], lowRange, highRange, height, 0, false);
				
				ofVertex(tx, ty);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
}

void wavePlotter::draw(float * data)
{

	ofPushStyle();
		ofPushMatrix();
		ofTranslate(x, y, 0); // position
			
		ofSetColor(225);
		//ofDrawBitmapString(label, 0, -2);
		
		// background
	
		ofSetColor(34, 34, 34);
		ofFill();
		ofRect(0, 0, width, height);

		// frame
		ofSetColor(255);
		ofSetLineWidth(2);
		ofNoFill();
		ofRect(0, 0, width, height);
		
		// content
		ofSetColor(color);
		ofSetLineWidth(line_weight);
					
			ofBeginShape();
			for (int i = 0; i < DATA_SIZE; i++)
			{
				float tx =  ofMap(i, 0, DATA_SIZE, 0, width, true);
				float ty =  ofMap(data[i], lowRange, highRange, height, 0, false);
				
				ofVertex(tx, ty);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
}



// <?> how to overload method to avoid code duplication in this particular case when dealing with pointer - casting would not work
void wavePlotter::draw (short * data)
{
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(x, y, 0); // position
			
		ofSetColor(225);
		//ofDrawBitmapString(label, 0, -2);
		
		// background
	
		ofSetColor(34, 34, 34);
		ofFill();
		ofRect(0, 0, width, height);

		// frame
		ofSetColor(255);
		ofSetLineWidth(2);
		ofNoFill();
		ofRect(0, 0, width, height);
		
		// content
		ofSetColor(color);
		ofSetLineWidth(line_weight);
					
			ofBeginShape();
			for (int i = 0; i < DATA_SIZE; i++)
			{
				float tx =  ofMap(i, 0, DATA_SIZE, 0, width, true);
				float ty =  ofMap(data[i], lowRange, highRange, height, 0, false);
				
				ofVertex(tx, ty);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

}