#define TINSEL (1u)
//#define PRINTDEBUG (1u)

#include "model.h"
#include <HostLink.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "../../include/POLite/ProgRouters.h"

//#define DEBUGRETURNS (1)

/*****************************************************
 * Petri Net Test - Tinsel Version
 * ***************************************************
 * This code creates a simple, tileable static data flow structure (SDFS)
 * https://eprint.ncl.ac.uk/75784
 * 
 * USAGE:
 * To Be Completed ...
 * 
 * PLEASE NOTE:
 * To Be Completed ...
 * 
 * ssh jordmorr@byron.cl.cam.ac.uk
 * scp -r C:\Users\drjor\Documents\tinsel\apps\PNTest jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps
 * scp jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps/PNTest/places.csv C:\Users\drjor\Documents\tinsel\apps\PNTest
 * scp jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps/PNTest/trans.csv C:\Users\drjor\Documents\tinsel\apps\PNTest
 * ****************************************************/

int main()
{
    // Create the hostlink (with a single box dimension by default)
    HostLink hostLink(2, 4);

    // Create the programmable router mesh with single box dimensions
    //ProgRouterMesh progRouterMesh(6, 8);
    
    // Generate and transmit pre-execution data
	
    // Start Initialisation Timer
    struct timeval start_init, finish_init, diff_init;
    gettimeofday(&start_init, NULL);
    
    /********************************************************
     * INTIALISATION
     * *****************************************************/
    
    for (uint8_t board = 0u; board < (NOOFBOXES * (TinselBoardsPerBox - 1u)); board++) {
    
        for (uint8_t mailbox = 0u; mailbox < TinselMailboxesPerBoard; mailbox++) {
            
            for (uint8_t col = 0u; col < (TinselCoresPerMailbox * NOOFHWCOLSPERCORE); col++) {
                
                /********************************************************
                * HARDWARE LAYER SETUP
                * *****************************************************/
                
                //Global Column Number
                uint32_t HWColumn = (board * TinselCoresPerMailbox * NOOFHWCOLSPERCORE * TinselMailboxesPerBoard) + (mailbox * TinselCoresPerMailbox * NOOFHWCOLSPERCORE) + col;
                
                //Base Thread
                uint32_t prevBaseThread = 0u;
                uint32_t nextBaseThread = 0u;
                
                // Destination Mailbox
                uint32_t destMbox = 0u;
                
                // FORWARD THREAD ADDRESSES
                
                // If we are the last col on the current board
                if ( (col == ((TinselCoresPerMailbox * NOOFHWCOLSPERCORE) - 1u)) && (mailbox == TinselMailboxesPerBoard - 1u) ) {
                    
                    // If we arent in the last col for the setup
                    if ( !(board == ((NOOFBOXES * (TinselBoardsPerBox - 1u)) - 1u)) ) {
                        
                        // Construct destination the mailbox for the next board
                        destMbox = boardPath[board + 1u][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[board + 1u][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[0u][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[0u][0u];
                        
                        nextBaseThread = (destMbox << TinselLogThreadsPerMailbox);
                        
                    }
                    // If we are the last col for the hw setup, loop back to the beginning
                    else {
                        // Construct destination the mailbox for the next board
                        destMbox = boardPath[0u][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[0u][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[0u][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[0u][0u];
                        
                        nextBaseThread = (destMbox << TinselLogThreadsPerMailbox);

                    }
                    
                }
                // If we are the last col in the current mailbox
                else if ( col == ((TinselCoresPerMailbox * NOOFHWCOLSPERCORE) - 1u) ) {
                    
                        // Construct destination the next mailbox
                        destMbox = boardPath[board][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[board][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[mailbox + 1u][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[mailbox + 1u][0u];
                        
                        nextBaseThread = (destMbox << TinselLogThreadsPerMailbox);
                        
                    
                }
                // If we are just a regular col
                else {
                    
                        // Construct destination the same mailbox
                        destMbox = boardPath[board][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[board][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[mailbox][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[mailbox][0u];
                        
                        nextBaseThread = (destMbox << TinselLogThreadsPerMailbox) + ((col + 1u) * 8u);
                        
                    
                }
                
                // BACKWARD THREAD ADDRESSES 
                
                // If we are the first col on the current board
                if ( (col == 0u) && (mailbox == 0u) ) {
                    
                    // If we arent in the first col for the hw setup
                    if ( !(board == 0u) ) {
                        
                        // Construct destination the mailbox for the previous board
                        destMbox = boardPath[board - 1u][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[board - 1u][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[TinselMailboxesPerBoard - 1u][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[TinselMailboxesPerBoard - 1u][0u];
                        
                        prevBaseThread = (destMbox << TinselLogThreadsPerMailbox) + (7u * 8u);
                        
                    }
                    // If we are the first col for the hw setup, loop back around to the last col
                    else {
                        
                        // Construct destination the mailbox for the last board
                        destMbox = boardPath[((TinselBoardsPerBox - 1u) * NOOFBOXES) - 1u][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[((TinselBoardsPerBox - 1u) * NOOFBOXES) - 1u][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[TinselMailboxesPerBoard - 1u][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[TinselMailboxesPerBoard - 1u][0u];
                        
                        prevBaseThread = (destMbox << TinselLogThreadsPerMailbox) + (7u * 8u);

                    }
                    
                }
                // If we are the first col in the current mailbox
                else if (col == 0u) {
                    
                        // Construct destination the next mailbox
                        destMbox = boardPath[board][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[board][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[mailbox - 1u][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[mailbox - 1u][0u];
                        
                        prevBaseThread = (destMbox << TinselLogThreadsPerMailbox) + (7u * 8u);
                        
                    
                }
                // If we are just a regular col
                else {
                    
                        // Construct destination the same mailbox
                        destMbox = boardPath[board][1u];
                        destMbox = (destMbox << TinselMeshXBits) + boardPath[board][0u];
                        destMbox = (destMbox << TinselMailboxMeshYBits) + mailboxPath[mailbox][1u];
                        destMbox = (destMbox << TinselMailboxMeshXBits) + mailboxPath[mailbox][0u];
                        
                        prevBaseThread = (destMbox << TinselLogThreadsPerMailbox) + ((col - 1u) * 8u);
                    
                }
                
                for (uint32_t thread = 0u; thread < NOOFHWROWS; thread++) {
                
                    uint32_t threadID = 0u;
                        
                    // Construct ThreadID
                    threadID = boardPath[board][1u];
                    threadID = (threadID << TinselMeshXBits) + boardPath[board][0u];
                    threadID = (threadID << TinselMailboxMeshYBits) + mailboxPath[mailbox][1u];
                    threadID = (threadID << TinselMailboxMeshXBits) + mailboxPath[mailbox][0u];
                    threadID = (threadID << TinselLogThreadsPerMailbox) + (col * 8u) + thread;
                    
                    uint8_t coreID = (mailboxPath[mailbox][1u] * TinselMailboxMeshXLen * TinselCoresPerMailbox) + (mailboxPath[mailbox][0u] * TinselCoresPerMailbox);

                    uint32_t baseAddress = tinselHeapBaseGeneric(threadID);
                    
                    hostLink.setAddr(boardPath[board][0u], boardPath[board][1u], coreID, baseAddress);
                    
                    hostLink.store(boardPath[board][0u], boardPath[board][1u], coreID, 1u, &HWColumn);
                    
                    uint32_t prevThread = prevBaseThread + thread;
                    uint32_t nextThread = nextBaseThread + thread;
                    
                    hostLink.store(boardPath[board][0u], boardPath[board][1u], coreID, 1u, &prevThread);
                    hostLink.store(boardPath[board][0u], boardPath[board][1u], coreID, 1u, &nextThread);
                    
                    /********************************************************
                    * PERTI NET NETLIST
                    * *****************************************************/
                    
                    // Transmit netlist
                    
                    for (uint32_t element = 0u; element < NOOFELEMENTS; element++) {
                        
                        for (uint32_t node = 0u; node < 7u; node++) {
                            
                            uint32_t netlistElement = netlist[element][node];
                            
                            hostLink.store(boardPath[board][0u], boardPath[board][1u], coreID, 1u, &netlistElement);
                            
                        }
                        
                    }
                    
                    // Transmit inital place marking
                    
                    for (uint32_t p = 0u; p < NOOFPLACES; p++) {
                        
                        uint32_t marking = place[p];
                        
                        hostLink.store(boardPath[board][0u], boardPath[board][1u], coreID, 1u, &marking);
                    }

                
                }
                
            }
            
        }
    
    }
    
    /********************************************************
     * WRITE MESH AND START GRAPH
     * *****************************************************/
    
    // Write the keys to the routers
    //progRouterMesh.write(&hostLink);
    
    // Record init time
    gettimeofday(&finish_init, NULL);
    timersub(&finish_init, &start_init, &diff_init);
    double init_duration = (double) diff_init.tv_sec + (double) diff_init.tv_usec / 1000000.0;

    printf("Init Time = %0.5f\n", init_duration);
    printf("Launching\n");
    // Load the correct code into the cores
    hostLink.boot("code.v", "data.v");
    hostLink.go();
    
    // Start Processing Timer
    struct timeval start_proc, finish_proc, diff_proc;
    gettimeofday(&start_proc, NULL);

    /********************************************************
     * RECEIVE RESULTS
     * *****************************************************/
    
    HostMessage msg;
    
    hostLink.recvMsg(&msg, sizeof(msg));
    
    uint32_t lowerCnt = msg.observationNo;
    uint64_t upperCnt = msg.stateNo;
    
    double procTime = ((upperCnt << 32) + lowerCnt) / (TinselClockFreq * 1000000.0);
    
    printf("Cycle Proc Time = %.15f\n", procTime);
    
    // ----> FOR DEBUG ONLY
    /*
    uint32_t noOfResults = 10000u;
    uint32_t result[NOOFPLACES][noOfResults] = {0u};
    uint32_t active[NOOFTRANS][noOfResults] = {0u};
    uint32_t activeCnt[noOfResults] = {0u};
    uint32_t r[noOfResults] = {0u};
    uint32_t trans[noOfResults][2u] = {0u};
    
    for (uint32_t t = 0u; t < noOfResults; t++) {

        for (uint32_t p = 0u; p < NOOFPLACES + 1u; p++) {
            
            hostLink.recvMsg(&msg, sizeof(msg));
            
            if (msg.msgType == 0u) {
                
                result[msg.observationNo][msg.stateNo] = msg.val;
            
            }
            if (msg.msgType == 1u) {
                
                active[msg.observationNo][msg.stateNo] = msg.val;
                
            }
            
            if (msg.msgType == 2u) {
                
                activeCnt[msg.stateNo] = msg.val;
            
            }
            if (msg.msgType == 3u) {
                
                r[msg.stateNo] = msg.val;
                
            }
            if (msg.msgType == 4u) {
                
                trans[msg.observationNo][0u] = msg.val;
                trans[msg.observationNo][1u] = msg.stateNo;
                
            }
            //printf("%d %d %d\n", t, p, msg.val);
        
        }
    
    }

    
    for (uint32_t p = 0u; p < NOOFPLACES; p++) {
    
        for (uint32_t t = 0u; t < noOfResults; t++) {

        printf("%d", result[p][t]);
        
        }
        
        printf(" // %d\n", p);
    
    }
    
    printf("\n");
    printf("\n");
    
    for (uint32_t p = 0u; p < NOOFTRANS; p++) {
    
        for (uint32_t t = 0u; t < noOfResults; t++) {

        printf("%d,", active[p][t]);
        
        }
        
        printf(" // %d\n", p);
    
    }
    
    
    //Create a file pointer
    FILE * fp1;
    // open the file for writing
    fp1 = fopen ("places.csv","w");
    
    //Create a file pointer
    FILE * fp2;
    // open the file for writing
    fp2 = fopen ("trans.csv","w");
    
    uint32_t msgCnt = 0u;
    //uint32_t actCnt, trans;
    uint8_t live = true;
    //for (uint32_t t = 0u; t < 40u; t++) {
    while (live) {
        
        hostLink.recvMsg(&msg, sizeof(msg));
            
        if (msg.msgType == 0u) {
            
            result[msg.observationNo][msg.stateNo] = msg.val;
        
        }
        if (msg.msgType == 1u) {
            
            active[msg.observationNo][msg.stateNo] = msg.val;
            
        }
        
        if (msg.msgType == 2u) {
            
            activeCnt[msg.stateNo] = msg.val;
        
        }
        if (msg.msgType == 3u) {
            
            r[msg.stateNo] = msg.val;
            
        }
        if (msg.msgType == 4u) {
            
            //trans[msg.observationNo][0u] = msg.val;
            //trans[msg.observationNo][1u] = msg.stateNo;
            
            printf("Loop No - %d\n", msg.observationNo);
            
            
            if (msg.val == 0u) {
                    
                // Write Places
                
                // Header
                
                fprintf(fp1, " ,");
                
                for (uint32_t p = 0u; p < NOOFPLACES; p++) {
                    fprintf(fp1, "%d,", p);
                }
                
                fprintf(fp1, "\n");
                
                for (uint32_t t = 0u; t < noOfResults; t++) {
                    
                    fprintf(fp1, "%d,", t);

                    for (uint32_t p = 0u; p < NOOFPLACES; p++) {
                        
                        fprintf(fp1, "%d,", result[p][t]);
                    
                    }
                    
                    fprintf(fp1, "\n");
                
                }
                
                // close the file  
                fclose (fp1);
                
                // Write Transitions
                
                fprintf(fp2, "t, ActiveCnt, Trans\n");
                
                for (uint32_t t = 0u; t < noOfResults; t++) {
                
                    fprintf(fp2, "%d, %d, %d\n", t,  trans[t][0u], trans[t][1u]);
                
                }
                
                // close the file  
                fclose (fp2);
                
                live = false;
            }
            
        }
        
    }
    */ 
 
    // Record processing time
    gettimeofday(&finish_proc, NULL);
    timersub(&finish_proc, &start_proc, &diff_proc);
    double proc_duration = (double) diff_proc.tv_sec + (double) diff_proc.tv_usec / 1000000.0;
    printf("Wall Proc Time = %0.5f\n", proc_duration);
    
    return 0;
}
