#ifndef VAD_H
#define VAD_H
#include <complex>
#include <fftw3.h>
#include <math.h>
#include <iostream>

using namespace std;

class vad_detector{// voice active detector with logrithm algorithm
	
public:
	int  NoiseMargin;
	int  Hangover;
	int  NoiseCounter;
    int  size;
	int  NoiseFlag;
	int  SpeechFlag;
	double Dist;

	vad_detector( int size,double * signal,double *noise,int NoiseCounter){
		  this->NoiseMargin=3;
		  this->Hangover=8;
	      this->NoiseCounter=NoiseCounter;
		  this->NoiseFlag=0;
		  this->SpeechFlag=0;
		  this->Dist=0.0;

		  this->size=size;
		  
		  double sum_tmp=0.0;
		  ///////////////begin vad part////////////////
		  double *SpectralDist=new double[size];
		  int count=0;
		  for (int n=0;n<size;n++)
		  {
			SpectralDist[n]= 20*(log10(signal[n])-log10(noise[n]));// test for the spectral distance between noise and signal

			if (SpectralDist[n]<0){
					SpectralDist[n]=0;
					count++;
			}
			sum_tmp=sum_tmp+SpectralDist[n]; 

		  }
		 Dist=sum_tmp/(size-count);
		 if (Dist < NoiseMargin) {// decision making part
				setNoiseFlag(1); 
				NoiseCounter=NoiseCounter+1;
			}
		 else{
				setNoiseFlag(0);
				setNoiseCounter(0);
			}
		

		//% Detect noise only periods and attenuate the signal     
		if (NoiseCounter > Hangover) 
				 this->SpeechFlag=0;    
		else 
				 this->SpeechFlag=1; 
		 
	};
	////////////////
	~vad_detector(){};

	int getSpeechFlag()
		{ return SpeechFlag;}

	int getNoiseCounter()
		{return NoiseCounter;}

	int getNoiseFlag()
		{return NoiseFlag;}

	double getDist()
		{return Dist;}

	void setSpeechFlag(int SpeechFlagin)
		{ 
			SpeechFlag=SpeechFlagin;
		}
	void setNoiseCounter(int NoiseCounterin)
		{ 
			NoiseCounter=NoiseCounterin;
		}
	void setNoiseFlag(int NoiseFlagin)
		{ 
			NoiseFlag=NoiseFlagin;
		}



};
#endif