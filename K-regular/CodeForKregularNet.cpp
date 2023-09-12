////////////////////////////////////////////////
// Implementation code for k-regular netwok  //
////////////////////////////////////////////////
using namespace std;

#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

#define linkState(x)  ( (x==0) ? 0 : 1)  // inline function

#define N    5000   // Number of nodes
#define E    0.8    // percentage of Excitatory neurons
#define TE   5      // activation time of Exc. links
#define pNodeE 0.001  // probability for randomly node activation of Exc. neurons
#define TI   7      // activation time of Inh. links
#define D    4     // thereshold value for firing
#define tmax 25000    // maximum time
#define pNodeI pNodeE  // probability for randomly node activation of Inh. neurons

int  A[N][N] = {0};               // Adjacency Matrix
int node_state[N] = {0};          // state of neurons, including: 0=inactive , 1=active
int node_input[N] = {0};          // neuron input
int Link_counter[N][N]         = {0}; // including 0, 1, 2, ...,TE for 0<row<n_E and 0<col<N
int Link_counter_updated[N][N] = {0}; // and 0, 1, 2, ...,TI for n_E<row<N and 0<col<N

/********************************************/
// Main
/********************************************/
int main()
{
    ofstream output("./ActiveNodeDensity.txt"); //output for active neurons
	output << "#time,	NE,	NI" << endl;
      
    
    unsigned seed = time(NULL);
    srand(seed);
    double r;       // double random number between (0,1)
    int t=0;        // time
    int n_E = N*E;  // number of Excitatory neurons
    
    // neurons
	double act_E_density=0; // fraction of excitatory active neurons
    double act_I_density=0; // fraction of inhibitory active neurons
    
    
    //-----------------------------------------------
    // INPUT; the adjacency matrix A 
    ifstream input_file("./AdjMatListKregular.txt"); 
    int num1,num2,i,j;
 	input_file >> num1 >> num2 ;
 	for (int counter=0; counter<num2; counter++){
    	input_file >> i >> j ;
		A[i][j] = 1;
	}
    
    
    //------------------------------------------------
        
    // Noise on nodes at t=0
    int N_E_0=n_E*pNodeE;      // initial number of active excitatory neurons
	int N_I_0=(N-n_E)*pNodeI;  // initial number of active inhibitory neurons
    
    
	// becomming active
	// Excitatory
    for (int i=0 ; i<N_E_0 ; i++){
        node_state[i]=1;
        act_E_density++;
    }
    // Inhibitory
    for (int i=0 ; i<N_I_0 ; i++){
        node_state[N-i-1]=1;
        act_I_density++;
    }
     
	    
    //output at t=0
    // neurons
    act_E_density /= N;
    act_I_density /= N;
    output << t << "\t" << act_E_density << "\t" << act_I_density << endl;
    
    
    
    // Starting Dynamics
    t++;
    while ( t<=tmax ){
            
        // updating link-counters for Excitatory links
        for (int i=0 ; i<n_E ; i++){
            for (int j=0 ; j<N ; j++){
              	if ( A[i][j]!=0 ){
                	
					// links that remain inactive	
              		if ( (node_state[i]==0) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 0;
                     
					// links that become active as a result of activation of thier pre-synaptic neuron  	
                    else if ( (node_state[i]==1) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 1;	
                    
					// active links whose time counter is less than TE   
                    else if ( (Link_counter[i][j] >= 1) && (Link_counter[i][j] < TE) )
                       	Link_counter_updated[i][j] = Link_counter[i][j] + 1;
                        
                    // active links whose time counter is TE --> become inactive
					else if ( Link_counter[i][j] == TE )
                       	Link_counter_updated[i][j] = 0;	
				}
        	}
    	}
        
		// updating link-counters for Inhibitory links
        for (int i=n_E ; i<N ; i++){
            for (int j=0 ; j<N ; j++){
              	if ( A[i][j]!=0 ){
                	
					// links that remain inactive	
               		if ( (node_state[i]==0) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 0;
                    
					// links that become active as a result of activation of thier pre-synaptic neuron    	
                    else if ( (node_state[i]==1) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 1;	
                    
					// active links whose time counter is less than TI    
                    else if ( (Link_counter[i][j] >= 1) && (Link_counter[i][j] < TI) )
                       	Link_counter_updated[i][j] = Link_counter[i][j] + 1;
                    
					// active links whose time counter is TI --> become inactive    
					else if ( Link_counter[i][j] == TI )
                       	Link_counter_updated[i][j] = 0;	
				}	
			}       
        }
        
		
		    
        // updating node states
        // First: node inputs
        for (int i=0 ; i<N ; i++){
            node_input[i] = 0;
            // positive
			int positive_input=0;
            for (int j=0 ; j<n_E ; j++){
            	positive_input += ( A[j][i] *  linkState(Link_counter_updated[j][i]) );
			}
            // negative
            int negative_input=0;
            for (int j=n_E ; j<N ; j++){
            	negative_input += ( A[j][i] *  linkState(Link_counter_updated[j][i]) );
			}
            // total input
            node_input[i] = positive_input - 4*negative_input;
    	}   
        // Then: node-state
        for (int i=0 ; i<N ; i++){
            node_state[i]=0;
            if ( node_input[i] >= D )
                node_state[i]=1;
        }
        
		    
        // external input on nodes at t
        // excitatory neurons
        for (int i=0 ; i<n_E ; i++){
            r = rand()/double(RAND_MAX);
            if (r<pNodeE) node_state[i]=1;
        }
        // inhibitory neurons
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
          
		   
        // output at t
        // active neurons
        act_E_density /= N;
        act_I_density /= N;
        output << t << "\t" << act_E_density << "\t" << act_I_density << endl;
        
		    
        //updating for the next time step
        t++;
        for (int i=0 ; i<N ; i++){
        	for (int j=0; j<N ; j++){
                Link_counter[i][j] = Link_counter_updated[i][j];
            }
		}
                     
    } // end of while
        
    return 0;
}



