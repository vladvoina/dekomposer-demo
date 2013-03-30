#pragma once

#include <Eigen\Dense>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;
using namespace Eigen;

class PCA {
private:
	// - Passed in data must be structured in a: ROWS - DIMENSIONS, COLUMNS - OBSERVATIONS (frames)
	// - The data will be processed depending on the number of observations
	// - If the input data are of Eigen datatype dimension information is not necessary
	MatrixXf data;
	MatrixXf covariance_matrix;
	MatrixXf feature_vector;
	MatrixXf eigen_values;
	MatrixXf transformed_data;
	MatrixXf reexpressed_data;
	
	RowVectorXf data_mean;
	float mean;
	float standard_deviation;
	
	// testing
	long covariance_progres;

public:
	PCA();
	void process(MatrixXf* data);
	float getMean();
	float getStandardDeviation();
	///
	float getCovariance(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y);
	float getCorrelation(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y);
	void computeCovarianceMatrix(Matrix<float, Dynamic, Dynamic>* data);
	void computeCorrelationMatrix(Matrix<float, Dynamic, Dynamic>* data);

	MatrixXf* getCovarianceCorrelationMatrix();
	// 0 for all components, positive for first highest, negative how many to drop from the end
	void computeFeatureVector(int components = 0);
	void transformData(Matrix<float, Dynamic, Dynamic>* data);
	MatrixXf* getTransformedData();
	void reexpressData();
	MatrixXf* getReexpressedData();;





};