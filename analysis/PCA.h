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
	MatrixXf zero_mean_data;
	MatrixXf covariance_matrix;
	MatrixXf feature_vector;
	MatrixXf feature_vector_t;
	MatrixXf eigen_values;
	MatrixXf transformed_data;
	MatrixXf reexpressed_data;

	MatrixXf projected_data;
	
	VectorXf data_mean;
	float mean;
	float standard_deviation;
	
	// testing
	long covariance_progres;

public:
	PCA();
	void process(MatrixXf* data);
	float getMean();
	float getStandardDeviation();
	// STATIC METHODS
	static float getStdDev(MatrixXf* input)
	{
	  return sqrt((input->array() - input->mean()).matrix().squaredNorm()/input->size());
	}
	//
	float getCovariance(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y);
	float getCorrelation(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y);
	void computeCovarianceMatrix(Matrix<float, Dynamic, Dynamic>* data);
	void computeCorrelationMatrix(Matrix<float, Dynamic, Dynamic>* data);
	// optimized functions
	void computeMeanVector(MatrixXf* data);
	void computeCovarianceMatrix2 (MatrixXf* data, bool bijection=false);
	void computeCovarianceMatrix2T(MatrixXf* data);

	MatrixXf* getCovarianceCorrelationMatrix();
	// 0 for all components, positive for first highest, negative how many to drop from the end
	void computeFeatureVector (int components = 0);
	void computeFeatureVectorT(int components = 0);
	void computeFeatureVectorRedux(int components); // drop prinicipal components
	void computeFeatureVectorReduxT(int components);

	MatrixXf* getFeatureVector();
	void transformData(Matrix<float, Dynamic, Dynamic>* data);
	// optimized
	void transformData2();
	void transformData2T();
	MatrixXf* getTransformedData();

	void reexpressData();
	void reexpressDataT();

	MatrixXf* getReexpressedData();

	void projectData(MatrixXf* data);
	void distanceToSpace(MatrixXf* data);
	void computeDistances();






};