#pragma once

#include <cmath>
#include "ofxMaxim.h"
#include <Eigen/Dense>
#include "PCA.h"
#include <vector>
#include <iostream>


using namespace std;
using namespace Eigen;

class spectralFlux {
private:
	// <!> make sure you are computing the flux in steps as close as possible to eachother
	// <!> be careful when reading stero signal with panning
	// <!> maybe analyse panninG
	// <&> add flux grouping
	// <&> add several types of flux analysers depending on the outcome or application
	// <&> positive and negative flux
	//***********************************

	// FFT
	int WINDOW_SIZE;
	int BINS_SIZE; // half the fft size
	int HOP_SIZE;
	ofxMaxiFFT fft;

	// SPECTRAL FLUX
	float* flux_sig;
	float* lastFFT;
	float diff; // difference between successive bins

	int flux_history_length; // dependent on the length of the samples
	MatrixXf flux_history_matrix; // float*  flux_history; 
	//float** flux_sig_history;
	
	// THRESHOLDING
	// no need for an instance of PCA, create static methods for computing mean and std deviation
	PCA pca;
	int flux_average_length;
	float flux_threshold;
	float* flux_threshold_history; // maybe turn into eigen matrix
	float precision;
	float multiplier;

	// PEAK-PICKING
	float time_resolution; // the temporal detail of the onset peak picking algortithm - in milliseconds
	int resolution_frames;
	
	vector<int> onsets;
	float* prunned_flux;
	//FFT frames
	MatrixXf fft_data;

	
public:
	spectralFlux(int HOP_SIZE);
	// <!> NORMALIZE SAMPLE SET

	//<!> Create methods returning EigenVectors/Matrices
	void computeFFTData(short* samples, int length, bool scale=false);
	void computeFFTData(ofxMaxiSample* sample, bool scale=false);
	MatrixXf* getFFTData();
	
	//<!> MUST BE CALLED FOR EACH NEW SPECTRAL FRAME dictated by the HOP size of the FFT
	float  getFlux    (float* fft);
	float* getFluxSig (float* fft);

	// <OPTIM> these functions might take longer to compute - TODO perform benchmarks when assigning variables to Eigenvectors
	void computeFlux (short* samples, int length);
	void computeFlux (ofxMaxiSample* sample);	
	float*    getFluxHistory();
	MatrixXf* getFluxHistoryM();
	float getThreshold(int flux_frame);	
	void computeFluxThreshold();
	float* getFluxThresholdHistory();
	// set precision standard deviation
	void setPrecision(float p);
	void findFluxOnsets();
	vector<int>* getOnsets();
	void computePrunnedFlux();
	float* getPrunnedFlux();

	void computeFluxSig(short* samples, int length);
	void computeFluxSig(ofxMaxiSample* sample);
	float** getFluxSigHistory();

	//
	void updateTail();
	int getBinsSize();
};