#include "graph.h"

void Graph::setProperties(float x1, float y1, float w1, float h1, ofColor col1, string s, int data_size, bool back)
{
  line_weight = 1;
  point_weight = 2;
  lines = false;

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

void Graph::setLines(bool l) { lines = l; }

void Graph::draw(MatrixXf* data)
{
	startStyle();
	//content
	//ofFill();
	//ofBeginShape();
	//ofFill();
			for (int i=0; i<data->cols(); i++)
			{
				float tx = ofMap((*data)(0, i), lowRange, highRange, 0, width);
				float ty = ofMap((*data)(1, i), lowRange, highRange, height, 0);
				//if(lines) ofVertex(tx, ty);
				ofCircle(tx, ty, point_weight);
			}
	//ofEndShape(false);
	// zero axis
	ofSetColor(255);
	ofSetLineWidth(1);
	float zero_x = ofMap(0, lowRange, highRange, 0, width);
	float zero_y = ofMap(0, lowRange, highRange, height, 0);
	ofLine(0, zero_y, width, zero_y);
	ofLine(zero_x, height, zero_x, 0);

	endStyle();
}

