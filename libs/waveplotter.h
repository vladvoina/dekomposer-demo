#ifndef WAVEPLOTTER
#define WAVEPLOTTER

#include "ofMain.h"
#include "utils.cpp"
#include <Eigen/Dense>

using namespace Eigen;

class wavePlotter {
private:
	int nothing_yet;

protected:
	int DATA_SIZE;
	float x, y, width, height;
	string label;
	float line_weight;
	bool background;
	int data_start_index, data_end_index;

    ofColor color;
	void startStyle();
	void endStyle();

public:

	float lowRange, highRange;
	
	void setProperties(float x, float y, float width, float height, 
		               ofColor col,string s, int data_size, bool background=true);
    void setRange(float low, float high);
	void setDisplayWindow(float start, float end); // in percentages

	void autoScaleRange(VectorXf &data);
	void autoScaleRange(short * data);
	void autoScaleRange(float * data);
	void autoScaleRange(double * data);

	void draw(double * data);
	void draw(float * data);
	void draw(short * data);
	void draw(VectorXf &data);


};

#endif