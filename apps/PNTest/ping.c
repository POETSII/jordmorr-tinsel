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
 

//GLOBALS

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
    
    
    
    // Received values for hardware abstraction
    // -------------------------------------------->
/*    
    for (uint32_t x = 0u; x < NOOFLEGS; x++) {
        
        uint32_t legOffset = 4u + (x * (4u + NOOFSTATEPANELS + LINRATIO));
    
        fwdSame[x] = *(float*)(baseAddress + legOffset);
        fwdDiff[x] = *(float*)(baseAddress + legOffset + 1u);
        bwdSame[x] = *(float*)(baseAddress + legOffset + 2u);
        bwdDiff[x] = *(float*)(baseAddress + legOffset + 3u);
        
        for (uint32_t y = 0u; y < NOOFSTATEPANELS; y++) {
        
            match[y][x] = (uint16_t)*(baseAddress + legOffset + 4u + y);
        
        }
        
        totalDistance[x] = 0.0f;
        
        for (uint32_t y = 0u; y < LINRATIO; y++) {
            
            dmLocal[y][x] = *(float*)(baseAddress + legOffset + 4u + NOOFSTATEPANELS + y);
            totalDistance[x] += dmLocal[y][x];
            
        }
    
    }
    // <-------------------------------------------
    
    // Declared and intialised seperately to avoid memset error on compilation
    
    for (uint32_t statePanel = 0u; statePanel < NOOFSTATEPANELS; statePanel++) {
        for (uint32_t leg = 0u; leg < NOOFLEGS; leg++) {
            
            alpha[statePanel][leg] = 0.0f;
            beta[statePanel][leg] = 0.0f;
            
            prevAlpha[statePanel][leg] = 0.0f;
            nextAlpha[statePanel][leg] = 0.0f;
            prevBeta[statePanel][leg] = 0.0f;
            nextBeta[statePanel][leg] = 0.0f;
            
            rdyFlags[statePanel][leg] = 0u;
            
        }
    }
    
    for (uint32_t statePanel = 0u; statePanel < NOOFSTATEPANELS; statePanel++) {
        for (uint32_t leg = 0u; leg < NOOFLEGS; leg++) {
            for (uint32_t linVal = 0u; linVal < (LINRATIO - 1u); linVal++) {
                
                alphaLin[statePanel][leg][linVal] = 0.0f;
                betaLin[statePanel][leg][linVal] = 0.0f;
                
            }
        }
    }
    
    for (uint32_t leg = 0u; leg < NOOFLEGS; leg++) {
        
        fwdRecCnt[leg] = 0u;
        bwdRecCnt[leg] = 0u;
    
    }
    
    targHaplotype = 0u;
    waveCnt = 0u;
    
    toBeSentFlags = 0u;
    toBeSentNextFlags = 0u;
    sentFlags = 0u;
*/    
    // Startup for forward algorithm
    if (HWColNo == (NOOFHWCOLS - 1u)) {
        
        
        // Initiate message chain
        volatile ImpMessage* msgOut = tinselSendSlot();
        
        tinselWaitUntil(TINSEL_CAN_SEND);
        msgOut->msgType = 0u;
        msgOut->match = 0u;
        msgOut->leg = 0u;
        msgOut->stateNo = 0u;
        msgOut->val = 0.0f;

        // Propagate to previous column
        tinselSend(prevThread, msgOut);
        
        
        
        /*
        // If we are also the first hardware row (Core 0, Thread 0) start the performance counter
        if (HWRowNo == 0u) {
            
            tinselPerfCountReset();
            tinselPerfCountStart();
            
        }
        */
    }
    
    /***************************************************
    * MAIN NODE HANDLER LOOP
    ****************************************************/
    
    while (1u) {
            
        tinselWaitUntil(TINSEL_CAN_RECV);
            
        volatile ImpMessage* msgIn = tinselRecv();
        
        if (HWColNo != 0u) {
        
            // Send to next thread    
                
            // Get pointers to mailbox message slot
            volatile ImpMessage* msgOut = tinselSendSlot();
                
            tinselWaitUntil(TINSEL_CAN_SEND);
            msgOut->msgType = 0u;
            msgOut->match = 0u;
            msgOut->leg = 0u;
            msgOut->stateNo = 0u;
            msgOut->val = msgIn->val + 1.0f;
            
            // Propagate to previous column
            tinselSend(prevThread, msgOut);
        
        }
        else {
            
            // Send to host
            volatile HostMessage* msgHost = tinselSendSlot();

            tinselWaitUntil(TINSEL_CAN_SEND);
            msgHost->msgType = 0u;
            msgHost->observationNo = 0u;
            msgHost->stateNo = 0u;
            msgHost->val = msgIn->val;

            tinselSend(host, msgHost);
        }
            
    }
    // Should never reach here
    return 0;
}
