/////////////////////////
// segmentation with hamming window jin
//////////////////////////
#ifndef SEG_H
#define SEG_H
#include <iostream>
#include <math.h>
#define PI 3.14159265
using namespace std;

class Segment_class{
 
public:
	int W;
	double SP;
	int size_L;
	int N;
	//int **index_matrix;
	double **hw;
	//double **seg_sigal;
	//double hamm_result[(const int)W];
	void hamming_window(const double L,double *result);
	Segment_class(int size,double *signal,int W,double SP);
	double ** Get_Segment(){return hw;};
	void Set_Segment(double **hw_){hw=hw_;};
	int Get_N(){return N;};
	~Segment_class(){};


};

Segment_class::Segment_class(int size,double *signal,int W,double SP)
{
	this->size_L=size;
	this->W =W;
	this->SP=SP;
	SP=floor(W*SP);
	this->N=floor((size_L-W)/SP +1); //number of segments
	cout<<"N "<<N<<" SP "<<SP<<endl;
	double *hamm_result;
	hamm_result=new double[W];
	hamming_window(W,hamm_result);
	
	this->hw = (double **) malloc(W *sizeof(double));
	for(int iRow=0;iRow < W;iRow++)
		this->hw[iRow] = (double *) malloc(N*sizeof(double));
	/*this->index_matrix = (int **) malloc(W *sizeof(int));
	for(int iRow=0;iRow < W;iRow++)
		this->index_matrix[iRow] = (int *) malloc(N*sizeof(int));
	*/
	////////assign the hw matrix
	for(int j=0;j<N;j++){
		for(int i=0;i<W;i++)
		{
			hw[i][j]=hamm_result[i];
			
		}
	
	}
	//int tmp=0;
	/// assign index matrix value
	for(int i=0;i<W;i++){
		for(int j=0;j<N;j++)
		{
			//index_matrix[i][j]=i+SP*j;
			//tmp=i+SP*j;
			hw[i][j]=signal[(int)(i+SP*j)]*hw[i][j];
			//cout<<i<<" "<<j<<" "<<hw[i][j]<<endl; 
			
		}
		
	
	}
	//cout<<hw[0][0]<<" "<<hw[0][1]<<" "<<hw[1][0]<<endl;
	Set_Segment(hw);
	
	
}

void Segment_class::hamming_window(const double L,double *result)
// here the L should be greater than 1
{
	
	double N=L-1;
	
	for (int n=0;n<L;n++)
	{
		
		result[n]=0.54-0.46*cos(2*n*PI/N);
		
	}
	
	

}
#endif