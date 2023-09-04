//////////////////////////////////////////////////////////////////////////////
// k-regular Network with four blocks 
// Notice the range of degrees in each block!
//
// In final network:
//		for E-neurons: k_in=kE+kIout/4	,	k_out=kE+kEout
//		for I-neurons: k_in=kI+4*kEout	,	k_out=kI+kIout
//
// So, for a network with the same in- and out-degree, set:
//		kE = kIout = k such that k=8*n
//		kI = kEout = kE/4
//
// Example; for a network with N=5000, kin(+)=400, kin(-)=100, kout=500
// set: kE=kIout=400 , kI=kEout=100
// 
// The output is a text file as the adjacency list where
// 		first row: total_number_of _nodes,	total_number_of_links
// 		other rows: presynaptic_neuron,	postsynaptic_neuron
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

using namespace std;

#define N	5000    // Total number of nodes
#define per	0.8    // Number of Exc nodes
#define kE  400	   // input excitatory degree (in block E-to-E) MUST BE EVEN 2*n
#define kI  100	   // input inhibitory degree (in block I-to-I) MUST BE EVEN 2*n
#define kEout 100  //output degree of I nodes (in block I-to-E) MUST BE EVEN 2*n
#define kIout 400   //output degree of E nodes (in block E-to-I) MUST BE 8*n
#define Wexcitatory   1 // weight of excitatoty synapses
#define Winhibitory   1 // weight of inhibitory synapses

int    A[N][N] = {0};  // Adjacency Matrix layer Exc

/********************************************/
// Main
/********************************************/
int main()
{
 int NE=per*N;
 int NI=N-NE;
 int i, j, l, a, j_max;
 int start, end, counter, limit;
	
 // output file; adjacency list
 ofstream outputAdjMat("./AdjMatListKregular.txt");
  
 //-----------------------------------------------------------------
 // creating block E-to-E; a k-regular graph with NE nodes and k=kE
 j_max=kE/2;
 for ( j=1 ; j<=j_max; j++ ){
 	for ( i=0 ; i<(NE-j) ; i++ )
 		A[i][i+j]=A[i+j][i]=Wexcitatory;
 	for (i=0 ; i<j ; i++)
 		A[NE-j+i][i]=A[i][NE-j+i]=Wexcitatory;
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
 // Output: Adjacency list (as a list)
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
 
