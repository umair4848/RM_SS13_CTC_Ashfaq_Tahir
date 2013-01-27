/////grid base fe generator
// generate grid based fe points according to the scale in localize_alg.h
////////////////
#include"geometry_msgs/PointStamped.h"
#include <iostream>
#include<math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h> 
#include <ctime>

#include <cstdlib>


//////////////////////


using namespace std;
#define PI 3.1415926
class fe_cal_jin //Sample Class for the C++ Tutorial 
   {
       public:
        
		 double yval_;
		 double *y_;
		 double *position2;
     int in_int;
		  fe_cal_jin(double** yintpt,double ** mic_loc,double magiconst,double efsintp,int N,double*  bstart,double*  bend,double *y_height_vec, double *radia_range,double *angel_range,int in)//double*  bstart1,double*  bend1,double* * mic_location1,double magiconst1,double** yintpt1,double**  row11,double efsintp1) //Constructor for the C++ tutorial 
         { 
			  clock_t time;
			  this->yval_=0.0;
			  this->y_=new double[3];
			  for(int i=0;i<3;i++)
				  y_[i]=0;
			  this->position2=new double [3];
			  double *dist=new double[4];
			  double xx1[4][3];

			  if(in==0)
				this->in_int=0;
			  else
				this->in_int=in;
			  
				  time=clock();
          srand(time*in_int);
          in_int++;
				  y_[2]=y_height_vec[rand()%100+1];// get height
         // y_[2]=1.0;
				 //srand ( (unsigned)time(0) );
				  time=clock();
          srand(time*in_int);
          in_int++;
				double y_angle=angel_range[rand()%360+1];// get angle
				// srand ( (unsigned)time(0) );
				 time=clock();
          srand(time*in_int);
          in_int++; 
				double y_radias= radia_range[rand()%200+1]; // get radius
			/*	
				 for(int i=0;i<3;i++)
			  {
				 
				  y_[i]=bstart[i]*(1-y_[i]/1000)+bend[i]*y_[i]/1000;
				
			  }	
			 */ 
				 position2=y_;
				 y_[0]=y_radias*cos(y_angle);// get position x from above
				 y_[1]=y_radias*sin(y_angle);// get corr. position y
				
				 //position2=[y_r,angel,y_h];

			  for(int i=0;i<4;i++)// calculate dis. between microphones and the assumed source pos.
			  {
				  for(int j=0;j<3;j++)
				  {
					  xx1[i][j]=y_[j]-mic_loc[i][j];

				  }
				  dist[i]=sqrt(pow(xx1[i][0],2)+pow(xx1[i][1],2)+pow(xx1[i][2],2));
			  }
			 double *v=new double[6];
			// cal. distance difference
			v[0]=dist[0]-dist[1];
			v[1]=dist[0]-dist[2];
			v[2]=dist[0]-dist[3];
			v[3]=dist[1]-dist[2];
			v[4]=dist[1]-dist[3];
			v[5]=dist[2]-dist[3];
			
			for (int j=0;j<6;j++)
			{
				yval_=yval_+yintpt[(int)floor(magiconst*v[j]+efsintp+1.5)][j];
				// get corr. srp value from the table
			}
			
         };
	    double *get_position(){return y_;};
		double get_yval(){return yval_;};
       ~fe_cal_jin() //destructor for the C++ Tutorial 
       {
		  /* 
			delete []bstart_;
			delete []bend_;
			delete []position_new_;
			delete []y_;
			delete []row1_;
			for(int row = 0;row < N_; row++) 
			{
				 free(yintpt_[row]);
			}
			free(yintpt_);
			
			for(int row = 0;row < 4; row++) 
			{
				 free(mic_loc_[row]);
			}
			free(mic_loc_);
			cout<<"finish deconstr "<<endl;
			*/
	   
	   } ;

}; 
  