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
	// <&> positive and negative flux
	//***********************************

	// FFT
	int WINDOW_SIZE;
	int BINS_SIZE; // half the window size
	int HOP_SIZE;
	ofxMaxiFFT fft;

	// SPECTRAL FLUX
	float* lastFFT;
	float diff; // difference between successive bins
	float* flux_sig; // for flux signature

	int flux_history_length; // dependent on the length of the samples
	RowVectorXf flux_history_matrix; // float*  flux_history; 
	//float** flux_sig_history;
	
	// THRESHOLDING
	int flux_average_length;
	int flux_std_length;

	float flux_threshold;
	float* flux_threshold_history; // maybe turn into eigen matrix
	float precision;
	float multiplier;
	MatrixXf buffer;

	// PEAK-PICKING
	float time_resolution; // the skip time of the onset peak picking algortithm - in milliseconds
	int resolution_frames;
	
	vector<unsigned int> onsets;
	float* prunned_flux;
	//FFT frames
	MatrixXf fft_data;
	MatrixXf onsets_fft;

	
public:
	spectralFlux(int HOP_SIZE, int WINDOW);
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
	void computeFluxOff();

	float*    getFluxHistory();
	RowVectorXf* getFluxHistoryM();
	float getThreshold(int flux_frame);
	float getThreshold2(int frame);
	void computeFluxThreshold(bool type=false);
	float* getFluxThresholdHistory();
	
	// PARAMETERS
	void setPrecision(float p); // the scaling of the standard deviation
	void setStdAvgLength(int l); // the buffer length
	void setMeanMult(float m); // scaling for the threshold multiplier
	void setMeanAvgLength(int l); // buffer length of the average

	void computePrunnedFlux();
	float* getPrunnedFlux();

	void findFluxOnsets();
	vector<unsigned int>* getOnsets();

	void computeOnsetsFFT();
	MatrixXf* getOnsetsFFT();

	int getBinsSize();
	int getHopSize();
	//
	void updateTail();
	void computeFluxSig(short* samples, int length);
	void computeFluxSig(ofxMaxiSample* sample);
	float** getFluxSigHistory();
};