#pragma once

#include "ofxMaxim.h"

#include "spectralFlux.h"
#include "PCA.h"
#include "kMeans.h"

class onsetClassification {

public:
	onsetClassification(int hop_size, int window_size);
	~onsetClassification();

	// to pass in multiplier, std avg, etc
	void setOnsetDetectionParams();
	void setFFTCrop(int crop);
	// separation is the number of clusters, precision is the number of dim used in pca
	void analyse(ofxMaxiSample* sample, int separation, int precision);
	//
	vector<unsigned int>*				getOnsets();
	vector<unsigned long long>* getOnsetsInMillis();
	int* getIDs();
	
private:
	//vector<int> onsets;
	int fft_crop;
	vector<unsigned long long> onsets_in_millis;

	spectralFlux* flux;
	PCA* pca;
	kMeans* kmeans;



};