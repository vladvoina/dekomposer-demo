#pragma once

#include "waveplotter.h"
#include <Eigen/Dense>

using namespace Eigen;

class Graph: public wavePlotter
{
protected:
	float point_weight;
	bool lines;

public:
	void setLines(bool lines=true);
	void setProperties(float x, float y, float width, float height, 
		               ofColor col,string s, int data_size, bool background=true);

	//Data is assumed to be a 2(row)-dimentional Matrix organized
	void draw(MatrixXf* data);
	void draw(Matrix<double, Dynamic, Dynamic, RowMajor>* data);
	void draw(Matrix<float, Dynamic, Dynamic, RowMajor>* data);

};