#pragma once

#include <Eigen\Dense>

using namespace Eigen;

class Covariance {
private:
	// - Passed in data must be structured in a: ROWS - DIMENSIONS, COLUMNS - OBSERVATIONS (frames)
	// - The data will be processed depending on the number of observations
	// - If the input data are of Eigen datatype dimension information is not necessary
	MatrixXd data;
	
public:
	Covariance();
	void process(MatrixXd* data);
	void zeroMean();
	void variance();
	double standardDeviation();
	
	MatrixXd getCovarianceMatrix();



};