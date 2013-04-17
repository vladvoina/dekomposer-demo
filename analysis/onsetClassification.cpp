#include "onsetClassification.h"


onsetClassification::onsetClassification(int hop_size, int window_size)
{
  flux = new spectralFlux(hop_size, window_size);
  pca = new PCA();
  kmeans = new kMeans();

  setFFTCrop(flux->getBinsSize()/2); // default fft crop
}

void onsetClassification::setOnsetDetectionParams()
{

}

void onsetClassification::setFFTCrop(int crop)
{
	fft_crop = crop;
}

void onsetClassification::analyse(ofxMaxiSample* sample, int separation, int precision)
{
  // detect onsets
  flux->computeFFTData(sample);
  flux->computeFluxOff();
  flux->computeFluxThreshold();
  flux->computePrunnedFlux();
  flux->findFluxOnsets();
  flux->computeOnsetsFFT();
  // calculate onsets in millis
  onsets_in_millis.resize(flux->getOnsets()->size());
  for (int i=0; i<onsets_in_millis.size();i++)
  {
	  onsets_in_millis[i] = (unsigned long long) ((*flux->getOnsets())[i] * (flux->getHopSize()/44100) * 1000);
  }

  // calculate feature space
  // <!> skip a few frames for long tracks
  MatrixXf data_chunk = flux->getOnsetsFFT()->topRows(fft_crop);
  
  pca->computeCovarianceMatrix2T(&data_chunk);
  pca->computeFeatureVectorT(precision);
  pca->transformData2T(&data_chunk);
  
  // clustering
  kmeans->cluster(pca->getTransformedData(), separation);
}

vector<unsigned int>* onsetClassification::getOnsets() { return flux->getOnsets(); }
vector<unsigned long long>* onsetClassification::getOnsetsInMillis() {return &onsets_in_millis;}

int* onsetClassification::getIDs() { return kmeans->getClusterIDs(); }