#pragma once

#include <cmath>
#include "ofxMaxim.h"

class spectralFlux {
private:
	// <!> nu flux history yet
	
	// <!> make sure you are computing the flux in steps as close as possible to eachother

	// <!> be careful when reading stero signal with panning
	// <!> maybe analyse panning

	// <&> add flux grouping
	// <&> average thresholding
	// <&> add several types of flux analysers depending on the outcome or application
	// <&> positive and negative flux

	//***********************************
	ofxMaxiFFT fft;

	int bins_size;
	float diff; // difference of individual bins
		
	float* flux;
	float* lastFFT;

	int flux_history_length; // dependent on the length of the samples
	float*  flux_history;
	float** flux_sig_history;
	/* thresholding */
    int tail_length;
	float** fluxes;
	
public:
	spectralFlux(int bins_size_, int tail_length=5);

	//<!> Create methods returning EigenVectors/Matrices

	//<!> MUST BE CALLED FOR EACH NEW SPECTRAL FRAME
	float* getFluxSig (float* fft);
	float  getFlux    (float* fft);

	void processFlux    (short* samples, int length);
	void processFluxSig (short* samples, int length);
	void processFlux    (ofxMaxiSample* sample);
	void processFluxSig (ofxMaxiSample* sample);
	
	//<&> add return as EigenVectors
	float*  getFluxHistory();
	float** getFluxSigHistory();

	void updateTail();
	
	float* getThreshold();
	
};