//#ifndef NEA_H
//#define NEA_H

//#include "Localize_alg.h"

#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <algorithm>
//#include <stdlib.h>
#include  <time.h>
#include "jin_record_paper.h"
#include  "vad_jin2.h"
#include  "segment2.h"
#include  "overlapadd2.h"

using namespace std;

class NRA1
{
  public: 
    ////////////////////////
    clock_t start;
    clock_t afterread;
    clock_t  finish;
    jin_record *record_obj11;
    double **source_in;
    int *maxIndex;
    double ** source_out;
    //////////////////////////
    NRA1();
    void setMaxIndex(double begin, double end, double indexsize)
    {
      maxIndex[0]=begin;
      maxIndex[1]=end;
      maxIndex[2]=indexsize;
    };
    void setSourceOut(double **source_in,int *maxIndex)
    {
       cout<<"set source out "<<maxIndex[0]<<" "<<maxIndex[1]<<" "<<maxIndex[2]<<endl;
       this->source_out = (double **) malloc(maxIndex[2]*sizeof(double));
		   for(int tmp_maxIndex=maxIndex[0], iRow=0;tmp_maxIndex<maxIndex[1],iRow < maxIndex[2];tmp_maxIndex++,iRow++)
				{
         this->source_out[iRow] = (double *) malloc(4*sizeof(double));
         for (int j=0;j<4;j++)
             source_out[iRow][j]=source_in[tmp_maxIndex][j];
        }
    };
    double** GetSourceOut(){return source_out;};
    int *     GetMaxIndex()
    {
        return maxIndex;
    };
      
    ~NRA1(){};




};

NRA1::NRA1()
{

   
///////////read data////////////
    start=clock();
    maxIndex=new int [3];
    record_obj11=new jin_record();
    record_obj11->data_recording();
    const int size=2*44100;
    this->source_in = (double **) malloc(size*sizeof(double));
		for(int iRow=0;iRow < size;iRow++)
				this->source_in[iRow] = (double *) malloc(4*sizeof(double));
    double *signal=new double[size];
    source_in=record_obj11->get_source_data();
    for(int iRow=0;iRow < size;iRow++)
     signal[iRow]=source_in[iRow][0];

  /*  char * inname = "ls_data_ori_44100.txt";
      ifstream infile(inname);
    for(int i=0;i<size;i++)
	    infile >> signal[i];
      */
    //cout<<"signal "<<signal[11025*2-1]<<endl;
    afterread=clock();

/////////////////////////
  double IS=0.25;
  int     fs=44100;
  int     W=512;//floor(.025*fs); //Window length is 25 ms
  double  SP=0.4;
//////////////////////


  int NIS=10;//floor((IS*fs-W)/(SP*W) +1);//number of initial silence 
 cout<<"NIS "<<NIS<<endl;
 Segment_class *seg_obj;
 seg_obj=new Segment_class(size,signal,W,SP);
 const int N=seg_obj->Get_N();
 cout<<"seg finish "<<endl;
 double **y;
 y = (double **) malloc(W *sizeof(double));
 for(int iRow=0;iRow < W;iRow++)
	  y[iRow] = (double *) malloc(N*sizeof(double));

  y=seg_obj->Get_Segment();
//cout<<y[0][0]<<" "<<y[0][1]<<endl;
///////////////////////
 int hal_W=(int)floor(1+(double)W/2);
 cout<<"hal_w "<<hal_W<<endl;
 double **YPhase,**Y,**X,**xi;
// Y=new double();
 YPhase = (double **) malloc(hal_W *sizeof(double));
 for(int iRow=0;iRow < hal_W;iRow++)
	YPhase[iRow] = (double *) malloc(N*sizeof(double));
 ///////////////////////////////////////////////////
 Y = (double **) malloc(hal_W *sizeof(double));
 X = (double **) malloc(hal_W *sizeof(double));
 for(int iRow=0;iRow < hal_W;iRow++)
	Y[iRow] = (double *) malloc(N*sizeof(double));
 for(int iRow=0;iRow < hal_W;iRow++)
	X[iRow] = (double *) malloc(2*N*sizeof(double));
 xi = (double **) malloc(hal_W *sizeof(double));
 for(int iRow=0;iRow < hal_W;iRow++)
	xi[iRow] = (double *) malloc(2*sizeof(double));
////////////fft////////////

//int SIZE=(seg_obj->Get_N())*W;
//double          input[SIZE] = { 3.4, 2.5, 1.345, 9.0123 };
//float	(*output)[2]=new float[W*N][2];
for(int j=0;j<N;j++)
{
	fftw_complex    *data, *fft_result;
	fftw_plan       plan_forward;
   
	data        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * W);
		fft_result  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * W);
	   // ifft_result = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );	
		plan_forward  = fftw_plan_dft_1d(W, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );
	//fftw_plan_dft_2d(src->height,src->width, in, out, FFTW_FORWA

	//plan_backward = fftw_plan_dft_1d( SIZE, fft_result, ifft_res

	 for (int i=0; i<W; i++) {
		//for (int j=0; j<N; j++) {
			data[i][0] = y[i][j];
			data[i][1] = 0;
	   //  }
	 }
   
	fftw_execute( plan_forward );
	
	//showresult(data, fft_result,W,N);
	fftw_destroy_plan( plan_forward );
	fftw_free( data );
	
	
//cout<<fft_result[0][0]<<" i"<<fft_result[0][1]<<" "<<fft_resul
////////////////////////////

//double YPhase[hal_W][N];
	for(int i=0;i<hal_W;i++)
	{

		YPhase[i][j]= atan2((double)fft_result[i][1],(double)fft_result[i][0]);
		Y[i][j]=sqrt(pow((double)fft_result[i][0],2)+pow((double)fft_result[i][1],2));
		
	

	}
	fftw_free( fft_result );
}
cout<<"phase check "<<YPhase[0][0]<<" "<<YPhase[hal_W-1][N-1]<<endl;
cout<<"y check "<<Y[0][0]<<" "<<Y[0][1]<<" "<<Y[hal_W-1][N-1]<<endl;
//NIS=5;
const int	numberOfFrames=N;
int FreqResol=hal_W;
//hal_W=3;



    double *N_mean,*LambdaD, *Gamma;
    LambdaD=new double[hal_W];
    Gamma=new double[hal_W];
    //xi=new double[hal_W];
    N_mean=new double[hal_W];


//int num_nis=W/2+1;

for(int row=0;row<hal_W;row++)
{
	N_mean[row]=0;
	LambdaD[row]=0;
	//cout<<"in loop"<<endl;
	for(int i=0;i<NIS;i++)
	{
	
		N_mean[row]=N_mean[row]+Y[row][i];
		//cout<<" loop"<<endl;
		LambdaD[row]=LambdaD[row]+pow(Y[row][i],2);
	}
		N_mean[row]=N_mean[row]/NIS;
		//cout<<" out"<<row<<endl;
		LambdaD[row]=LambdaD[row]/NIS;


}

double alpha=0.99;//used in smoothing xi (For Deciesion Directed
int PowExp=2; // Power Exponent
double Beta=0.5;//(gamma(PowExp+1)-(pow((double)gamma(1+PowExp/2

cout<<"beta "<<Beta<<endl;
int NoiseCounter=0;
int NoiseLength=9;//This is a smoothing factor for the noise upd
double mu=0.05;
int NoiseFlag=0;
int SpeechFlag=0;
double Dist=0.0;
double s=0.0;
double tmp_re=0.0;
double tmp_im=0.0;
double tmp_mag=0.0;
double DEN=0.0;
//////////main loop
for (int i=0;i<N;i++)
{
	if (i<=NIS) // If initial silence ignore VAD
      
	{
		 SpeechFlag=0;
		 NoiseCounter=100;
	}
	else
	{
		double *tmp_signal=new double[hal_W];
		for(int row=0;row<hal_W;row++)
		{
			tmp_signal[row]=Y[row][i];
			//tmp_signal=&Y;
		}
		
		vad_detector *obj;
		obj=new vad_detector(hal_W,tmp_signal,N_mean,NoiseCounter);
		NoiseFlag=obj->getNoiseFlag();
		SpeechFlag=obj->getSpeechFlag();
		//SpeechFlag=1;//rand()%2;
		//cout<<"speech flag "<<SpeechFlag<<endl;
		NoiseCounter=obj->getNoiseCounter();
		Dist=obj->getDist();
		delete obj;
		delete tmp_signal;
	
	}
	//cout<<"mean test "<<N_mean[10]<<" "<<LambdaD[10]<<endl;
	if (SpeechFlag==0) //If not Speech Update Noise Parameters
	{
		for(int row=0;row<hal_W;row++)
		{
			N_mean[row]=(NoiseLength*N_mean[row]+Y[row][i])/(NoiseLength+1); //Update and smooth noise mean
				LambdaD[row]=(NoiseLength*LambdaD[row]+pow(Y[row][i],2))/(1+NoiseLength);
		    //cout<<"mean test "<<row<<" "<<N_mean[row]<<" "<<Lambda
		}
		
	}
	//cout<<"mean test "<<N_mean[10]<<" "<<LambdaD[10]<<endl;
	//cout<<"finish speech update "<<endl;
//////////////////////////////
	for(int row=0;row<hal_W;row++)
	{
		Gamma[row]=pow(Y[row][i],2)/LambdaD[row]; //A postiriori SNR
	}
	//cout<<"finish posti snr "<<endl;
	if (i>0)
	{
		for(int row=0;row<hal_W;row++)
		{
			//xi[row][0]=(alpha*pow(X[row][i-1],2) + (1-alpha)*max((po
			//for (int col_n=0;col_n<2*N;col_n=col_n+2){
				xi[row][0]=(alpha*(pow(X[row][2*(i-1)],2)-pow(X[row][2*(i-1)+1],2))+ (1-alpha)*max((pow(Y[row][i],2) - LambdaD[row]),0.0))/LambdaD[row];
			//}
		}
	}
	
	else
	{
		for(int row=0;row<hal_W;row++)
		{
			xi[row][0]=(Gamma[row]+1)/2;
			xi[row][1]=0;
		}
	}
	//cout<<"xi update "<<endl;
	//////////////
	for(int row=0;row<hal_W;row++)
	{
		//if((pow(Y[row][i],PowExp) -pow( N_mean[row],PowExp))<0)
		//{ // here (a.^2+b.^2).^2+beta*(a.^2-b.^2)+i*{2*a*b*beta}
			DEN=pow( (pow(xi[row][0],2)- pow(xi[row][1],2) +Beta ),2)+4*pow(xi[row][0],2)*pow(xi[row][1],2);// trick complex calculation
			s=pow(Y[row][i],2) -pow( N_mean[row],2);
			tmp_re=s*(pow (pow(xi[row][0],2)+pow(xi[row][1],2),2)+Beta*( pow(xi[row][0],2)-pow(xi[row][1],2)))/DEN; 
			tmp_im=s*( 2*xi[row][0]*xi[row][1]*Beta)/DEN;
			tmp_mag=sqrt(pow(tmp_re,2)+pow(tmp_im,2));
			
			if(tmp_im>=0)//complex sqrt 
			{
				X[row][2*i]=pow((tmp_mag+tmp_re)/2,0.5);  //pow( ( )*(po
				X[row][2*i+1]=pow((tmp_mag-tmp_re)/2,0.5);//pow( ( )*(po
			}
			else
			{
				X[row][2*i]=pow((tmp_mag+tmp_re)/2,0.5);  //pow( ( )*(po
				X[row][2*i+1]=-pow((tmp_mag-tmp_re)/2,0.5);
			
			}
			s=0.0;
			tmp_re=0.0;
			tmp_mag=0.0;	
			tmp_im=0.0;
			DEN=0.0;
			
	}
	//" "<<xi[2][0]<<" "<<xi[2][1]<<endl;
	if (i>0)
	{
		for(int row=0;row<hal_W;row++)
		{
			double X_modul=pow(X[row][2*i],2)+pow(X[row][2*i+1],2);
				double Y_modul=pow(0.5*(mu*Y[row][i]+X[row][2*(i-1)]),2)+pow(0.5*(X[row][2*(i-1)+1]),2);
			if(X_modul>Y_modul)
			{
				X[row][2*i]=X[row][2*i];//max( X[row][i],0.5*(mu*Y[row][
				X[row][2*i+1]=X[row][2*i+1];
			}
			else
			{	
				X[row][2*i]=0.5*(mu*Y[row][i]+X[row][2*(i-1)]);
				X[row][2*i+1]=0.5*(X[row][2*(i-1)+1]);

			}
		}
	}
	//cout<<"Xi  "<<xi[0][0]<<" "<<xi[0][1]<<endl;	
//	cout<<"X last  "<<X[0][0]<<" "<<X[0][1]<<" "<<X[1][0]<<" "<<
}
cout<<"X  "<<X[140][220]<<" "<<X[140][221]<<" "<<X[200][240]<<" "<<X[200][241]<<endl;
//cout<<"XX update "<<endl;
///////////////overlap reconstruct of signal/////////////
double *final_result;
Overloadadd_class2 *ovl_obj=new Overloadadd_class2(X,YPhase,W,SP*W,hal_W,N);
////////select most proper signal///////////
  const int sigLen_ova=ovl_obj->GetSigLen ();
  final_result=new double[sigLen_ova];
  final_result=ovl_obj->Getsig();
  cout<<"set signal "<<final_result[0]<<" "<<final_result[sigLen_ova-1]<<endl;
   /////////////calculate the most possible frame of signal///////////////
  double *tmp_mean_result=new double[4];
  int tmp_length=floor((double)sigLen_ova/4);
  int itd=0;
  int tmp_itd=0;
  for (int otd=0;otd<3;otd++)
  {
    for ( itd=itd+tmp_itd;itd<tmp_length+tmp_itd;itd++)
    {
      tmp_mean_result[otd]+=pow(abs(final_result[itd]),2);
    }
    tmp_mean_result[otd]=tmp_mean_result[otd]/tmp_length;
    tmp_itd=tmp_itd+tmp_length;
  }
   for(itd=3*tmp_length;itd<sigLen_ova;itd++)
      tmp_mean_result[3]+=pow((final_result[itd]),2);
   tmp_mean_result[3]= tmp_mean_result[4]/(sigLen_ova-3*tmp_length);
  ////////////compare and get the index////////////////////

    double max_tmp=max(max(tmp_mean_result[0],tmp_mean_result[1]), max(tmp_mean_result[2],tmp_mean_result[3]));
   //double max_tmp=tmp_mean_result[3]; //max(tmp_mean_result[2],tmp_mean_result[3]);
   if(max_tmp==tmp_mean_result[0])
     setMaxIndex(0,tmp_length-1,tmp_length);
   else if(max_tmp==tmp_mean_result[1])
     setMaxIndex(tmp_length,2*tmp_length-1,tmp_length);
   else if(max_tmp==tmp_mean_result[2])
     setMaxIndex(2*tmp_length,3*tmp_length-1,tmp_length);
   else
     setMaxIndex(3*tmp_length,sigLen_ova-1,sigLen_ova-3*tmp_length);

     /////assign loudest output data
    
    setSourceOut(source_in, maxIndex);


   //////////////////////////////////////////////////////////
  //char * outfile = "final_sigal_result.txt";
	//std::ofstream file(outfile);

	//for(int i=0;i<sigLen_ova;i++)
	//{
	
	//	file<<final_result[i]<<endl;
	//}
	
finish=clock();
cout<<"get final signal and total time:"<<finish-afterread<<endl;
cout<<"time for reading: "<<afterread-start<<endl;



};
//#endif