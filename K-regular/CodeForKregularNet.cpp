////////////////////////////////////////////////
// Implementation code for k-regular network  //
////////////////////////////////////////////////
using namespace std;

#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

#define linkState(x)  ( (x==0) ? 0 : 1)

#define N    5000   // Number of nodes
#define E    0.8    // percentage of Excitatory neurons
#define TE   5      // activation time of Exc. links
#define TI   7      // activation time of Inh. links
#define D    4     // neuron firing threshold value
#define tmax 20000    // implementation maximum time
#define pNodeE 0.001  // randomly excitatory node activation probability
#define pNodeI pNodeE  // randomly inhibitory node activation probability


int  A[N][N] = {0};             // Adjacency Matrix of Excitatory links
int  B[N][N] = {0};             // Adjacency Matrix of Inhibitory links
int node_state[N] = {0};    // state of neurons at odd times, including: 0=inactive , 1=active
int node_input[N] = {0};        // neuron input

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
    int randNode;    // integer random number
	int t=0;        // time
    int n_E = N*E;  // number of Excitatory neurons; an integer
    
    
    // INPUT; the adjacency matrix A 
    ifstream input_file("./AdjMatListKregular.txt"); 
    int num1,num2,i,j;
 	input_file >> num1 >> num2 ;
 	for (int counter=0; counter<num2; counter++){
    	input_file >> i >> j ;
		A[i][j] = 1;
	}
	// creating matrix B according to A
    for (int i=n_E ; i<N ; i++){
    	for (int j=0 ; j<N ; j++){
    		if (A[i][j] != 0){
    			B[i][j] = A[i][j];
    			A[i][j] = 0;
			}
		}
	}   
        
    double act_E_density=0;  // fraction of excitatory active neurons
    double act_I_density=0;  // fraction of inhibitory active neurons
        
    
    // active neurons at t=0
    int N_E_0=n_E*pNodeE;      // initial number of active excitatory neurons
    int N_I_0=(N-n_E)*pNodeI;  // initial number of active inhibitory neurons
    int counter=0;
    // Excitatory neurons
	while (counter<N_E_0){
    	randNode = rand()%n_E;  // a random excitatory neuron
    	if (node_state[randNode] == 0){
    		node_state[randNode]=1;
    		counter++;
		}
	}
	// Inhibitory neurons
	counter=0;
    while (counter<N_I_0){
    	randNode = rand()%(N-n_E);  // a random inhibitory neuron
    	if (node_state[n_E+randNode] == 0){
    		node_state[n_E+randNode]=1;
    		counter++;
		}
	}
        
    //output at t=0
    outputForActiveNodes << t << "\t" << pNodeE << "\t" << pNodeI << endl;      
     
	    
    // Starting Dynamics
    t++;
    while ( t<=tmax ){
        
    	// updating link-counters
    	for (int i=0 ; i<N ; i++){
        	for (int j=0 ; j<N ; j++){
                    
            	///// Excitatory links:
                    
            	// links that remain inactive:
            	if ( (A[i][j]!=0) && (node_state[i]==0) && (Exc_link_counter[i][j]==0)  )
                Exc_link_counter_updated[i][j] = 0;
                    
				// links that become active as a result of activation of their pre-synaptic neuron
                else if ( (A[i][j]!=0) && (node_state[i]==1) && (Exc_link_counter[i][j]==0)  )
                    Exc_link_counter_updated[i][j] = 1;
                    
				// active links that their time counter is less than TE yet
                else if ( (A[i][j]!=0) && (Exc_link_counter[i][j] >= 1) && (Exc_link_counter[i][j] < TE) )
                    Exc_link_counter_updated[i][j] = Exc_link_counter[i][j] + 1;
                    
				// active links that their time counter is reached TE -> become inactive
                else if ( (A[i][j]!=0) && (Exc_link_counter[i][j] == TE) )
                    Exc_link_counter_updated[i][j] = 0;
                    
                    
            	///// Inhibitory links:
                    
            	// links that remain inactive:
                if ( (B[i][j]!=0) && (node_state[i]==0) && (Inh_link_counter[i][j]==0)  )
                    Inh_link_counter_updated[i][j] = 0;
                    
            	// links that become active as a result of activation of their pre-synaptic neuron
                 else if ( (B[i][j]!=0) && (node_state[i]==1) && (Inh_link_counter[i][j]==0)  )
                    Inh_link_counter_updated[i][j] = 1;
                    
            	// active links that their time counter is less than TI yet
                else if ( (B[i][j]!=0) && (Inh_link_counter[i][j] >= 1) && (Inh_link_counter[i][j] < TI) )
                    Inh_link_counter_updated[i][j] = Inh_link_counter[i][j] + 1;
                    
            	// active links that their time counter is reached TI -> become inactive
                else if ( (B[i][j]!=0) && (Inh_link_counter[i][j] == TI) )
                    Inh_link_counter_updated[i][j] = 0;                    
            }
        }
                
            
        ///// updating node states
        // First: obtaining internal node inputs (synaptic current)
        for (int i=0 ; i<N ; i++){
            node_input[i] = 0;
            int positive_input=0;
            int negative_input=0;
            for (int j=0 ; j<N ; j++){
                positive_input += ( A[j][i] *  linkState(Exc_link_counter_updated[j][i]) );
                negative_input += ( B[j][i] *  linkState(Inh_link_counter_updated[j][i]) );
            }
            node_input[i] = positive_input - 4*negative_input;
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



