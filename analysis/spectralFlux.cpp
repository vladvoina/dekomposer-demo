#include "spectralFlux.h"


spectralFlux::spectralFlux(int bins_size_, int tail)
{
	bins_size = bins_size_;

	diff = 0; // the difference between two succesive fft frames
	          // it will range between 0 and the maximum amplitude of the fft bins
	flux = new float[bins_size];
	lastFFT = new float[bins_size];

	fft.setup(bins_size*2, bins_size*2, bins_size);
//	fluxes = new float[tail][bins_size_];

}

float* spectralFlux::getFluxSig(float* fft)
{
	// <!> flux is being computed using both negative and positive direction
	// might be inappropriate for onset detection
	for(int i=0; i<bins_size;i++)
	{
	  diff = fft[i] - lastFFT[i];
 	  flux[i] = abs(diff);
	}
    for(int i=0; i<bins_size; i++) lastFFT[i] = fft[i];
/*
	// delay line 
	for (int i=1; i<tail_length; i++)
	{
     fluxes[i-1] = fluxes[i];
	}

	fluxes[tail_length-1] = flux;
*/
	return flux; //
}

float spectralFlux::getFlux(float* fft)
{
	// <!> flux is being computed using both negative and positive direction
	// might be inappropriate for onset detection
	float flux_t = 0;

	for(int i=0; i<bins_size;i++)
	{
	  flux_t += abs(fft[i] - lastFFT[i]);
    //= abs(diff);
	}

	flux_t /= bins_size;
  
    for(int i=0; i<bins_size; i++) lastFFT[i] = fft[i];

	return flux_t; //
}

void spectralFlux::processFlux(short* samples, int length)
{
  flux_history_length = (int)(length/bins_size);
  flux_history = new float[flux_history_length];
  
  int count = 0;
  for (long i=1; i<length; i++)
  {
	  fft.process(samples[i]/32767.0);
	 
	  if (i % bins_size == 0)
	  {
      //<!> first flux frame is VEERY HIGH
	  flux_history[count] = getFlux(fft.magsToDB());
	  count++;
 	  }  
  }
}

void spectralFlux::processFlux(ofxMaxiSample* sample)
{
	processFlux(sample->temp,sample->length); 
}

float* spectralFlux::getFluxHistory()
{
	return flux_history;
}


void spectralFlux::processFluxSig(short* samples, int length)
{


}



