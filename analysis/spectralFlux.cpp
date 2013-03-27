#include "spectralFlux.h"


spectralFlux::spectralFlux(int HOP_SIZE_)
{
	HOP_SIZE = HOP_SIZE_;
	WINDOW_SIZE = 1024; // for maximilian same as fft size
	BINS_SIZE = 512; // half the fft size?

	flux_average_length = 15; // keep in mind temporal constraints
	//init FFT
	fft.setup(WINDOW_SIZE, WINDOW_SIZE, HOP_SIZE);

	lastFFT = new float[BINS_SIZE]; for (int i=0; i<BINS_SIZE; i++) lastFFT[i] = 0; // initialize fft
	flux_sig = new float[BINS_SIZE];

	diff = 0; // the difference between two succesive fft frames
	          // it will range between 0 and the maximum amplitude of the fft bins

    flux_threshold = 0;
	precision = 0.0;
	multiplier = 1.2;

	time_resolution = 70;
	resolution_frames = (int) (time_resolution/(HOP_SIZE*1000/44100.0));
	cout << " resolution_frames: " << resolution_frames << endl;
}

void spectralFlux::setPrecision(float p) { precision = p; }

float spectralFlux::getFlux(float* fft)
{
	// <!> flux is being computed using both negative and positive direction
	// might be inappropriate for onset detection
	float flux_t = 0;

	for(int i=0; i<BINS_SIZE;i++)
	{
	  flux_t += abs(fft[i] - lastFFT[i]);
    //= abs(diff);
	}
	flux_t /= BINS_SIZE;

    for(int i=0; i<BINS_SIZE; i++) lastFFT[i] = fft[i];

	return flux_t; //
}

void spectralFlux::computeFlux(short* samples, int length)
{

  flux_history_length = (int)(length/HOP_SIZE) + 1; //<!> some matching problems might arise here
  flux_history_matrix = RowVectorXf(flux_history_length);
  
  // for optimization - to avoid an if-statement check each itteration
  int count = 0;
  for (int i=0; i<WINDOW_SIZE; i++)
  {
	  fft.process(samples[i]/32767.0);
	  if (i % HOP_SIZE == 0)
	  {
	   flux_history_matrix(count) = 0; // no flux for first few frames until a window is processed
	   count++;	 
	  }
  }   
  for (long i=WINDOW_SIZE; i<length; i++)
  {
	  fft.process(samples[i]/32767.0); // <OPTIM> process method to be passed in larger chunks of data
	  
	  if (i % HOP_SIZE == 0) // <OPTIM> maybe different if stamement
	  {
       //<!> first flux frame is VEERY HIGH - TO FIX
	   flux_history_matrix(count) = getFlux(fft.magsToDB());
	   count++;
 	  }  
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

MatrixXf* spectralFlux::getFluxHistoryM()
{
 return &flux_history_matrix;
}

float spectralFlux::getThreshold(int frame)
{
	// to avoid stepping out of the array at the beggining of analysis
	if (frame-flux_average_length >= 0)
	{
	//<OPTIM> avoid this array creation
	//<OPTIM> set maxMatrix size - look into Eigen reference
	MatrixXf buffer = flux_history_matrix.block(0, frame-flux_average_length, 1, flux_average_length); 
	pca.process(&buffer);
	//<!> standard deviation only makes sense when calculated across larger buffers
	flux_threshold = pca.getMean() * multiplier + pca.getStandardDeviation() * precision;
	} else // <!> can be improved so that the buffer increases gradually
	  { flux_threshold = 0; }

	return flux_threshold; // <!> maybe return as pointer
}

void spectralFlux::computeFluxThreshold()
{
	flux_threshold_history = new float[flux_history_length];

	for(int i=0; i<flux_history_length; i++)
	{
		flux_threshold_history[i] = getThreshold(i);
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
	/*
	// the number of flux frames, calculated depending on the resolution
	float prunned_flux0, prunned_flux1;
	//
	int count=0;
	while (count<flux_history_length-1)
	{
		prunned_flux0 = flux_history_matrix(count) - getThreshold(count);
		prunned_flux1 = flux_history_matrix(count+1) - getThreshold(count+1);
		if(prunned_flux0 > prunned_flux1)
		{// ONSET DETECTED
         // store onset index
	    onsets.push_back(count);
 		//count+= resolution_frames;
		continue;
		}
	 count++;
	}
	*/
	int count=0;
	while (count<flux_history_length-1)
	{
		if(prunned_flux[count] > prunned_flux[count+1])
		{// ONSET DETECTED
         // store onset index
	    onsets.push_back(count);
		//count++;
 		count+= resolution_frames;
		continue;
		}
	 count++;
	}
}

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
