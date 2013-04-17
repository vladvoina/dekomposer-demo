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
	
	MatrixXf zero_mean_data;    // data with the mean of each row subtracted from each coefficient
	MatrixXf covariance_matrix; // covariance matrix - symetric matrix
	MatrixXf feature_vector;    // matrix contaning the reduced or full number of eigenvectors stored in columns in descending order
	MatrixXf feature_vector_t;  // same as above for the optimized reduced covariance matrix
	MatrixXf eigen_values;      // vector for storing the eigenvalues
	//MatrixXf transformed_data;  // expression of original data onto the new eigenvectors basis
	Matrix<float, Dynamic, Dynamic, RowMajor> transformed_data;
	MatrixXf reexpressed_data;  // re-expression onto original coordinate system - if no dimensions are reduced data should be exactly the same
	MatrixXf projected_data;    // projected data
	
	VectorXf data_mean;
	float mean;
	float standard_deviation;
	
	// testing
	long covariance_progres;

public:
	PCA();
	void process(MatrixXf* data);  // <!> deprecated
	float getMean();			   // <!> deprecated
	float getStandardDeviation();  
	// STATIC METHODS
	static float getStdDev(MatrixXf* input)
	{
	  return sqrt((input->array() - input->mean()).matrix().squaredNorm()/input->size());
	}
	// initial functions
	float getCovariance(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y);
	float getCorrelation(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y);
	void computeCovarianceMatrix(Matrix<float, Dynamic, Dynamic>* data);
	void computeCorrelationMatrix(Matrix<float, Dynamic, Dynamic>* data);
	// optimized functions
	void computeMeanVector(MatrixXf* data);
	void computeCovarianceMatrix2 (MatrixXf* data);
	void computeCovarianceMatrix2T(MatrixXf* data);
	MatrixXf* getCovarianceCorrelationMatrix();
	
	// 0 for all components, positive for first highest, negative how many to drop from the end
	void computeFeatureVector (int components = 0);
	void computeFeatureVectorT(int components = 0);
	 // drop prinicipal components - component indicates the number of principal components to drop starting from the highest
	void computeFeatureVectorRedux(int components);
	void computeFeatureVectorReduxT(int components);
	MatrixXf* getFeatureVector();
	
	void transformData(Matrix<float, Dynamic, Dynamic>* data);
	// optimized
	//** transforms data that was used for exatracting eigenvectos 
	void transformData2();
	void transformData2T();
	//** transforms input data - methods to be used if the data to be projected is not the same with the training data
	void transformData2 (MatrixXf* data);
	void transformData2T(MatrixXf* data); 
	Matrix<float, Dynamic, Dynamic, RowMajor>* getTransformedData();

	void reexpressData();
	void reexpressDataT();
	MatrixXf* getReexpressedData();

	// <!> only vectos to be passed in here yet
	void projectData(MatrixXf* data);
	// <!> need revision
	void distanceToSpace(MatrixXf* data);
	void computeDistances();






};