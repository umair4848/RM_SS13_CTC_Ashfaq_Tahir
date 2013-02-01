///////reconst signal for noise reduction/////////
#ifndef OVAC_H
#define OVAC_H
#include <iostream>
#include <math.h>
#include <fftw3.h>
using namespace std;
class Overloadadd_class2
{
public: 
	double  **XNEW;
	double  **yphase;
	int     windowLen;
	int		ShiftLen;
	 int	row_num;
	 int	col_num;
	 int	start;
	double  **Spec;
	double  **Spec_ud;
	double  *sig;
	double *tmp_ifft;
  int sig_length;
	Overloadadd_class2(double **XNEW,double **yphase,int windowLen,int ShiftLen, int row_num, int col_num );
	void Setsig(double *sig_){sig=sig_;};
	double *Getsig(){return sig;};
  int GetSigLen(){return this->sig_length;};
	~Overloadadd_class2(){cout<<"over lap adding finished ";};






};

Overloadadd_class2::Overloadadd_class2 (double **XNEW,double **yphase,int windowLen,int ShiftLen, int row_num, int col_num )
{
	this->col_num=col_num;
	this->row_num=row_num;
	this->ShiftLen =ShiftLen;
	this->windowLen =windowLen;
	this->XNEW = (double **) malloc(row_num *sizeof(double));
	for(int iRow=0;iRow < row_num;iRow++)
		this->XNEW[iRow] = (double *) malloc(2*col_num*sizeof(double));
	this->yphase = (double **) malloc(row_num *sizeof(double));
	for(int iRow=0;iRow < row_num;iRow++)
		this->yphase[iRow] = (double *) malloc(col_num*sizeof(double));
	this->XNEW =XNEW;
	this->yphase=yphase;
	const int twice_col_num=2*col_num;
	const int const_row_num=this->col_num;
	const int tconst_row_num=2*row_num-1;
	this->Spec = (double **) malloc(row_num *sizeof(double));
	for(int iRow=0;iRow < row_num;iRow++)
		this->Spec[iRow] = (double *) malloc(twice_col_num*sizeof(double));
  
	const int in_length=(col_num-1)*ShiftLen+windowLen;
  this->sig_length=in_length;
  cout<<"in-length "<<in_length<<endl;
	this->sig=new double [in_length];
	for(int i=0;i<in_length;i++)
		sig[i]=0;
	this->tmp_ifft=new double[col_num];
	for(int i=0;i<col_num;i++)
		tmp_ifft[i]=0;
	this->Spec_ud = (double **) malloc((2*row_num-2) *sizeof(double));
	for(int iRow=0;iRow <(2*row_num-2);iRow++){
		this->Spec_ud[iRow] = (double *) malloc(twice_col_num*sizeof(double));}
	//const int ifft_length=2*row_num-2;
	//this->start=0;
	//this->Spec=new double[const_row_num][twice_col_num];
	//this->Spec_ud=new double[tconst_row_num][twice_col_num];
	/////////////////////////////////////////////
	//Spec=XNEW.*exp(j*yphase);
	for(int row=0;row<row_num;row++)
	{
		for(int col=0;col<col_num;col++)
		{
			//for(int s_col=0;s_col<twice_col_num;s_col=s_col+2)
			//{
				Spec[row][2*col]=  XNEW[row][2*col]*cos(yphase[row][col])-XNEW[row][2*col+1]*sin(yphase[row][col]);
				Spec[row][2*col+1]=XNEW[row][2*col]*sin(yphase[row][col])+XNEW[row][2*col+1]*cos(yphase[row][col]);
			//}
		}
	
	}
	
	//cout<<"XNEW "<<XNEW[1][0]<<" "<<XNEW[1][1]<<" "<<XNEW[2][0]<<" "<<XNEW[2][1]<<endl;
	
	/*if ((windowLen%2)==1) //if FreqResol is odd
	{
		this->Spec_ud=new double[2*row_num-1][2];
		for(int row=0;row<row_num;row++)
		{
			//for(int rcindex=0;rcindex<2;rcindex++)
			//{
				Spec_ud[row][0]=Spec[row][0];
				Spec_ud[row][1]=-Spec[row][1];
			
			//}
		}
		for(int row=row_num,row_flip=2*row_num-2;row<2*row_num-1,row_flip>=row_num;row_flip--,row++)
		{
			//for(int rcindex=0;rcindex<2;rcindex++)
			//{
				Spec_ud[row][0]=Spec [row_flip][0];
				Spec_ud[row][1]=-Spec[row_flip][1];
			
			//}
		}
		//Spec=[Spec;flipud(conj(Spec(2:end,:)))];
	}*/
	//else
	//{
	
		//this->Spec_ud=new double[2*row_num-2][twice_col_num];
		for(int row=0;row<row_num;row++)
		{
			for(int col=0;col<col_num;col++)
			{
				Spec_ud[row][2*col]=Spec[row][2*col];
				Spec_ud[row][2*col+1]=Spec[row][2*col+1];
			
			}
		}
		for(int row=row_num, row_flip=row_num-2; row<2*row_num-2, row_flip>0;row_flip--,row++)
		{
			for(int col=0;col<col_num;col++)
			{
				Spec_ud[row][2*col]  =  Spec [row_flip][2*col];
				Spec_ud[row][2*col+1]= -Spec[row_flip][2*col+1];
			
			}
		}
		cout<<"row num "<<row_num<<endl;
		cout<<"Spec"<<Spec_ud[0][0]<<" "<<Spec_ud[0][1]<<endl;
		cout<<"Spec"<<Spec_ud[128][0]<<" "<<Spec_ud[128][1]<<endl;
		cout<<"Spec"<<Spec_ud[129][0]<<" "<<Spec_ud[129][1]<<endl;
		//for (int row=2*row_num-2;row<windowLen;row++)
		//	Spec_ud[row][col] 
	////////////////////ifft
	//float	(*ifft_output)[2]=new float[W*N][2];
	int k=0;
	for (int i=0;i<col_num;i++)
	{
		fftw_complex    *source, *ifft_result;
		fftw_plan       plan_backward;
	   
		source        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex )* windowLen );
		ifft_result  =  ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex )* windowLen );
	   // ifft_result = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );	
		plan_backward  = fftw_plan_dft_1d( windowLen, source, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE );
	//fftw_plan_dft_2d(src->height,src->width, in, out, FFTW_FORWARD,FFTW_ESTIMATE);

    //plan_backward = fftw_plan_dft_1d( SIZE, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE );

      for (int row_in=0; row_in<windowLen; row_in++) {//(2*row_num-2)
        //for (int j=0; j<col_num; j++) {
			//for(int k=0;k<twice_col_num; k=k+2){
		 // if(k<twice_col_num){
				source[row_in][0] = Spec_ud[row_in][2*i];
				source[row_in][1] = Spec_ud[row_in][2*i+1];
			//	k=k+2;
		 // }
			//}
       //  }
     }
    // source[2*row_num-2][0]=0;
	 //source[2*row_num-2][1]=0;
	// source[2*row_num-1][0]=0;
	 //source[2*row_num-1][1]=0;
    fftw_execute( plan_backward );
	
	//showresult(data, fft_result,W,N);
    fftw_destroy_plan( plan_backward );
	fftw_free( source );
	
	/////////////////////////////////////////
	//cout<<"ifft result "<<i<<" "<<ifft[i][0]<<endl;
		//start=i*ShiftLen;
		//for(int row=0;row<row_num;row++)
		//{
	//for(int rowifft=0;rowifft<windowLen;rowifft++)
	//{
		for(int index=i*ShiftLen, rowifft=0;index<(i*ShiftLen+windowLen),rowifft<windowLen;rowifft++,index++){
				//cout<<"ifft_result "<<i<<" "<<(double)ifft_result[127][0]<<endl;
				sig[index]=sig[index]+(double)ifft_result[rowifft][0]/windowLen;
		}
	//}
				//sig(start:start+windowLen-1)=sig(start:start+windowLen-1)+real(ifft(spec,windowLen));    
		//}
		fftw_free( ifft_result );

	}
	
	Setsig(sig);
	
	//cout<<"winLen: "<<windowLen<<" 2*row-2: "<<2*row_num-2<<endl;
	//*/
};
#endif