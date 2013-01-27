//////////////////////
// Localization part needs nr_algorithm1.h to provide the processing data, and grid_base_fe.h to 
//  generate random points. It does localization and print the result on dispaly
//////////////////////////
#ifndef Loca_H
#define Loca_H
#include "nr_algorithm1.h"
#include "grid_base_fe.h"
//#include "fe_class.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_sort_double.h>
#include <fftw3.h>
#include <vector>
#include<fstream>
using namespace std;
#define epsmatlab 2.2204e-016
#define PI 3.1415926
//////////////////////////////////////////////////////////
	 
	class Localize_thread
	{
	public:
		//jin_record *record_obj;
		//int     interationice;
	  NRA1 *nra_obj;
    int Rows;
		static const int Cols=4;
		double magiconst;
		double *bstart;
		double *bend;
		double **mic_loc;
		double * final_position;
		static const int	M=4;// number of microphones in the system
		static const int	np=6;//
		static const int 	efs=801;
		static const int  hefs=401;
		double **sf;
		double **yv;
		double **source_in;
    
    
		
		
   
	Localize_thread( )//double **mic_loc,double *bstart,double *bend,const int Rows,const int Cols,const int M,const int np,const )//Communication_jin<MYTYPE> cjin )
	{
		//record_obj=new jin_record();
    this->nra_obj=new NRA1(); // object from noise reduction part
   
    int *arr_Rows=new int[3];
    arr_Rows=nra_obj ->GetMaxIndex();
    this->Rows =arr_Rows[2];
    cout<<"debug of rows "<<this->Rows<<endl;
		double tempc=20.0;
		double fs=44100.0;
		double csound=0;
		double csound_tmp=(tempc/273)+1;
		csound=331.4*sqrt(csound_tmp);
		this->magiconst=10*fs/csound; 
		this->bstart=new double[3];// start search position
		 bstart[0]=-3.5;
		 bstart[1]=-3.5;
		 bstart[2]=1.0;
		 this->bend=new double[3];// end search position
		 bend[0]=3.5;
		 bend[1]=3.5;
		 bend[2]=1.6;
		
		 this->mic_loc = (double **) malloc(4 *sizeof(double));// microphone position must be changed unit in meter
		 for( int iRow=0;iRow < 4;iRow++)
			this->mic_loc[iRow] = (double *) malloc(3*sizeof(double));
			//-0.5 1.75 1.75;-0.5 1.5 1.51;-0.25 1.5 1.5;-0.25 1.75 1.51
			mic_loc[0][0]=0.32;
	    mic_loc[0][1]=-0.35;
	    mic_loc[0][2]=0.9;
	    mic_loc[1][0]=-0.28;
	    mic_loc[1][1]=-0.21;
	    mic_loc[1][2]=0.9;
      mic_loc[2][0]=-0.31;
	    mic_loc[2][1]=0.33;
	    mic_loc[2][2]=0.9;
	    mic_loc[3][0]=0.24;
	    mic_loc[3][1]=0.25;
	    mic_loc[3][2]=0.9;
			/////////////////////////////// initialize parameters 
      this->final_position=new double[3];
			this->sf = (double **) malloc(Rows *sizeof(double));
			for(int row=0;row < Rows;row++)
				this->sf[row] = (double *) malloc(2*(this->Cols)*sizeof(double));
			this->yv = (double **) malloc(efs *sizeof(double));
			for(int row=0;row < this->efs;row++)
				this->yv[row] = (double *) malloc(this->np*sizeof(double));
			
			this->source_in = (double **) malloc(Rows *sizeof(double));
			for(int iRow=0;iRow < Rows;iRow++)
				this->source_in[iRow] = (double *) malloc(this->Cols*sizeof(double));
			
	};
   
	~Localize_thread(){};
	
	// Implementations of the functions inherited from Ice:Thread.
	void Localize_algorithm(){
	//////////////////////////////////
	
		
		  this->source_in=nra_obj->GetSourceOut();// get data from the selected noise reduction part
			/////////////////////////////////////////////////////////////////////
			int		row,col,col2;
			int		p=0;
		for (col=0;col<Cols;col++)// behave fft for source data 
		{
				fftw_complex    *data, *fft_result;
				fftw_plan       plan_forward;
			   
				data        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * Rows);
				fft_result  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * Rows);
			  
				plan_forward  = fftw_plan_dft_1d(Rows, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );
				//fftw_plan_dft_2d(src->height,src->width, in, out, FFTW_FORWARD,FFTW_ESTIMATE);

				//plan_backward = fftw_plan_dft_1d( SIZE, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE );

				 for (row=0; row<Rows; row++) {
					//for (int j=0; j<N; j++) {
						data[row][0] = source_in[row][col];
						data[row][1] = 0;
				   //  }
				 }
			   
				fftw_execute( plan_forward );
				
				//showresult(data, fft_result,W,N);
				fftw_destroy_plan( plan_forward );
				fftw_free( data );
				for(row=0;row<Rows;row++)
				{

					sf[row][2*col]=fft_result[row][0];
					sf[row][2*col+1]=fft_result[row][1];
									
				}
				fftw_free(fft_result);


		}
		//cout<<"checking fft result :"<<" sf:"<<sf[5000][2]<<" "<<sf[5000][3]<<" "<<sf[7000][6]<<" "<<sf[7000][7]<<endl;
//////////////////////////////////////////////////
		double den=0.0;
		double **su1mic,**ss,**newifft;//,**prodal
		//double tmp_re,tmp_imag;
		/////////////shitting loop begins/////////////////
    // doing generalize cross correlation in a table shifting the peak to the center
		su1mic  =  (double **) malloc(Rows *sizeof(double));
		for( row=0;row < Rows;row++)
			{
				su1mic[row] = (double *) malloc(2*M*Cols*sizeof(double));
			}
		for (int i=0;i<M-1;i++)
		{
			/////dynamic variable declarion////////////
			
			
			ss      =   (double **) malloc(Rows *sizeof(double));
			for( row=0;row < Rows;row++)
			{
			
				//su1mic[row] = (double *) malloc(2*M*Cols*sizeof(double));
				ss[row] = (double *) malloc(2*(M-i-1)*sizeof(double));
			}
			
			
			newifft = (double **) malloc(efs *sizeof(double));
			for(row=0;row < efs;row++)
				newifft[row] = (double *) malloc((M-i-1)*sizeof(double));
			///////////////////////////////////////////
			
				for (col=0;col<M;col++)
				{
					for(row=0;row<Rows;row++)
					{
						
						
						su1mic[row][2*col]=sf[row][2*i]*sf[row][2*col]+sf[row][2*i+1]*sf[row][2*col+1];
						su1mic[row][2*col+1]=sf[row][2*i+1]*sf[row][2*col]-sf[row][2*i]*sf[row][2*col+1];
						
					}
				}
				for (col=0, col2=i+1;col<M-i-1,col2<M;col++,col2++)
				{
					for(row=0;row<Rows;row++)
					{
						den=(epsmatlab + sqrt( pow(su1mic[row][2*col2],2)+ pow(su1mic[row][2*col2+1],2) ) );
						ss[row][2*col]=su1mic[row][2*col2]/den;
						ss[row][2*col+1]=su1mic[row][2*col2+1]/den;
						//den=0.0;
					}
				
				//////////////ifft part////////
					fftw_complex    *source, *ifft_result;
					fftw_plan       plan_backward;
					   
					source        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex )* Rows );
					ifft_result  =  ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex )* Rows );
					   // ifft_result = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );	
					plan_backward  = fftw_plan_dft_1d( Rows, source, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE );
					

					for (row=0; row<Rows; row++) 
					{
						
						source[row][0] = ss[row][2*col];
						source[row][1] = ss[row][2*col+1];
							
					 }
					
					fftw_execute( plan_backward );
					fftw_destroy_plan( plan_backward );
					fftw_free( source); 
					
				for(int row_con=Rows-hefs,row_new=0;row_con<Rows-1,row_new<hefs;row_con++,row_new++)
				{
					newifft[row_new][col]=(double)ifft_result[row_con][0]/Rows;//ssifft[row_con][j];			
				}
				for(int row_con=0,row_new=hefs;row_con<efs-hefs,row_new<efs;row_con++,row_new++)
				{
					newifft[row_new][col]=(double)ifft_result[row_con][0]/Rows;//ssifft[row_con][j];	
				}
				fftw_free(ifft_result); 
			}
			for(int cc=p,col=0;cc<p+M-i-1,col<M-i-1;cc++,col++)
			{
				for(row=0;row<efs;row++)
				{
					yv[row][cc]=newifft[row][col];  
				}
			}
			p=p+M-i-1; 
			
			for(row = 0;row < Rows; row++) 
			{
				 //free(su1mic[row]);
				 free(ss[row]);
				
				
			}
			for(row = 0; row < efs; row++) 
			{
				 free(newifft[row]);
			}
			
			//free(su1mic);
			free(ss);
			free(newifft);
			
		}
		for(row = 0;row < Rows; row++) 
			{
				 free(su1mic[row]);
			}
	free(su1mic);
	cout<<"check yv "<<yv[0][0]<<" "<<yv[efs-1][np-1]<<" "<<yv[100][0]<<" "<<yv[20][4]<<endl;
	cout<<"check yv "<<yv[699][3]<<" "<<yv[599][2]<<endl;//" "<<yv[100][0]<<" "<<yv[20][4]<<endl;
	
////////////////////////////////////interpolation part
	const int N=(efs-1)*10+1;
	double *x=new double[10];
	double *xx=new double[N];
	double **yv_spline;//(np,N);
	yv_spline = (double **) malloc( N *sizeof(double));
	for(int row=0;row < N;row++)
		yv_spline[row] = (double *) malloc(np*sizeof(double));
	
	
	///////////////////////////
	double* int_tmp_sp=new double[10];
	double xi=0.0,yi=0.0;
  int j;
	for (int i=0;i<6;i++)
	{	
		for(int k=0;k<80;k++)
		{
		//cout<<"step 5"<<endl;
		//tmp_sp=yv.get_row (i);
			for( j=0;j<10;j++)
			{
				int_tmp_sp[j]=yv[k*10+j][i];
				x[j]=k*10+j;
			}
		//	cout<<"step 6"<<endl;
			gsl_interp_accel *acc	= gsl_interp_accel_alloc ();
			gsl_spline		 *spline = gsl_spline_alloc (gsl_interp_cspline, 10);
			gsl_spline_init (spline, x, int_tmp_sp, 10);
			 xi=x[0];
		//	cout<<"step 7"<<endl;
			
			for (int ind=0;ind<100;ind++)
			   {
			     
				    yv_spline[ind+k*100][i]= gsl_spline_eval (spline, xi, acc);
				
				   xi=xi+0.1;
				  
			   }
			
			
			gsl_spline_free (spline);
			gsl_interp_accel_free (acc);
		
		
		}
		
	}
	  delete int_tmp_sp;
	
	  double efsintp=N/2;
	  cout<<"interpolation finished "<<endl;
	//////////////////grid generator////////////
    double *y_height_vec=new double[100];
	  for(int i=0;i<100;i++)
		  y_height_vec[i]=bstart[2]+(bend[2]-bstart[2])*i/100;

	  double *radia_range =new double[200];
	  double step=(bend[0]-0.5)/200;
	  for(int i=0;i<200;i++)
		  radia_range[i]=0.5+step*i;
	  double *angel_range= new double[360];
	  for(int i=0;i<360;i++)
		  angel_range[i]=(1+i)*PI/180;
///////////////////////////////////////
	  clock_t src_start,src_end;
	  src_start=clock();
  //// search begins here/////////
	  src(bstart, bend, mic_loc,magiconst, yv_spline, efsintp,final_position,y_height_vec,radia_range,angel_range);
	  src_end=clock();
	//cout<<"time src"<<src_end-src_start<<endl;
	  cout<<"final position "<<final_position[0]<<" "<< final_position[1]<<" "<<final_position[2]<<endl;
  // get final result
  
		
	};
  ///////////////////////////////////////////////////////////
  double *get_final_position()
   {
     return this->final_position;
   };
   
  /* string get_final_position()
   {
	   return "get result";
   }*/
//////////////////////////grid based search////////////////////////////////////////
 void src(double* bstart,double* bend,double** mic_loc,double magiconst,double** yv_spline,double efsintp,double *final_position,double *y_height_vec, double *radia_range,double *angel_range)
{
	cout<<"in src "<<endl;
	const int N=8001;
	const int randpts=3000,npoints=100;
	const int maxFEs=100000;
	fe_cal_jin *array_result[randpts]; // for store num of randpts result
	double **position,*yval;
	int new_size=0;
	vector<vector <double>> bestvec;
	
	yval=new double[randpts];
	double* tmp_srp_vec=new double[npoints];
	vector<double>  srp_vec;
	//ssrp_vec=new double[npoints];
	position = (double **) malloc(randpts *sizeof(double));
	for(int i=0;i < randpts;i++) // generate and store random points here
	{
		//position[i] = (double *) malloc(3*sizeof(double));
		
		//array_result[i]=new fe_cal( yv_spline,mic_loc,magiconst,efsintp,N,bstart,bend,i);
		array_result[i]=new fe_cal_jin( yv_spline,mic_loc,magiconst,efsintp,N,bstart,bend,y_height_vec,radia_range,angel_range,i);
    yval[i]=array_result[i]->get_yval ();
		//for(int j=0;j<3;j++)
		position[i]=array_result[i]->get_position();
		//cout<<"position "<<i<<" "<<position[i][0]<<" "<<position[i][1]<<" "<<position[i][2]<<endl;
	}
	cout<<"3000 random finished "<<endl;
	////////////sort//////////////
	  
     
    size_t ii, kkgsl = npoints, NN = randpts;
     
		//double * x = new double[randpts];
       
		size_t * large_index=new size_t[npoints];
        int * ind_vec=new int[npoints];
     
      // T = gsl_rng_default;
      // r = gsl_rng_alloc (T);
     
       
		
        gsl_sort_largest (tmp_srp_vec,kkgsl , yval, 1, NN);
		    gsl_sort_largest_index (large_index,kkgsl ,yval, 1, NN);
      //for(int i=0;i<npoints;i++)
        //gsl_sort_largest_index (large_index,kkgsl ,yval, 1, NN);
		int index;
       //ind_vec=large_index;
        for (ii = 0,index=0; ii < npoints,index<npoints; ii++,index++)
         {
           
		   ind_vec[index]=(int)large_index[ii];
		   //cout<<(int)large_index[ii]<<endl;
		   //cout<<"tmp srp "<<tmp_srp_vec[index]<<endl;
		}
		for(int index=0;index<npoints;index++)
		{	
			srp_vec.push_back (tmp_srp_vec[index]);
        
			//cout<<"get srp in out "<<srp_vec[index]<<endl;
			vector<double> tmp_bestvec(3);
			for(int j=0;j<3;j++)
			{
				
				tmp_bestvec[j]= position[ind_vec[index]][j];
			}
			//cout<<"get tmp vec "<<endl;
				bestvec.push_back (tmp_bestvec);
				tmp_bestvec.clear();
			
		}
		
		double *tmp_array=new double[npoints];
		
		 cout<<"debug of best vec "<<bestvec[99][0]<<" "<<bestvec[99][1]<<" "<<bestvec[99][2]<<" "<<endl;
		double *newbstart=new double[3];
		double *newbend=new double[3];

		   for(int i=0;i<3;i++)
		   {
			  for(int row=0; row < npoints; row++)
			  {
				  tmp_array[row]=bestvec[row][i];
			  }
			    // memcpy(tmp_array,bestvec[])
				 gsl_sort_largest (tmp_array,kkgsl , tmp_array, 1, kkgsl);
				
				 newbstart[i]=tmp_array[npoints-1];
				 newbend[i]= tmp_array[0];
				 //cout<<"start end"<<newbstart[i]<<" "<<newbend[i]<<endl;
			   
		   }
		 delete tmp_array;
		 double searchVol=(newbend[0]-newbstart[0])*(newbend[1]-newbstart[1])*(newbend[2]-newbstart[2]);
		 
		 double minVoxel=0.000001;//100*pow10(-6.0);
		
		 int countfe=0,kk=0,TotalFEs=randpts;
		
		 int size_np;
		 int num=0;
		 double tmp_sum;
		//int iter=0;
		//int *gidx;
		
		double *goodpos=new double[3];
		double *nonewpts=new double[maxFEs];
    while (searchVol>minVoxel && TotalFEs <=maxFEs)// make up some points and search 
	  {      
			
			if(kk==0)
				size_np=npoints;
			else
				size_np=srp_vec.size ();
			
			num=0;
			tmp_sum=0.0;
		//////////////////////////////
			for (int i=0;i<size_np;i++)
			{
				tmp_sum=tmp_sum+srp_vec[i];
			}
			tmp_sum=tmp_sum/size_np;
			//cout<<"tmp sum "<<tmp_sum<<endl;
			vector <int> gidx(size_np);//=new int[(const int) size_np];
			//cout<<"loop ini gidx "<<endl;
			for(int i=0;i<size_np;i++)
				{
					if(srp_vec[i]>=tmp_sum)
					{
						
							gidx[num]=i;
							num++;
							
					}
				}
		
			
		//////////////
		gidx.resize (num);
		vector<double> a5;//(num);
		vector<vector <double>> newbestvec;//( num, vector<double> ( 3 ) );
		cout<<"new bestvec init "<<endl;
		//a5=new double[(const int) num];
		//double **newbestvec;
		//newbestvec = (double **) malloc(num *sizeof(double));
		for(int i=0;i < num;i++)
		{
			//newbestvec[i] = (double *) malloc(3*sizeof(double));
			//newbestvec.push_back(vector<double> (3));

			  a5.push_back(srp_vec[gidx[i]]);
			//for(int j=0;j<3;j++)
				newbestvec.push_back( bestvec[gidx[i]]);
		}
			//cout<<"num new "<<num<<"a5 verifiy "<<a5[num-1]<<" "<<newbestvec[num-1][0]<<endl;
			nonewpts[kk]=npoints-num;
			//////////////////////////////////////////
			//double *tmp_new=new double[(const int)num];
			tmp_array=new double[(const int)num];
			kkgsl=num;
			for(int i=0;i<3;i++)
		    {
			  for(int row=0; row < num; row++)
			  {
				 tmp_array[row]=newbestvec[row][i];
				// cout<<"check sort "<<newbestvec[row][i]<<endl;
			  }
			    // memcpy(tmp_array,bestvec[])
				 gsl_sort_largest (tmp_array, kkgsl , tmp_array, 1, kkgsl);
				 newbstart[i]=tmp_array[num-1];
				 newbend[i]= tmp_array[0];
				 //cout<<"start end in loop "<<kk<<" "<<newbstart[i]<<" "<<newbend[i]<<endl;
			   
			}
			delete tmp_array;
			//cout<<"after delete "<<endl;
		////////////////////////////
			if (nonewpts[kk]>=1)
			{		//cout<<"in small loop "<< nonewpts[kk]<<endl;
					for (int i=0;i< nonewpts[kk];i++)
					{
							double a4=0.0;
							
							while (a4 < tmp_sum && countfe <=maxFEs)// check point for the mean value is not make up some points
							{
								//cout<<"get new points in while "<<endl;
								//fe_cal *a4_good;
								//a4_good	=new fe_cal( yv_spline,mic_loc,magiconst,efsintp,N,newbstart,newbend,countfe+3000);
								fe_cal_jin *a4_good;
								a4_good	=new fe_cal_jin( yv_spline,mic_loc,magiconst,efsintp,N,newbstart,newbend,y_height_vec,radia_range,angel_range,countfe+3000);
                a4=a4_good->get_yval ();
								goodpos=a4_good->get_position();
								//cout<<"goodpos "<<goodpos[0]<<endl;
								countfe=countfe+1;
								delete a4_good;
								//cout<<"in while loop "<<endl;

							}
							if(a4!=0)

							{       //cout<<"in a4!=0 "<<a4<<endl;
									num++;
									vector<double> tmp_vec_godpos(3);
								    for(int ts=0;ts<3;ts++)
									  tmp_vec_godpos[ts]=goodpos[ts];
									//a5.resize(a5.size()+1);
								  	a5.push_back (a4);
									//num=a5.size ();
									//newbestvec.resize(a5.size(),vector<double>(3));
									newbestvec.push_back (tmp_vec_godpos);
								//	cout<<"new best in a4 llop  "<<tmp_vec_godpos[0]<<endl;
									tmp_vec_godpos.clear();
							}
							
						 }

		  }
		

		////////////////////////////////
		//cout<<"out of small loop "<<endl;
	  new_size=a5.size();
		srp_vec.clear ();
		srp_vec.resize(new_size);
		for(int i=0;i<new_size;i++)
			srp_vec[i]=a5[i];
		//cout<<"check for new size!!!!!!!!! "<<new_size<<endl;
		bestvec.clear();
		//cout<<"free old bestvec "<<endl;
		//bestvec.resize(new_size,vector<double>(3));
		//bestvec((const int)new_size,vector<double>3);
		//bestvec = (double **) malloc(new_size *sizeof(double));
		for(int i=0;i <new_size;i++)
		{
			//bestvec.push_back(vector<double> (3));

			//for (int j=0;j<3;j++)
				bestvec.push_back (newbestvec[i]);
		}
		
		//cout<<"get new best vec"<<new_size <<endl;//" best vec "<<bestvec[new_size-1][2]<<" "<<newbestvec[new_size-1][2]<<endl;
		
		////////////////////////////////
		tmp_array=new double[(const int) new_size];
		kkgsl=new_size;
		for(int i=0;i<3;i++)
		   {
			  for(int row=0; row < new_size; row++)
			   {
				    tmp_array[row]=bestvec[row][i];
			   }
			    // memcpy(tmp_array,bestvec[])
				 gsl_sort_largest (tmp_array, kkgsl , tmp_array, 1, kkgsl);
				 newbstart[i]=tmp_array[new_size-1];
				 newbend[i]= tmp_array[0];
				 //cout<<"start end"<<newbstart[i]<<" "<<newbend[i]<<endl;
			   
		    }
		delete tmp_array;
		////////////////renew search volumn
		searchVol=(newbend[0]-newbstart[0])*(newbend[1]-newbstart[1])*(newbend[2]-newbstart[2]);
		
		TotalFEs=randpts+countfe;
    kk=kk+1;
		//cout<<"searchVol "<<searchVol<<" total fes "<<TotalFEs<<" iter "<<kk<<endl;
		/////////////////////
		
		newbestvec.clear();
		a5.clear();
		gidx.clear();
		
		//////////////////////
		//cout<<"delete uneeded info "<<endl;

	}
	//////////////////////////////////
	
	size_t *lar_id=new size_t[1];
	//double *tsort_srp_vec=new double[new_size];
  tmp_array=new double[new_size];
	kkgsl=1;
	NN=new_size;
	for(int i=0;i<new_size;i++){
		tmp_array[i]=srp_vec[i];
		//cout<<srp_vec[i]<<endl;
	}
  // sort for get the final best position
	gsl_sort_largest_index (lar_id,kkgsl ,tmp_array, 1, NN);
	delete tmp_array;
	//cout<<"finish sorting "<<endl;
	//searchVol=0;
	//////////////////////////////////
	for (int i = 0; i < randpts; i++)
     {
         delete array_result[i];
         array_result[i] = NULL;
     }
	cout<<"delet array result "<<lar_id[0]<<endl;
	//double *finaljin=new double[3];
	int ind_final=(int)lar_id[0];
	final_position[0]=bestvec[ind_final][0];
	final_position[1]=bestvec[ind_final][1];
	final_position[2]=bestvec[ind_final][2];
	
};//end

///////////////////////////////////////////////////////////////////////////

	
};
#endif