#ifndef Com_H
#define Com_H
#define __WINDOWS_ASIO__
#include "RtAudio.h"
//#include "nr_algorithm1.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
using namespace std;
#define epsmatlab 2.2204e-016

typedef double  MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64


//Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif
  ////////////////////////////
struct InputData 
  {
	  MY_TYPE* buffer;
	  unsigned long bufferBytes;
	  unsigned long totalFrames;
	  unsigned long frameCounter;
	  unsigned int channels;
	};
/////////////////////////////////////////////
  int input( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
  double streamTime, RtAudioStreamStatus status, void *data )

{
	  InputData *iData = (InputData *) data;
		//iData->buffer=NULL;
	  // Simply copy the data to our allocated buffer.
	  unsigned int frames = nBufferFrames;
	  if ( iData->frameCounter + nBufferFrames > iData->totalFrames ) {
		frames = iData->totalFrames - iData->frameCounter;
		iData->bufferBytes = frames * iData->channels * sizeof( MY_TYPE );
	  }

	  unsigned long offset = iData->frameCounter * iData->channels;
	  memcpy( iData->buffer+offset, inputBuffer, iData->bufferBytes );
	  iData->frameCounter += frames;

	  if ( iData->frameCounter >= iData->totalFrames ) return 2;
	  return 0;
};

  
 
///////////////////////////////////////////////
class jin_record
{
public:
	unsigned int channels;
	unsigned int fs;
	unsigned int bufferFrames;
	unsigned int device;
	unsigned int offset ;
  
	double time;
	RtAudio adc;
  double **source;
  
	jin_record()  // default construct act as a main here
	{
		
			
		
		this->channels=4;
		this->fs=44100;
		this->bufferFrames=512;
		this->device = 0;
		this-> offset = 0;
		this->time=3.0;
		this->source = (double **) malloc(44100*2 *sizeof(double));
    for(int iRow=0;iRow < 44100*2;iRow++){
			this->source[iRow] = (double *) malloc(4*sizeof(double));
      for(int clo=0;clo<4;clo++)
          this->source[iRow][clo]=0;
		  }
	};
	 
  double ** get_source_data(){return source;};
  //////////////////////////////////////////////
  
 
	////////////////////////////
  /////////////////////////////////////////////
	~jin_record(){};
	///////////////////////////
  void  data_recording()
  {
	
	  RtAudio adc;
	  if ( adc.getDeviceCount() < 1 )
	  {
				std::cout << "\nNo audio devices found!\n";
				exit( 1 );
	  }
	  adc.showWarnings( true );
	  RtAudio::StreamParameters iParams;
	  iParams.deviceId = device;
	  iParams.nChannels = channels;
	  iParams.firstChannel = offset;

	  InputData data;
	  
	  data.buffer = NULL;
	  try {
	
			adc.openStream( NULL, &iParams, FORMAT, fs, &bufferFrames, &input, (void *)&data );
		  }
		  catch ( RtError& e )
		  {
			 std::cout << '\n' << e.getMessage() << '\n' << std::endl;
			goto cleanup;
       	}

	  data.bufferBytes = bufferFrames * channels * sizeof( MY_TYPE );
	  data.totalFrames = (unsigned long) (fs * time);
	  data.frameCounter = 0;
	  data.channels = channels;
	  unsigned long totalBytes;
	  totalBytes = data.totalFrames * channels * sizeof( MY_TYPE );

	  // Allocate the entire data buffer before starting stream.
	  data.buffer = (MY_TYPE*) malloc( totalBytes );
	  
	  adc.startStream();
  

	  std::cout << "\nRecording for " << time << " seconds "<<"(buffer frames = " << bufferFrames << ")." << std::endl;
	  while ( adc.isStreamRunning() ) {
		SLEEP( 100 ); // wake every 100 ms to check if we're done
	  }
	 cout<<"recording finished "<<endl;
	 for (int i=8000,j=0;i<2*44100*4+8000,j<2*44100;j++,i=i+4)
			{
				source[j][0]= data.buffer [i]*100;
				source[j][1]= data.buffer [i+1]*100;
				source[j][2]= data.buffer [i+2]*100;
				source[j][3]= data.buffer [i+3]*100;
				
			
			}
   
	 cleanup:
		  if ( adc.isStreamOpen() ) adc.closeStream();
		  if ( data.buffer ) free( data.buffer );
		 
	   
	};
  
  
};
 
  

	/////////////////////////////////////////
#endif