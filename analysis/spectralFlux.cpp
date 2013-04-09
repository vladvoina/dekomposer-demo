#include "spectralFlux.h"

/// Initialize variables
spectralFlux::spectralFlux(int HOP_SIZE_, int WINDOW)
{
	HOP_SIZE = HOP_SIZE_;
	WINDOW_SIZE = WINDOW; // for maximilian same as fft size
	BINS_SIZE = WINDOW_SIZE/2; // half the fft size
	//init FFT
	fft.setup(WINDOW_SIZE, WINDOW_SIZE, HOP_SIZE);
	lastFFT = new float[BINS_SIZE]; for (int i=0; i<BINS_SIZE; i++) lastFFT[i] = 0; // initialize fft
	flux_sig = new float[BINS_SIZE];
	diff = 0; // the difference between two succesive fft frames
	          // it will range between 0 and the maximum amplitude of the fft bins
	/*************************************************************/
	flux_average_length = 15; // the number of flux frames over which the mean for the threshold is calculated
	flux_std_length = 30; // the number of flux frames over which the standard deviation for the threshold is calculated
    flux_threshold = 0; // init
	precision = 0.0; // the scaling of the standard deviation
	multiplier = 1.2; // scaling for the mean to avoid detecting small peaks over sections with very low energy
	//<!> does not work well with tunes that have very high energy
	 
	time_resolution = 70; // the minimum time separation between succesive onsets
	resolution_frames = (int) (time_resolution/(HOP_SIZE*1000/44100.0));
	cout << " resolution_frames: " << resolution_frames << endl;
}

int spectralFlux::getBinsSize() { return BINS_SIZE; }

// pre-compute fft frames for an audio file
void spectralFlux::computeFFTData(short* samples, int length, bool scale)
{
	// 
	int observations = 0;
	observations = (int)(length/HOP_SIZE)+1;
	cout << "The FFT array has " << observations << " frames." << endl;
	fft_data = MatrixXf(BINS_SIZE, observations);
	//VectorXf fft(BINS_SIZE, 1); 
	Map<MatrixXf> fft_(fft.magsToDB(), BINS_SIZE, 1);
	int count = 0;
	
	for (long i=0; i<length; i++)
    {
	  fft.process(samples[i]/32767.0); // <OPTIM> process method to be passed in larger chunks of data
	  if (i % HOP_SIZE == 0) // <OPTIM> maybe different if stamement
	  {
	     fft.magsToDB();
	     fft_data.col(count) = fft_;
		 // adjust range to vary from 0 to 1
		 if (scale) fft_data.col(count) = fft_data.col(count).array() / (float) fft_data.col(count).maxCoeff();
	     count++;
 	  }  
    }	
}

void spectralFlux::computeFFTData(ofxMaxiSample* sample, bool scale)
{
	computeFFTData(sample->temp,sample->length, scale); 
}

MatrixXf* spectralFlux::getFFTData() { return &fft_data; }


void spectralFlux::setPrecision(float p) { precision = p; }
void spectralFlux::setStdAvgLength(int l) { flux_std_length = l; }
void spectralFlux::setMeanMult(float m) { multiplier = m; }
void spectralFlux::setMeanAvgLength(int l) {flux_average_length = l; }

// realtime function for calculating flux
float spectralFlux::getFlux(float* fft)
{
	// <!> flux is being computed using both negative and positive direction
	// might be inappropriate for onset detection
	float flux_t = 0;
	for(int i=0; i<BINS_SIZE;i++)
	{
	  flux_t += abs(fft[i] - lastFFT[i]);
	}
	flux_t /= BINS_SIZE;
    for(int i=0; i<BINS_SIZE; i++) lastFFT[i] = fft[i];
	return flux_t; //
}

// stores spectral flux function into a vector
void spectralFlux::computeFlux(short* samples, int length)
{

  flux_history_length = (int)(length/HOP_SIZE) + 1; //<!> some matching problems might arise here
  flux_history_matrix = RowVectorXf(flux_history_length);
  
  // the for loops are separated for optimization - to avoid an if-statement check each itteration
  int count = 0;
  for (int i=0; i<WINDOW_SIZE; i++)
  {
	  // we need two fft frames to calculate the flux so wait until a window is filled
	  fft.process(samples[i]/32767.0);
	  if (i % HOP_SIZE == 0)
	  {
	   flux_history_matrix(count) = 0; // no flux for first few frames until a window is processed
	   count++;	 
	  }
  }   
  for (long i=WINDOW_SIZE; i<length; i++)
  {
	  fft.process(samples[i]/32767.0); // <OPTIM> process method to be passed in buffers of audio instead of single samples
	  
	  if (i % HOP_SIZE == 0) // <OPTIM> maybe different if stamement
	  {
       //<!> first flux frame is VEERY HIGH - TO FIX
	   flux_history_matrix(count) = getFlux(fft.magsToDB());
	   count++;
 	  }  
  }
}

// non-realtime method for cumputing the flux function
void spectralFlux::computeFluxOff()
{
  flux_history_length = fft_data.cols();//(int)(length/HOP_SIZE) + 1; //<!> some matching problems might arise here
  flux_history_matrix = RowVectorXf(flux_history_length);
  flux_history_matrix(0) = 0;
  
  for (int i=1; i<flux_history_length; i++)
  {
	 flux_history_matrix(i) = (fft_data.col(i) - fft_data.col(i-1)).array().abs().sum()/BINS_SIZE;
  }
}

void spectralFlux::computeFlux(ofxMaxiSample* sample)
{
	computeFlux(sample->temp,sample->length); 
}

float* spectralFlux::getFluxHistory()
{
	return &flux_history_matrix(0);
}

RowVectorXf* spectralFlux::getFluxHistoryM()
{
 return &flux_history_matrix;
}

float spectralFlux::getThreshold(int frame)
{
	// to avoid stepping out of the array at the beggining of analysis
	if (frame-flux_std_length >= 0)
	{
	//<OPTIM> avoid this array creation
	//<OPTIM> set maxMatrix size - look into Eigen reference
	buffer = flux_history_matrix.block(0, frame-flux_std_length, 1, flux_std_length); 
	//pca.process(&buffer);
	//<!> standard deviation only makes sense when calculated across larger buffers
	//flux_threshold = buffer.mean() * multiplier + pca.getStandardDeviation() * precision;
	flux_threshold = flux_history_matrix.block(0, frame-flux_average_length, 1, flux_average_length).mean() * multiplier +
		             PCA::getStdDev(&buffer) * precision;
	} else // <!> can be improved so that the buffer increases gradually
	  { flux_threshold = 1000; } // set to a value that wil never be reached

	return flux_threshold; // <!> maybe return as pointer
}

// will calculate the threshold around the frame - half of before, half ahead
float spectralFlux::getThreshold2(int frame)
{
	// to avoid stepping out of the array at the beggining and end of flux array
	if (frame-flux_std_length >= 0 && (frame+(flux_average_length/2)+1) < flux_history_matrix.cols())
	{
	//<OPTIM> avoid this array creation
	//<OPTIM> set maxMatrix size - look into Eigen reference
	buffer = flux_history_matrix.block(0, frame-flux_std_length, 1, flux_std_length); 
	//pca.process(&buffer);
	//<!> standard deviation only makes sense when calculated across larger buffers
	//flux_threshold = buffer.mean() * multiplier + pca.getStandardDeviation() * precision;
	flux_threshold = flux_history_matrix.block(0, frame-(int)(flux_average_length/2), 1, flux_average_length).mean() * multiplier +
		             PCA::getStdDev(&buffer) * precision;
	} else // <!> can be improved so that the buffer increases gradually
	  { flux_threshold = 1000; } // set to a threshold that will never be reached

	return flux_threshold; // <!> maybe return as pointer
}

void spectralFlux::computeFluxThreshold(bool type)
{
	flux_threshold_history = new float[flux_history_length];
	if (!type)
	{
		for(int i=0; i<flux_history_length; i++)
		{
			flux_threshold_history[i] = getThreshold(i);
		}
	} else
	{
        for(int i=0; i<flux_history_length; i++)
		{
			flux_threshold_history[i] = getThreshold2(i);
		}
	}
}

float* spectralFlux::getFluxThresholdHistory() { return flux_threshold_history; }

void spectralFlux::computePrunnedFlux()
{
  prunned_flux = new float[flux_history_length];

  for (int i=0; i<flux_history_length; i++)
  {
    if (flux_history_matrix(i) > flux_threshold_history[i])
	{
    prunned_flux[i] = flux_history_matrix(i) - flux_threshold_history[i];
	} else
	  {
       prunned_flux[i] = 0;
	  }
  }
}

float* spectralFlux::getPrunnedFlux() { return prunned_flux; }

void spectralFlux::findFluxOnsets()
{
	onsets.clear();
	int count=0;
	while (count<flux_history_length-1)
	{
		if(prunned_flux[count] > prunned_flux[count+1])
		{// ONSET DETECTED
         // store onset index
	    onsets.push_back(count);
		//count++;
 		count+= resolution_frames; // resolution frames is the number of frames skipped (under the assumption
		                           // that there will never be more than one onset in a certain interval
		continue;
		}
	 count++;
	}
}

// grabs the fft frames for associated with each onset
void spectralFlux::computeOnsetsFFT()
{
	onsets_fft = MatrixXf(fft_data.rows(), onsets.size());

	for (int i=0; i<onsets.size();i++)
	{
		if ((onsets[i]+1) >= 0 && (onsets[i]+1) <= fft_data.cols())
		{
		onsets_fft.col(i) = fft_data.col(onsets[i]+1);
		} else
		  {
		   onsets_fft.col(i) = VectorXf::Zero(onsets_fft.rows());
		  }
	}
}

MatrixXf* spectralFlux::getOnsetsFFT() { return &onsets_fft; }


vector<int>* spectralFlux::getOnsets() { return &onsets; }


// <!> revise thorughly - bins_size variable
float* spectralFlux::getFluxSig(float* fft)
{
	// <!> flux is being computed using both negative and positive direction
	// might be inappropriate for onset detection
	for(int i=0; i<BINS_SIZE;i++)
	{
	  diff = fft[i] - lastFFT[i];
 	  flux_sig[i] = abs(diff);
	}
    for(int i=0; i<BINS_SIZE; i++) lastFFT[i] = fft[i];
/*
	// delay line 
	for (int i=1; i<tail_length; i++)
	{
     fluxes[i-1] = fluxes[i];
	}

	fluxes[tail_length-1] = flux;
*/
	return flux_sig; //
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/*
void spectralFlux::processFlux(short* samples, int length)
{
  flux_history_length = (int)(length/bins_size);
  flux_history = new float[flux_history_length];
  
  int count = 0;
  for (long i=1; i<length; i++)
  {
	  fft.process(samples[i]/32767.0);
	  //<!> CHANGE TO HOP SIZE
	  if (i % bins_size == 0)
	  {
      //<!> first flux frame is VEERY HIGH - TO FIX
	  flux_history[count] = getFlux(fft.magsToDB());
	  count++;
 	  }  
  }
}

void spectralFlux::processFlux(ofxMaxiSample* sample)
{
	processFlux(sample->temp,sample->length); 
}

*/
