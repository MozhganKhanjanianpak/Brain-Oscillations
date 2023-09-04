/////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation code for ER network with exponential decay of synapse activation    	 
// Set appropriate cut-off (LowCurLimExc and LowCurLimInh) of excitatory and inhibitory links 
// For example:	
//			TE:			4		5		6		7
//		----------------------------------------------------------------------------------
//		LowCurLimExc:                  0.17	       0.13            0.09	      0.08
//
//			TI:			4		5		6		7
//		---------------------------------------------------------------------------------
//		LowCurLimInh:                  0.69            0.54            0.38          0.35
//////////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;

#include <iostream>
#include <math.h>
#include <cmath>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

#define linkState(x)  ( (x==0) ? 0 : 1)

#define N    5000   // Number of nodes
#define E    0.8    // percentage of Excitatory neurons
#define TE   5      // time constant of Exc. links
#define TI   5      // time constant of Inh. links
#define D    4      // neuron firing threshold value
#define tmax 20000    // implementation maximum time
#define cmax  1   // maximum value of an active link's current 
#define LowCurLimExc 0.13
#define LowCurLimInh 0.54
#define pNodeE 0.001 // randomly excitatory node activation probability
#define pNodeI pNodeE  // randomly inhibitory node activation probability
#define p   0.1        // connection probability for ER network

const double  e = 2.71828182845904523536;

int  A[N][N] = {0};             // Adjacency Matrix of Excitatory links
int  B[N][N] = {0};             // Adjacency Matrix of Inhibitory links
int node_state[N] = {0};    // state of neurons at odd times, including: 0=inactive , 1=active
double node_input[N] = {0};        // neuron input

int Exc_link_counter[N][N]         = {0}; // the counter of Excitatory links; 0, 1, 2, ...,TE
int Exc_link_counter_updated[N][N] = {0}; // updated counters
int Inh_link_counter[N][N]         = {0}; // the counter of Inhibitory links; 0, 1, 2, ...,TI
int Inh_link_counter_updated[N][N] = {0}; // updated counters

/********************************************/
// Main
/********************************************/
int main()
{
    ofstream outputForActiveNodes("./ActiveNodeDensity.txt"); //output; Excitatory & Inhibitory active neurons versus time
    outputForActiveNodes << "#time,	NE,	NI" << endl;
    
    unsigned seed = time(NULL);
    srand(seed);
    double r;       // double random number between (0,1)
	int t=0;        // time
    int n_E = N*E;  // number of Excitatory neurons; an integer
    
    double PosCurrent=0;
    double NegCurrent=0;
    
	//--------------------------------------------------------------------
	// making multiplex ER network;
    // A=excitatory layer  ,  B=inhibitory layer
    for ( int i=0 ; i<N; i++ ){
        for ( int j=0 ; j<N ; j++ ){
            if (j==i) continue;
            double r = rand()/double(RAND_MAX);
            if (r<p){
                if (i<n_E)  A[i][j]=1;   //  L_E++; }
                else        B[i][j]=1;   //  L_I++; }
            }
        }
    }
    
	//--------------------------------------------------------------------	      
    double act_E_density=0;  // fraction of excitatory active neurons
    double act_I_density=0;  // fraction of inhibitory active neurons   
    
    // Noise on nodes at t=0
    int N_E_0=n_E*pNodeE;
    int N_I_0=(N-n_E)*pNodeI;        
    for (int i=0 ; i<N_E_0 ; i++){
        node_state[i]=1;
        act_E_density++;
    }     
    for (int i=0 ; i<N_I_0 ; i++){
        node_state[N-i-1]=1;
        act_I_density++;
    }
	    
    //output at t=0
    outputForActiveNodes << t << "\t" << act_E_density/N << "\t" << act_I_density/N << endl;      
        
    //--------------------------------------------------------------------   
    // Starting Dynamics
    t++;
    while ( t<=tmax ){
        
    	// updating link-counters
    	for (int i=0 ; i<N ; i++){
        	for (int j=0 ; j<N ; j++){
                    
            	///// Excitatory links:  
            	if ( A[i][j]!=0 ){
            		// links that remain inactive:
            		if ( node_state[i]==0 && Exc_link_counter[i][j]==0 )
                		Exc_link_counter_updated[i][j] = 0;
                    
					// links that become active as a result of activation of their pre-synaptic neuron
                	else if ( node_state[i]==1 && Exc_link_counter[i][j]==0 )
                    	Exc_link_counter_updated[i][j] = 1;
                    
					// increasing the time counter of active links 
                	else if ( Exc_link_counter[i][j] >= 1 )  
                    	Exc_link_counter_updated[i][j] = Exc_link_counter[i][j] + 1;
				}
            		   
            	///// Inhibitory links:
                if ( B[i][j]!=0 ){
                	// links that remain inactive:
                	if ( node_state[i]==0 && Inh_link_counter[i][j]==0 )
                    	Inh_link_counter_updated[i][j] = 0;
                    
            		// links that become active as a result of activation of their pre-synaptic neuron
                 	else if ( node_state[i]==1 && Inh_link_counter[i][j]==0 )
                    	Inh_link_counter_updated[i][j] = 1;
                    
            		// Increasing the time counter of active links
                	else if ( Inh_link_counter[i][j] >= 1 ) 
                    	Inh_link_counter_updated[i][j] = Inh_link_counter[i][j] + 1;
				}                                         
            }
        }               
            
        ///// updating node states
        // First: obtaining internal node inputs (synaptic current)
        for (int i=0 ; i<N ; i++){
            node_input[i] = 0;
            double positive_input=0;
            double negative_input=0;
            for (int j=0 ; j<N ; j++){
            	PosCurrent=0;
            	NegCurrent=0;
            	if ( A[j][i] * linkState(Exc_link_counter_updated[j][i]) != 0 )
            		PosCurrent = pow( e , (1.-Exc_link_counter_updated[j][i])/TE );
            	positive_input += PosCurrent;
            	if ( B[j][i] * linkState(Inh_link_counter_updated[j][i]) != 0 )
            		NegCurrent = pow( e , (1.-Inh_link_counter_updated[j][i])/TI );
            	negative_input += NegCurrent;

            	if ( cmax*PosCurrent < LowCurLimExc )
            		Exc_link_counter_updated[j][i] = 0;
                if ( 4*cmax*NegCurrent < LowCurLimInh )
                	Inh_link_counter_updated[j][i] = 0;
            }
            node_input[i] = cmax * ( positive_input - 4*negative_input );
        }
            
            
        // Second: obtaining node states (active or inactive) 
        for (int i=0 ; i<N ; i++){
            node_state[i]=0;
            if ( node_input[i] >= D )
                node_state[i]=1;
        }
                
        // Randomly active neurons at t as a result of external input
        for (int i=0 ; i<n_E ; i++){
            r = rand()/double(RAND_MAX);
            if (r<pNodeE) node_state[i]=1;
        }
        for (int i=n_E ; i<N ; i++){
            r = rand()/double(RAND_MAX);
            if (r<pNodeI) node_state[i]=1;
        }    
            
		// counting active nodes
        act_E_density=0;
        act_I_density=0;
        for (int i=0 ; i<n_E ; i++)
            if (node_state[i])
                act_E_density++;
        for (int i=n_E ; i<N ; i++)
            if (node_state[i])
                act_I_density++;    
			
		// output at t for active neurons
        act_E_density /= N;
        act_I_density /= N;
        outputForActiveNodes << t << "\t" << act_E_density << "\t" << act_I_density << endl;

            
        //updating for the next time step
        t++;
        for (int i=0 ; i<N ; i++)
            for (int j=0; j<N ; j++){
                Exc_link_counter[i][j] = Exc_link_counter_updated[i][j];
                Inh_link_counter[i][j] = Inh_link_counter_updated[i][j];
            }
            
    } // end of while
            
    return 0;
}



