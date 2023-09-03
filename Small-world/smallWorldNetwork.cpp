//////////////////////////////////////////////////////////////////////////////
// k-regular Network with Small-World block E-to-E
// Notice to the range of degrees in each block!
//
// For a SW network with a same mean in- and out-degree for both neuron typs 
// set: kE <= kEold - NE*prob, where kEold is kE in an k-regular graph 
//
// Example for a SW network with almost N=5000, <kin+>=400, <kin->=100, kout=500, prob=0.02
// set: kE=330, kIout=400 , kI=kEout=100
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

using namespace std;

#define N	5000    // Total number of nodes
#define per	0.8    // Number of Exc nodes
#define kE  330	   // input excitatory degree (in block E-to-E) MUST BE EVEN 2*n
#define kI  100	   // input inhibitory degree (in block I-to-I) MUST BE EVEN 2*n
#define kEout 100  //output degree of I nodes (in block I-to-E) MUST BE EVEN 2*n
#define kIout 400   //output degree of E nodes (in block E-to-I) MUST BE 2*n and >6 >>> for equal indegrees, set 8*n
#define Wexcitatory   1 // weight of excitatoty synapses
#define Winhibitory   1 // weight of inhibitory synapses
#define prob   0.02  // probability for E-to-E making small-world

int    A[N][N] = {0};  // Adjacency Matrix layer Exc

/********************************************/
// Main
/********************************************/
int main()
{
 int NE=per*N;
 int NI=N-NE;
 int i,j,l,a,j_max;
 int start,end,counter,limit;
	
 // output
 ofstream outputAdjMat("./AdjMatListSW.txt");

 unsigned seed = time(NULL);
 srand(seed);
 double r;       // double random number
  
 //-----------------------------------------------------------------
 // creating block E-to-E; a k-regular graph with NE nodes and k=kE
 j_max=kE/2;
 for ( j=1 ; j<=j_max; j++ ){
 	for ( i=0 ; i<(NE-j) ; i++ )
 		A[i][i+j]=A[i+j][i]=Wexcitatory;
 	for (i=0 ; i<j ; i++)
 		A[NE-j+i][i]=A[i][NE-j+i]=Wexcitatory;
 }
 // making small-world
 for ( i=0 ; i<NE ; i++){
	 for ( j=0 ; j<NE ; j++){
		 if ( i!=j ){
			double r = rand()/double(RAND_MAX); // between (0,1)
			if ( r<prob ){
				A[i][j] = Wexcitatory;
			}
		 }
	 }
 }
 //-----------------------------------------------------------------
 // creating block I-to-I; a k-regular graph with NI nodes and k=kI
 j_max=kI/2;
 for ( j=1 ; j<=j_max; j++ ){
 	for ( i=0 ; i<(NI-j) ; i++ )
 		A[i+NE][NE+i+j]=A[NE+i+j][NE+i]=Winhibitory;
 	for (i=0 ; i<j ; i++)
 		A[NI-j+i+NE][i+NE]=A[i+NE][NI-j+i+NE]=Winhibitory;
 }
 //-----------------------------------------------------------------
 // creating block I-to-E;
 a=(kIout-4)/2;
 for ( i=0 ; i<NI ; i++ ){
 	j=0;
 	bool flag=false;
 	while (!flag){
 		if ( floor(j/4)==i ){
 			start=j-a;
 			end=j+4+a;
 			if ( start>=0 && end<NE ){
 				for ( counter=start ; counter<end ; counter++ )
 					A[i+NE][counter]=Winhibitory;
			}
			else if ( start>=0 && end>=NE ){
				for ( counter=start ; counter<NE ; counter++ )
					A[i+NE][counter]=Winhibitory;
				limit=end-NE;
				for ( counter=0 ; counter<limit ; counter++)
					A[i+NE][counter]=Winhibitory;
			}
			else if ( start<0 && end<NE ){
				for ( counter=0 ; counter<end ; counter++)
					A[i+NE][counter]=Winhibitory;
				limit=abs(start);
				for ( counter=0 ; counter<limit ; counter++)
					A[i+NE][NE-counter-1]=Winhibitory;		
			}
			flag=true;
		}
		j++;
	}	
 } 
 //-----------------------------------------------------------------
 // creating block E-to-I; 
 a=kEout/2;
 for ( i=0 ; i<NE ; i++ ){
 	int g=floor(i/4);
 	start=g-a;
 	end=g+a;
 	if ( start>=0 && end<NI ){
 		for ( counter=start ; counter<end ; counter++ )
 			A[i][counter+NE]=Wexcitatory;
	}
	else if ( start>=0 && end>=NI ){
		for ( counter=start ; counter<NI ; counter++ )
			A[i][counter+NE]=Wexcitatory;
		limit=end-NI;
		for ( counter=0 ; counter<limit ; counter++)
			A[i][counter+NE]=Wexcitatory;
	}
	else if ( start<0 && end<NI ){
		for ( counter=0 ; counter<end ; counter++)
			A[i][counter+NE]=Wexcitatory;
		limit=abs(start);
		for ( counter=0 ; counter<limit ; counter++)
			A[i][NE+NI-counter-1]=Wexcitatory;		
	}
 }
 
 //-----------------------------------------------------------------
 // Output: Adjacency list
 int linkNum=0;
 for (i=0;i<N;i++)
 	for (j=0;j<N;j++)
 		if (A[i][j] != 0) linkNum++;
 
 outputAdjMat << N << "\t" << linkNum << endl;
 
 for (i=0;i<N;i++)
 	for (j=0;j<N;j++){
 		if (A[i][j] != 0)
 			outputAdjMat << i << "\t" << j << endl;
	}

return 0;
}
 
