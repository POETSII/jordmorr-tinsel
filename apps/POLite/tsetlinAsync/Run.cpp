// SPDX-License-Identifier: BSD-2-Clause
#include "tsetlin.h"
#include "data.h"
#include "myPOLite.h"
#include "params.h"

#include <HostLink.h>
#include <EdgeList.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*****************************************************
 * Tsetlin Machine - POETS Asynchronous Version
 * ***************************************************
 * This code performs the distributed, parallelised, asynchronous version of the Tselin Machine as set out in the paper below
 *  https://arxiv.org/abs/2009.04861
 * USAGE:
 * To Be Completed ...
 * 
 * PLEASE NOTE:
 * To Be Completed ...
 * 
 * ssh jordmorr@byron.cl.cam.ac.uk
 * scp -r C:\Users\drjor\Documents\tinsel\apps\POLite\tsetlinAsync jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps/POLite
 * scp jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps/POLite/tsetlinAsync/build/results.csv C:\Users\drjor\Documents\tinsel\apps\POLite\tsetlinAsync
 * 
 * ssh jordmorr@byron.cl.cam.ac.uk
 * scp -r /Users/jordanmorris/Documents/tinsel/apps/POLite/tsetlinAsync jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps/POLite
 * scp jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps/POLite/tsetlinAsync/build/results.csv /Users/jordanmorris/Documents/tinsel/apps/POLite/tsetlinAsync
 * 
 * ****************************************************/

int main(int argc, char **argv)
{
    
    // Start timer for mesh creation and mapping
    //struct timeval start_map, finish_map, diff_map;
    //gettimeofday(&start_map, NULL);
    
    // Seed random number generator
        // Seed random number generator
    time_t t;
    srand((unsigned) time(&t));
    
    // Connection to tinsel machine
    //HostLink hostLink;
    HostLink hostLink(1, 1);

    // Create POETS graph
    //PGraph<ImpDevice, ImpState, None, ImpMessage> graph;
    PGraph<ImpDevice, ImpState, None, ImpMessage> graph(1, 1);

    // Create 1D array of devices
    static PDeviceId mesh[TRAINSIZE];
    for (uint32_t x = 0; x < TRAINSIZE; x++) {
        
            mesh[x] = graph.newDevice();
            
    }
    
    
    
    // Add unicast edges
    for (uint32_t x = 0; x < TRAINSIZE ; x++) {
        for (uint32_t y = 0; y < TRAINSIZE; y++) {
                
            // Add unicast addresses
            graph.addEdge(mesh[x], y, mesh[y]);
            
        }
    }
    
    for (uint32_t x = 0; x < TRAINSIZE ; x++) {
        for (uint32_t y = 0; y < TRAINSIZE; y++) {
                
            // Add multicast addresses
            graph.addEdge(mesh[x], TRAINSIZE, mesh[y]);
                
            
        }
    }
    
    // Prepare mapping from graph to hardware
    graph.mapVerticesToDRAM = true;
    graph.map();
    
    // Record map time
    // gettimeofday(&finish_map, NULL);
    // timersub(&finish_map, &start_map, &diff_map);
    // double map_duration = (double) diff_map.tv_sec + (double) diff_map.tv_usec / 1000000.0;
    
    // Start timer for mesh init
    //struct timeval start_init, finish_init, diff_init;
    //gettimeofday(&start_init, NULL);
    
    // Initialise device coordinates/dimensions
    for (uint32_t x = 0; x < TRAINSIZE; x++) {
                
            // Initialise device IDs
            graph.devices[mesh[x]]->state.id = mesh[x];
            
            // Assign node it's clause number
            graph.devices[mesh[x]]->state.clauseNo = x;
            
            // Set initial states of Tsetlin Automata
            for (uint32_t y = 0; y < NOOFLITERALS; y++) {
            
                double random = (double)rand() / (double)RAND_MAX;
                    
                if (random <= 0.5f) {
                    graph.devices[mesh[x]]->state.ta[y] = EXCBOUND;
                }
                else {
                    graph.devices[mesh[x]]->state.ta[y] = INCBOUND;
                }
                
                graph.devices[mesh[x]]->state.seed = rand();
            
            }
            
            uint32_t datapoint = 0u;
                
            // Accumulate Datapoint In
            for (uint32_t y = 0; y < NOOFLITERALS; y++) {
                
                datapoint = (datapoint << 1) + in_train[x][y];
                
            }
            
            graph.devices[mesh[x]]->state.dpX = datapoint;
                
            graph.devices[mesh[x]]->state.dpY = out_train[x]; 
               
    }
    
    // Write graph down to tinsel machine via HostLink
    printf("Writing Graph . . . \n");
    graph.write(&hostLink);
    printf("Graph Written!\n");

    // Load code and trigger execution
    printf("Loading Code . . . \n");
    hostLink.boot("code.v", "data.v");
    printf("Code Loaded!\n");
    printf("Starting Nodes . . . \n");
    hostLink.go();
    printf("Nodes Started!\n");
    
    // Consume performance stats
    //politeSaveStats(&hostLink, "stats.txt");
  
    
#ifdef IMPDEBUG
        
        /*
        // Allocate array to contain final value of each device
        static float result[NOOFOBS] {};

        // Receive final value of each device
        for (uint32_t i = 0; i < (NOOFOBS); i++) {
            
            //printf("%d received \n", i);
            
            // Receive message
            PMessage<ImpMessage> msg;
            hostLink.recvMsg(&msg, sizeof(msg));

            // Save final value
            result[graph.devices[msg.payload.msgtype]->state.x] = msg.payload.val;
            
        }

        //Create a file pointer
        FILE * fp;
        // open the file for writing
        fp = fopen ("results.csv","w");

        uint32_t alphaSelect = 1u;
        
        if (alphaSelect) {

            for (uint32_t x = 0u; x < NOOFOBS; x++) {
            
                if (x != (NOOFOBS - 1u) ) {
                    fprintf(fp, "%e,", result[x]);
                }
                else {
                    fprintf(fp, "%e", result[x]);
                }
            
            }
                
        }
        else {
            
            for (uint32_t x = 0u; x < NOOFOBS; x++) {
        
                if (x != (NOOFOBS - 1u) ) {
                    fprintf(fp, "%e,", result[(NOOFOBS - 1) - x]);
                }
                else {
                    fprintf(fp, "%e", result[(NOOFOBS - 1) - x]);
                }
        
            }  

        }
        
        // close the file 
        fclose (fp);
        */
        
        static float result[NOOFSTATES][NOOFTARG] {};

        // Receive final value of each device
        for (uint32_t i = 0; i < (NOOFSTATES*NOOFTARG); i++) {
            
            // Receive message
            PMessage<ImpMessage> msg;
            hostLink.recvMsg(&msg, sizeof(msg));

            // Save final value
            result[graph.devices[msg.payload.msgtype]->state.y][graph.devices[msg.payload.msgtype]->state.x] = msg.payload.val;
            
        }

        //Create a file pointer
        FILE * fp;
        // open the file for writing
        fp = fopen ("results.csv","w");
        
        uint32_t alphaSelect = 1u;
        
        if (alphaSelect) {
        
            for (uint32_t y = 0u; y < NOOFSTATES; y++) {
                for (uint32_t x = 0u; x < NOOFTARG; x++) {
                
                    if (x != (NOOFTARG - 1u) ) {
                        fprintf(fp, "%e,", result[y][x]);
                    }
                    else {
                        fprintf(fp, "%e", result[y][x]);
                    }
                
                }
                fprintf(fp, "\n");
            }
        }
        else {
            for (uint32_t y = 0u; y < NOOFSTATES; y++) {
                for (uint32_t x = 0u; x < NOOFTARG; x++) {
                
                    if (x != (NOOFTARG - 1u) ) {
                        fprintf(fp, "%e,", result[y][(NOOFTARG - 1) - x]);
                    }
                    else {
                        fprintf(fp, "%e", result[y][(NOOFTARG - 1) - x]);
                    }
                
                }
                fprintf(fp, "\n");
            }
        }
        // close the file 
        fclose (fp);
       
#else

    static float result[TRAINSIZE] {};

    // Receive final value of each device
    for (uint32_t i = 0; i < TRAINSIZE; i++) {
            
            // Receive message
            PMessage<ImpMessage> msg;
            hostLink.recvMsg(&msg, sizeof(msg));

            // Save final value
            result[graph.devices[msg.payload.srcClause]->state.clauseNo] = msg.payload.vote;
            
    }
    
    for (uint32_t i = 0; i < TRAINSIZE; i++) {
     
        printf("%d, %f \n", i, result[i]);
        
    }

    // Receive first message and close timer
    //PMessage<ImpMessage> msg;
    //hostLink.recvMsg(&msg, sizeof(msg));
    
    //gettimeofday(&finish_proc, NULL);
    //timersub(&finish_proc, &start_proc, &diff_proc);
    //double proc_duration = (double) diff_proc.tv_sec + (double) diff_proc.tv_usec / 1000000.0;
    //printf("%lf\n", proc_duration);
        
#endif

    printf("Finished\n");
    
    return 0;
}
