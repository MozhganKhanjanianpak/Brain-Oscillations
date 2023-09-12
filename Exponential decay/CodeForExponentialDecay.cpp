///////////////////////////////////////////////////////////////////////////////////////
// Implementation code for ER netwoks with exponentially decay of synapses activation
///////////////////////////////////////////////////////////////////////////////////////
using namespace std;

#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

#define linkState(x)  ( (x==0) ? 0 : 1)

#define N    5000   // Number of nodes
#define p    0.1    // probability of connections in net.
#define E    0.8    // percentage of Excitatory neurons
#define TE   5      // time costant of Excitatory links --> for exp(-t/TE)
#define TI   7      // time costant of Inhibitory links --> for exp(-t/TI)
#define IterE	10  
#define IterI	10
#define cmax  	1   // maximum value of an active link's cuurent
#define pNodeE 0.001  // probability for randomly node activation
#define D    4     // thereshold value for firing
#define tmax 20000    // maximum time
#define pNodeI pNodeE  // probability for randomly node activation

const double  e = 2.71828182845904523536;

int  A[N][N] = {0};               // Adjacency Matrix
int node_state[N] = {0};          // state of neurons, including: 0=inactive , 1=active
double node_input[N] = {0};          // imput signal of neurons
int Link_counter[N][N] = {0}; // including 0, 1, 2, ...,TE for 0<row<n_E and 0<col<N
int Link_counter_updated[N][N] = {0}; // and 0, 1, 2, ...,TI for n_E<row<N and 0<col<N

/********************************************/
// Main
/********************************************/
int main()
{
    ofstream output("./ActiveNodeDensity.txt"); //output for the neurons
    output << "#time,	NE,	NI" << endl;
    
    unsigned seed = time(NULL);
    srand(seed);
    double r;       // double random number between (0,1)
    int t=0;        // time
    int n_E = N*E;  // number of Excitatory neurons; an integer
    
    double act_E_density=0;
    double act_I_density=0;
    
    //-----------------------------------------------
    // making Adjacency matrix of ER network (Directed)
    for ( int i=0 ; i<N; i++ ){
        for ( int j=0 ; j<N ; j++ ){
            if (j==i) continue;
            double r = rand()/double(RAND_MAX);
            if (r<p){
                A[i][j]=1;  
            }
        }
    }
    
    //------------------------------------------------
        
    // Noise on nodes at t=0
    int N_E_0=n_E*pNodeE;
	int N_I_0=(N-n_E)*pNodeI;
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
                		
              		if ( (node_state[i]==0) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 0;
                       	
                    else if ( (node_state[i]==1) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 1;	
                        
                    else if ( (Link_counter[i][j] >= 1) && (Link_counter[i][j] < IterE) )
                       	Link_counter_updated[i][j] = Link_counter[i][j] + 1;
                        
					else if ( Link_counter[i][j] == IterE )
                       	Link_counter_updated[i][j] = 0;	
				}
        	}
    	}
        
		// updating link-counters for Inhibitory links
        for (int i=n_E ; i<N ; i++){
            for (int j=0 ; j<N ; j++){
              	if ( A[i][j]!=0 ){
                		
               		if ( (node_state[i]==0) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 0;
                        	
                    else if ( (node_state[i]==1) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 1;	
                        
                    else if ( (Link_counter[i][j] >= 1) && (Link_counter[i][j] < IterI) )
                       	Link_counter_updated[i][j] = Link_counter[i][j] + 1;
                        
					else if ( Link_counter[i][j] == IterI )
                       	Link_counter_updated[i][j] = 0;	
				}	
			}       
        }
                    	
		    
        // updating node states
        // First: node inputs
        for (int i=0 ; i<N ; i++){
            node_input[i] = 0;
            
			double positive_input=0;
            for (int j=0 ; j<n_E ; j++){
            	double PosCurrent=0;
            	if (  linkState(Link_counter_updated[j][i]) )
            		PosCurrent = A[j][i] * pow( e , (1.-Link_counter_updated[j][i])/TE );
            	positive_input += PosCurrent;
			}
            
            double negative_input=0;
            for (int j=n_E ; j<N ; j++){
            	double NegCurrent=0;
            	if ( linkState(Link_counter_updated[j][i]) )
            		NegCurrent = A[j][i] * pow( e , (1.-Link_counter_updated[j][i])/TI );
            	negative_input += NegCurrent;
			}
            
            node_input[i] = cmax * (positive_input - 4*negative_input);
    	}
            
        // Second: node-state
        for (int i=0 ; i<N ; i++){
            node_state[i]=0;
            if ( node_input[i] >= D )
                node_state[i]=1;
        }
        
				    
        // Noise on nodes at t
        for (int i=0 ; i<n_E ; i++){
            r = rand()/double(RAND_MAX);
            if (r<pNodeE) node_state[i]=1;
        }
        for (int i=n_E ; i<N ; i++){
            r = rand()/double(RAND_MAX);
            if (r<pNodeI) node_state[i]=1;
        }
            
        // counting active nodes
        //active_density=0;
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



