#pragma once

#include <Eigen\Dense>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include <cluster.h>
}

using namespace Eigen;

class kMeans {
private:
	
	//MatrixXd* temp_data;
	Matrix<double, Dynamic, Dynamic, RowMajor>* temp_data;
	int** clusterid_;
	void kmeans(int nrows, int ncols, double** data, int** mask);

public:
	void cluster(Matrix<float, Dynamic, Dynamic, RowMajor>* dataa);
};