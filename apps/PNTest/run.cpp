#define TINSEL (1u)
//#define PRINTDEBUG (1u)

#include "model.h"
#include <HostLink.h>
#include <math.h>
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
 * scp jordmorr@byron.cl.cam.ac.uk:~/tinsel/apps/PNTest/results.csv C:\Users\drjor\Documents\tinsel\apps\PNTest
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
     * HARDWARE LAYER SETUP
     * *****************************************************/
    
    for (uint8_t board = 0u; board < (NOOFBOXES * (TinselBoardsPerBox - 1u)); board++) {
    
        for (uint8_t mailbox = 0u; mailbox < TinselMailboxesPerBoard; mailbox++) {
            
            for (uint8_t col = 0u; col < (TinselCoresPerMailbox * NOOFHWCOLSPERCORE); col++) {
                
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
                    
                
                }
                
            }
            
        }
    
    }
    
    /********************************************************
     * HARDWARE ABSTRACTION LAYER SETUP
     * *****************************************************/
    
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

    for (uint32_t x = 0u; x < 10u; x++) {
    hostLink.recvMsg(&msg, sizeof(msg));
    
    printf("Message Value = %e\n", msg.val);
    
    }
    
    // Record processing time
    gettimeofday(&finish_proc, NULL);
    timersub(&finish_proc, &start_proc, &diff_proc);
    double proc_duration = (double) diff_proc.tv_sec + (double) diff_proc.tv_usec / 1000000.0;
    printf("Wall Proc Time = %0.5f\n", proc_duration);
    
    return 0;
}
