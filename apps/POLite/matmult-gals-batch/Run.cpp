// SPDX-License-Identifier: BSD-2-Clause
#include "matrixmult.h"
#include "matrices.h"

#include <HostLink.h>
#include <POLite.h>
#include <sys/time.h>

/*****************************************************
 * Matrix Multiplier - Asynchronous - Batch Style
 * ***************************************************
 * This code multiplies the matrices specificfied in matrices.cpp.
 * USAGE:
 * 1. Matrix A and Matrix B to be defined in matrices.cpp
 * 2. Matrix A and Matrix B dimensions to be defined in matrices.h.
 * 
 * PLEASE NOTE:
 * Implementation checks whether mulitplication is possible.
 * (Matrix A Cols == Matrix B Rows)
 * ****************************************************/

int main() {
    
    if (!mult_possible) {
        printf("Multilpication not possible with supplied matrices!\n");
    }
    else {

        // Connection to tinsel machine
        HostLink hostLink;

        // Create POETS graph
        PGraph<MatDevice, MatState, None, MatMessage> graph;

        // Create 3D mesh of devices
        PDeviceId mesh[MESHLEN][MESHWID][MESHHEI];
        for (uint32_t x = 0; x < MESHLEN; x++) {
            for (uint32_t y = 0; y < MESHWID; y++) {
                for (uint32_t z = 0; z < MESHHEI; z++) {
                    
                    mesh[x][y][z] = graph.newDevice();
                    
                }
            }
        }
        
        // Add edges
        for (uint32_t x = 0; x < MESHLEN; x++) {
            for (uint32_t y = 0; y < MESHWID; y++) {
                for (uint32_t z = 0; z < MESHHEI; z++) {
                    
                    if (z < MESHHEI-1) {
                        graph.addEdge(mesh[x][y][z], 0, mesh[x][y][z+1]);
                    }
                    
                }
            }
        } 

        // Prepare mapping from graph to hardware
        graph.map();
                    
        // Initialise device coordinates/dimensions and starting matrices
        for (uint32_t x = 0; x < MESHLEN; x++) {
            for (uint32_t y = 0; y < MESHWID; y++) {
                for (uint32_t z = 0; z < MESHHEI; z++) {
                    
                    // Initialise device IDs
                    graph.devices[mesh[x][y][z]]->state.id = mesh[x][y][z];
                    
                    // Initialise Mesh coordinates on devices
                    graph.devices[mesh[x][y][z]]->state.x = x;
                    graph.devices[mesh[x][y][z]]->state.y = y;
                    graph.devices[mesh[x][y][z]]->state.z = z;

                    //Inform each device of matrix size for message passing decisions
                    graph.devices[mesh[x][y][z]]->state.xmax = MESHLEN;
                    graph.devices[mesh[x][y][z]]->state.ymax = MESHWID;
                    graph.devices[mesh[x][y][z]]->state.zmax = MESHHEI;
                    
                    // From maxtrix A
                    graph.devices[mesh[x][y][z]]->state.element1 = matrixA[y][z];

                    // From maxtrix B
                    graph.devices[mesh[x][y][z]]->state.element2 = matrixB[z][x];
                    
                    // Initialise variables prior to starting sumulation
                    graph.devices[mesh[x][y][z]]->state.aggregate = 0;
                    graph.devices[mesh[x][y][z]]->state.from = 0;
                    graph.devices[mesh[x][y][z]]->state.trackflag = 0;
                    
                }
            }
        }

        // Write graph down to tinsel machine via HostLink
        graph.write(&hostLink);

        // Load code and trigger execution
        hostLink.boot("code.v", "data.v");
        hostLink.go();
        printf("Starting\n");

        // Start timer
        struct timeval start, finish, diff;
        gettimeofday(&start, NULL);

        // Allocate array to contain final value of each device
        uint32_t result[MESHLEN][MESHWID] {};

        // Receive final value of each device
        for (uint32_t i = 0; i < RETMATSIZE; i++) {
            
            // Receive message
            PMessage<None, MatMessage> msg;
            hostLink.recvMsg(&msg, sizeof(msg));
            if (i == 0) gettimeofday(&finish, NULL);
            
            // Save final value
            result[graph.devices[msg.payload.from]->state.x][graph.devices[msg.payload.from]->state.y] = msg.payload.aggregate;
            
        }

        // Display time
        timersub(&finish, &start, &diff);
        double duration = (double) diff.tv_sec + (double) diff.tv_usec / 1000000.0;
        printf("Time = %lf\n", duration);

        for (uint32_t y = 0; y < MESHWID; y++) {
            for (uint32_t x = 0; x < MESHLEN; x++) {
                
                printf("%d ", result[x][y]);
                
            }
            
            printf("\n");
            
        }
    
    }
    
    return 0;

}
