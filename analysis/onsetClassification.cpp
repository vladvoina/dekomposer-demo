#include "onsetClassification.h"

#include <iostream>
using namespace std;


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

void onsetClassification::analyse(ofxMaxiSample* sample, int separation, int precision, bool skip, bool bijection)
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
	onsets_in_millis[i] = (unsigned long long) ((*flux->getOnsets())[i] * ((float)flux->getHopSize()/(float)44100) * 1000);
	//  cout << " --> " << ((*flux->getOnsets())[i] * ((float)flux->getHopSize()/(float)44100) * 1000) << endl;
  }

  // calculate feature space
  // <!> skip a few frames for long tracks
  MatrixXf data_chunk = flux->getOnsetsFFT()->topRows(fft_crop);

  // if track is long, there is not point training the system with the same onsets over and over
  // as it take too long to compute so only use sections of onsets for the training
  if (skip)
  {
	  int gap_size = 20;
	  int set_size = 10;
	  int long_head = 0;
	  int short_head = 0;
  
	  // run once just to find the size
	  while (long_head < data_chunk.cols()-set_size)
	  {
		  for(int i=long_head; i<long_head+set_size; i++)
		  {
		   short_head++;
		  }
	   long_head+= set_size + gap_size;
	  }
	  cout << "Reduced Training Onsets Set is of size: " << short_head+1 << endl;
	  MatrixXf data_chunk_skipped(data_chunk.rows(), short_head+1);

	  long_head = 0; short_head = 0;
	  // assign to new array
	  while (long_head < data_chunk.cols()-set_size)
	  {
		  for(int i=long_head; i<long_head+set_size; i++)
		  {
			  data_chunk_skipped.col(short_head) = data_chunk.col(i);
			  short_head++;
		  }
	   long_head+=set_size + gap_size;
	  }
	//
	  if (bijection) { pca->computeCovarianceMatrix2T(&data_chunk_skipped); }
	  else           { pca->computeCovarianceMatrix2 (&data_chunk_skipped); }
   } else
     {
      if (bijection) { pca->computeCovarianceMatrix2T(&data_chunk); }
	  else           { pca->computeCovarianceMatrix2 (&data_chunk); }
     }
  if (bijection)
  {
  pca->computeFeatureVectorT(precision);
  pca->transformData2T(&data_chunk);
  } else 
    {
     pca->computeFeatureVector(precision);
     pca->transformData2(&data_chunk);
    }
  
  // clustering
  kmeans->cluster(pca->getTransformedData(), separation);
}

vector<unsigned int>* onsetClassification::getOnsets() { return flux->getOnsets(); }
vector<unsigned long long>* onsetClassification::getOnsetsInMillis() {return &onsets_in_millis;}

int* onsetClassification::getIDs() { return kmeans->getClusterIDs(); }