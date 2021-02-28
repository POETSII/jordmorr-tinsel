#include <stdbool.h>

#include "imputation.h"

/*****************************************************
 * Hidden Markov Model and Linear Interpolation Node
 * ***************************************************
 * This code performs the stephens and li model for imputation
 * 
 * USE ARRAYS FOR MATCH / ALPHAS TO PUSH THE PROCESSING TO MESSAGE TIME IN FAVOUR OF POETS
 * ****************************************************/
 
/*****************************************************
* Pre-processor Definitions
* ***************************************************/
 
// Netlist Transition Elements
#define PIPELINE (0u)
#define SYNCHRONISER (1u)
#define GENERATOR (2u)
#define SINK (3u)
#define PORTOUT (4u)

// Message Directions
#define LEFT (0u)
#define RIGHT (1u)

/*****************************************************
* Defined Built-In Functions
* ***************************************************/

void *memset(void *s, int c, int n) {
  char *p = (char*) s;
  for (int i = 0; i < n; i++) p[i] = c;
  return s;
}

/*****************************************************
* Function Prototypes
* ***************************************************/

// Petri Net Elements
bool check_pipeline(uint32_t* in0, uint32_t* in1);
void run_pipeline(uint32_t* in0, uint32_t* in1, uint32_t* out0, uint32_t* out1);
bool check_synchroniser(uint32_t* in0, uint32_t* in1, uint32_t* in2);
void run_synchroniser(uint32_t* in0, uint32_t* in1, uint32_t* in2, uint32_t* out0, uint32_t* out1, uint32_t* out2);
void run_token_generator(uint32_t* place);
void run_token_sink(uint32_t* place);
void run_output_port(uint32_t *srcPlace, uint32_t destPlace, uint32_t direction, uint32_t prevThread, uint32_t nextThread);
void run_input_port(uint32_t* place, uint32_t tokens);

//Support Functions
uint16_t randomNum(void);

/*****************************************************
* Global Variables
* ***************************************************/



/*****************************************************
* Main Function
* ***************************************************/

int main()
{
    
    // Get thread id
    int me = tinselId();
    uint8_t localThreadID = me % (1u << TinselLogThreadsPerMailbox);
    
    // Get host id
    int host;
    host = tinselHostId();
    
    // Calculate model parameters
    uint32_t HWRowNo = localThreadID % 8u;

    // Received values for hardware layout
    uint32_t* baseAddress = tinselHeapBase();
    
    // Received values for hardware layout
    // -------------------------------------------->
    uint32_t HWColNo = *baseAddress;
    uint32_t prevThread = *(baseAddress + 1u);
    uint32_t nextThread = *(baseAddress + 2u);
    // <-------------------------------------------
    
    
    
    // Received values for petri net
    // -------------------------------------------->
    
    // Declare Netlist
    
    uint32_t netlist[NOOFELEMENTS][7u];
    uint32_t ptrOffset = 0u;
    
    // Populate Netlist
    
    for (uint32_t element = 0u; element < NOOFELEMENTS; element++) {
                        
        for (uint32_t node = 0u; node < 7u; node++) {
            
            netlist[element][node] = *(baseAddress + 3u + ptrOffset);
            
            ptrOffset++;
            
        }
        
    }
    
    // Declare Places
    
    uint32_t place[NOOFPLACES];
    
    // Populate Initial Marking
    
    for (uint32_t p = 0u; p < NOOFPLACES; p++) {
        
        place[p] = *(baseAddress + 3u + ptrOffset);
        
        ptrOffset++;
   
    }
    
    // List of active transitions
    
    uint32_t activeList[NOOFTRANS] = {0u};
    
    
    // List of token generators
    
    uint32_t tokenGenerator[NOOFANCILLIARY] = {0u};
    uint32_t generatorCnt = 0u;
    
    for (uint32_t element = NOOFTRANS; element < NOOFELEMENTS; element++) {
        
        if (netlist[element][0u] == GENERATOR) {
            
            tokenGenerator[generatorCnt] = netlist[element][1u];
            generatorCnt++;
            
        }
        
    }
    
    // List of token sinks
    
    uint32_t tokenSink[NOOFANCILLIARY] = {0u};
    uint32_t sinkCnt = 0u;
    
    // If only included for proof of concept. Needs removing for a general implementation
    if (HWColNo == 0u) {
    
        for (uint32_t element = NOOFTRANS; element < NOOFELEMENTS; element++) {
            
            if (netlist[element][0u] == SINK) {
                
                tokenSink[sinkCnt] = netlist[element][1u];
                sinkCnt++;
                
            }
            
        }
    
    }
    
    // List of output ports
    
    uint32_t outputPort[NOOFANCILLIARY] = {0u};
    uint32_t outPortCnt = 0u;
    
    for (uint32_t element = NOOFTRANS; element < NOOFELEMENTS; element++) {
        
        if (netlist[element][0u] == PORTOUT) {
            
            outputPort[outPortCnt] = element;
            outPortCnt++;
            
        }
        
    }
    
    // Start cycle counter for PRNG and node timing
    tinselPerfCountReset();
    tinselPerfCountStart();
    
    // Transition Counter
    uint32_t t = 0u;
    
    // ----> JPM for proof of concept only
    if (HWColNo == (NOOFREQCOLS - 1u)) {
        place[111] = 1u;
    }
    // <---- JPM for proof of concept only
    /***************************************************
    * MAIN NODE HANDLER LOOP
    ****************************************************/
        
    while (1u) {
        
        //Count sends to be made
        
        uint32_t sendCnt = 0u;
        
        if (outPortCnt > 0u) {
            for (uint32_t port = 0u; port < outPortCnt; port++) {
                // Is there a token to be sent?
                if (place[netlist[outputPort[port]][1u]]) {
                    sendCnt++;
                }
            }
        }    
                    

        // Send/Receive Event Loop
        
        while (1u) {
        
            // Attempt to make sends that are waiting, the network capacity permitting
            
            if (outPortCnt > 0u) {
            
                for (uint32_t port = 0u; port < outPortCnt; port++) {
                    
                    // Is there a token to be sent?
                    if (place[netlist[outputPort[port]][1u]]) {
                        
                        // Attempt to make the send
                        
                        if (tinselCanSend()) {
                            
                            run_output_port(&place[netlist[outputPort[port]][1u]], netlist[outputPort[port]][2u], netlist[outputPort[port]][3u], prevThread, nextThread);
                            
                            sendCnt--;
                            
                            /*
                            // Send to host
                            volatile HostMessage* msgHost = tinselSendSlot();

                            tinselWaitUntil(TINSEL_CAN_SEND);
                            msgHost->msgType = netlist[outputPort[port]][1u];
                            msgHost->observationNo = netlist[outputPort[port]][2u];
                            msgHost->stateNo = HWRowNo;
                            msgHost->val = HWColNo;

                            tinselSend(host, msgHost);
                            */
                        }
                        
                    }
                    
                }
            
            }
            
            
            int idle = tinselIdle(0u);

            if (idle && (sendCnt == 0u)) {
                if (HWColNo < NOOFREQCOLS) { 
                    break;
                }
                    
            }
            else {
                
                while (tinselCanRecv()) {
                    
                    volatile ImpMessage* msgIn = tinselRecv();
                    
                    run_input_port(&place[msgIn->destPlace], msgIn->tokenCnt);
                    
                    //msgReceived = true;
                    
                    // Free message slot
                    tinselFree(msgIn);
                    
                }
            
            }
        
        }
        
        /*
        if (generatorCnt > 0u) {
        
            for (uint32_t generator = 0u; generator < generatorCnt; generator++) {
                
                run_token_generator(&place[tokenGenerator[generator]]);
                
            }
        
        }
        */
        
        /* ----> JPM DEBUG
        
        // Send places to host
        if ((HWColNo == 1u) && (HWRowNo == 0u)) {
            
            for (uint32_t p = 0u; p < NOOFPLACES; p++) {
            
                // Send to host
                volatile HostMessage* msgHost = tinselSendSlot();

                tinselWaitUntil(TINSEL_CAN_SEND);
                msgHost->msgType = 0u;
                msgHost->observationNo = p;
                msgHost->stateNo = t;
                msgHost->val = place[p];
                

                tinselSend(host, msgHost);
            
            }
            
            //t++;
            
        }
        */
        
        
        // Check for active transitions
        
        uint32_t activeCnt = 0u;
        
        for (uint32_t element = 0u; element < NOOFTRANS; element++) {
            
            bool active = false;
            
            switch(netlist[element][0u])
            {
                
                case PIPELINE:
                
                    active = check_pipeline(&place[netlist[element][1u]], &place[netlist[element][2u]]);
                
                    break;
                
                case SYNCHRONISER:
                
                    active = check_synchroniser(&place[netlist[element][1u]], &place[netlist[element][2u]], &place[netlist[element][3u]]);
                
                    break;
                
            }
            
            if (active) {
                
                activeList[activeCnt] = element;
                
                activeCnt++;
                
            }
            
        }
        
        /* ----> JPM DEBUG
        if (activeCnt == 0u) {
        
            if ((HWColNo == 1u) && (HWRowNo == 0u)) {
            
                // Send to host
                volatile HostMessage* msgHost = tinselSendSlot();

                tinselWaitUntil(TINSEL_CAN_SEND);
                msgHost->msgType = 4u;
                msgHost->observationNo = t;
                msgHost->stateNo = 0u;
                msgHost->val = 0u;
                

                tinselSend(host, msgHost);
            
            }
            
        }
        */
        
        // Randomly Choose and Trigger a Single Active Transition (If One Exists)
        
        if (activeCnt > 0u) {
            
            int randNo = randomNum();
            uint32_t r = 0u;
            
            // Calculate Modulus 
            while (activeCnt <= randNo) {
                r = randNo - activeCnt;
                randNo -= activeCnt;
            }
            
            /* ----> JPM DEBUG
            if ((HWColNo == 1u) && (HWRowNo == 0u)) {
            
                // Send to host
                volatile HostMessage* msgHost = tinselSendSlot();

                tinselWaitUntil(TINSEL_CAN_SEND);
                msgHost->msgType = 4u;
                msgHost->observationNo = t;
                msgHost->stateNo = activeList[r];
                msgHost->val = activeCnt;
                

                tinselSend(host, msgHost);
                
                t++;
            
            }
            */ 
                
            switch(netlist[activeList[r]][0u])
            {
                
                case PIPELINE:
                
                    run_pipeline(&place[netlist[activeList[r]][1u]], &place[netlist[activeList[r]][2u]], &place[netlist[activeList[r]][3u]], &place[netlist[activeList[r]][4u]]);
                
                    break;
                
                case SYNCHRONISER:
                
                    run_synchroniser(&place[netlist[activeList[r]][1u]], &place[netlist[activeList[r]][2u]], &place[netlist[activeList[r]][3u]], &place[netlist[activeList[r]][4u]], &place[netlist[activeList[r]][5u]], &place[netlist[activeList[r]][6u]]);
                
                    break;
                
            }
            
        }
        
        /*
        if (sinkCnt > 0u) {
        
            for (uint32_t sink = 0u; sink < sinkCnt; sink++) {
                
                if (place[tokenSink[sink]] > 0u) {
                
                    run_token_sink(&place[tokenSink[sink]]);
                
                }
                
            }
        
        }
        */
        
        // For the proof of concept ----> JPM
        
        if (HWColNo == 0u) {
            
            if (place[1]) {
                run_token_sink(&place[1]);
                run_token_generator(&place[0]);
            }
            
        }
        if (HWColNo == (NOOFREQCOLS - 1u)) {
            
            if (place[110]) {
                run_token_sink(&place[110]);
                run_token_generator(&place[111]);
            }
            
        }
        
        // <---- JPM
        
        // Update Transition Counter
        t++;
        
        // Check whether number of transitions have been reached
        if ((HWColNo == 0u) && (HWRowNo == 0u) && (t == NOOFREQITER)) {
        
            // Get cycle counts
            uint32_t countLower = tinselCycleCount();
            uint32_t countUpper = tinselCycleCountU();
            
            // Send to host
            volatile HostMessage* msgHost = tinselSendSlot();

            tinselWaitUntil(TINSEL_CAN_SEND);
            msgHost->msgType = 0u;
            msgHost->observationNo = countLower;
            msgHost->stateNo = countUpper;

            tinselSend(host, msgHost);
        }
        
    }
    // Should never reach here
    return 0;
}


/***************************************************
* Petri Net Transition Element Library
****************************************************/

// Pipline Element

bool check_pipeline(uint32_t* in0, uint32_t* in1) {
    
    if (*in0 && *in1) {
        return true;
    }
    else {
        return false;
    }
    
}

void run_pipeline(uint32_t* in0, uint32_t* in1, uint32_t* out0, uint32_t* out1) {
    
    *in0 -= 1u;
    *in1 -= 1u;
    *out0 += 1u;
    *out1 += 1u;
    
    return;
    
}

// Syncroniser Element

bool check_synchroniser(uint32_t* in0, uint32_t* in1, uint32_t* in2) {
    
    if (*in0 && *in1 && *in2) {
        return true;
    }
    else {
        return false;
    }
    
}

void run_synchroniser(uint32_t* in0, uint32_t* in1, uint32_t* in2, uint32_t* out0, uint32_t* out1, uint32_t* out2) {
    
    *in0 -= 1u;
    *in1 -= 1u;
    *in2 -= 1u;
    *out0 += 1u;
    *out1 += 1u;
    *out2 += 1u;
    
    return;
    
}

/***************************************************
* Petri Net Ancilliary Element Library
****************************************************/

// Token Generator

void run_token_generator(uint32_t* place) {
    
    *place = 1u;
    
    return;
    
}

// Token Sink

void run_token_sink(uint32_t* place) {
    
    *place = 0u;
    
    return;
}

// Output Port

void run_output_port(uint32_t *srcPlace, uint32_t destPlace, uint32_t direction, uint32_t prevThread, uint32_t nextThread) {
    
    *srcPlace -= 1u;
    
    // Get pointers to mailbox message slot
    volatile ImpMessage* msgOut = tinselSendSlot();
        
    tinselWaitUntil(TINSEL_CAN_SEND);
    msgOut->destPlace = destPlace;
    msgOut->tokenCnt = 1u;
    
    // Select Direction
    
    switch(direction)
    {
        
        case LEFT:
        
            // Propagate to left tile
            tinselSend(prevThread, msgOut);
        
            break;
        
        case RIGHT:
        
            // Propagate to left tile
            tinselSend(nextThread, msgOut);
            
            break;
        
    }
    
    /*
    int host = tinselHostId();
    
    // Send to host
    volatile HostMessage* msgHost = tinselSendSlot();

    tinselWaitUntil(TINSEL_CAN_SEND);
    msgHost->msgType = 0u;
    msgHost->observationNo = direction;
    msgHost->stateNo = HWRowNo;
    msgHost->val = HWColNo;

    tinselSend(host, msgHost);
    */
}

// Input Port

void run_input_port(uint32_t* place, uint32_t tokens) {
    
    *place += tokens;
    
    return;
    
}

/***************************************************
* Support Functions
****************************************************/

// Simple LFSR-based Pseudo-Random Number Generator

uint16_t randomNum(void) {
    
    uint16_t lfsr = tinselCycleCount() & 0xFFFF;
    uint16_t bit;
    
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return ((lfsr >> 1) | (bit << 15));
}