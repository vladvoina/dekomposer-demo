#ifndef WAVEPLOTTER
#define WAVEPLOTTER

#include "ofMain.h"
#include "utils.cpp"

// <!> INEFFICIENT because data is mapped every call of draw, however, that makes the function dynamic
//                 also, each sample in the data is drawn which is a waste of computation
// <!> HOW TO OPTIMIZE - only draw the same amount of sample as the number of pixels that the graph will take



class wavePlotter {

	int DATA_SIZE;
	float x, y, width, height;
	string label;
	float lowRange, highRange;
	float line_weight;

    ofColor color;

public:

	
	void setProperties(float x, float y, float width, float height, 
		               ofColor col,string s, int data_size);
	void setRange(float low, float high);
	void autoScaleRange(short * data);
	void autoScaleRange(float * data);
	void autoScaleRange(double * data);

	void draw(double * data);
	void draw(float * data);
	void draw(short * data);


};

#endif