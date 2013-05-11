/// ***************************************************************************
// THIS CLASS HAS BEEN CREATED BASED ON THE EXAMPLE IN THE C Clustering Library
// ****************************************************************************

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
	// temp matrix to store a row major representation of the input data
	Matrix<double, Dynamic, Dynamic, RowMajor>* temp_data;
	int* clusterid; // array to store the IDs
	// method taken from the example in the C Clustering Library
	void kmeans(int nrows, int ncols, double** data, int** mask, int clusters);

public:
	void cluster(Matrix<float, Dynamic, Dynamic, RowMajor>* dataa, int clusters);
	int* getClusterIDs();
};