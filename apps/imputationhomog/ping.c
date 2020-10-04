#include "imputation.h"

#define PREVA (1u << 0u)
#define NEXTA (1u << 1u)
#define PREVB (1u << 2u)
#define NEXTB (1u << 3u)

/*****************************************************
 * Hidden Markov Model and Linear Interpolation Node
 * ***************************************************
 * This code performs the stephens and li model for imputation
 * 
 * USE ARRAYS FOR MATCH / ALPHAS TO PUSH THE PROCESSING TO MESSAGE TIME IN FAVOUR OF POETS
 * ****************************************************/

int main()
{
    // Get thread id
    int me = tinselId();
    uint8_t localThreadID = me % (1u << TinselLogThreadsPerMailbox);
    
    // Calculate model parameters
    uint8_t HWRowNo = localThreadID % 8u;

    // Received values for hardware layout
    uint32_t* baseAddress = tinselHeapBase();
    
    // Received values for hardware layout
    // -------------------------------------------->
    uint32_t fwdKey = *baseAddress;
    uint32_t bwdKey = *(baseAddress + 1u);
    uint32_t prevThread = *(baseAddress + 2u);
    uint32_t observationNo = *(baseAddress + 3u);
    // <-------------------------------------------
    
    // Received values for each leg
    // -------------------------------------------->
    
    float fwdSame[NOOFLEGS] = {0.0f};
    float fwdDiff[NOOFLEGS] = {0.0f};
    float bwdSame[NOOFLEGS] = {0.0f};
    float bwdDiff[NOOFLEGS] = {0.0f};
    
    
    uint32_t match[NOOFSTATEPANELS][NOOFLEGS] = {{0u}};
    
    // Populate local genetic distances and calculate total genetic distance
    float dmLocal[LINRATIO][NOOFLEGS] = {{0.0f}};
    float totalDistance[NOOFLEGS] = {0.0f};
    
    for (uint32_t x = 0u; x < NOOFLEGS; x++) {
        
        uint32_t legOffset = 4u + (x * (4u + NOOFSTATEPANELS + LINRATIO));
    
        fwdSame[x] = *(float*)(baseAddress + legOffset);
        fwdDiff[x] = *(float*)(baseAddress + legOffset + 1u);
        bwdSame[x] = *(float*)(baseAddress + legOffset + 2u);
        bwdDiff[x] = *(float*)(baseAddress + legOffset + 3u);
        
        for (uint32_t y = 0u; y < NOOFSTATEPANELS; y++) {
        
            match[y][x] = *(baseAddress + legOffset + 4u + y);
        
        }
        
        for (uint32_t y = 0u; y < LINRATIO; y++) {
            
            dmLocal[y][x] = *(float*)(baseAddress + legOffset + 4u + NOOFSTATEPANELS + y);
            totalDistance[x] += dmLocal[y][x];
            
        }
    
    }
    
    // <-------------------------------------------
    
    // Get host id
    int host = tinselHostId();
    
    float alpha[NOOFSTATEPANELS][NOOFLEGS] = {{0.0f}};
    float beta[NOOFSTATEPANELS][NOOFLEGS] = {{0.0f}};
    float alphaLin[NOOFSTATEPANELS][NOOFLEGS][LINRATIO - 1u];
    float betaLin[NOOFSTATEPANELS][NOOFLEGS][LINRATIO - 1u];
    
    float prevAlpha[NOOFSTATEPANELS][NOOFLEGS] = {{0.0f}};
    float nextAlpha[NOOFSTATEPANELS][NOOFLEGS] = {{0.0f}};
    float prevBeta[NOOFSTATEPANELS][NOOFLEGS] = {{0.0f}};
    float nextBeta[NOOFSTATEPANELS][NOOFLEGS] = {{0.0f}};
    
    uint8_t rdyFlags[NOOFSTATEPANELS][NOOFLEGS] = {0u};
    
    
    if (observationNo == 1u) {
        
        // Send to host
        volatile HostMessage* msgHost = tinselSendSlot();
        
        tinselWaitUntil(TINSEL_CAN_SEND);
        msgHost->msgType = FORWARD;
        msgHost->observationNo = observationNo;
        msgHost->stateNo = HWRowNo;
        msgHost->val = totalDistance[0];

        tinselSend(host, msgHost);
    
    }
    
    
    /*
    START HERE TO ADD THE NOOFLEGS DIMENSION INTO THE CODE (ALSO CHECK ABOVE IS CORRECT)
    
    // Startup for forward algorithm
    if (observationNo == 0u) {
        
        for (uint32_t y = 0u; y < NOOFSTATEPANELS; y++) {
            
            // Calculate initial probability
            alpha[y] = 1.0f / NOOFSTATES;
            
            // Multiply alpha by emission probability
            if (match[y] == 1u) {
                alpha[y] = alpha[y] * (1.0f - (1.0f / ERRORRATE));
            }
            else {
                alpha[y] = alpha[y] * (1.0f / ERRORRATE);
            }
            
            prevAlpha[y] = alpha[y];
            rdyFlags[y] |= PREVA;
            
            // Get pointers to mailbox message slot
            volatile ImpMessage* msgOut = tinselSendSlot();
            
            // Prepare message to send to HMM node
            tinselWaitUntil(TINSEL_CAN_SEND);
            msgOut->msgType = FORWARD;
            msgOut->stateNo = (y * NOOFHWROWS) + HWRowNo;
            msgOut->val = alpha[y];
            
            // Propagate to next column
            tinselKeySend(fwdKey, msgOut);
            
            // Send to host
            volatile HostMessage* msgHost = tinselSendSlot();

            tinselWaitUntil(TINSEL_CAN_SEND);
            msgHost->msgType = FORWARD;
            msgHost->observationNo = observationNo * LINRATIO;
            msgHost->stateNo = (y * NOOFHWROWS) + HWRowNo;
            msgHost->val = alpha[y];

            tinselSend(host, msgHost);
        
        }
    
    }
    
    // Startup for backward algorithm
    if (observationNo == (NOOFTARGMARK - 1u)) {
        
        for (uint32_t y = 0u; y < NOOFSTATEPANELS; y++) {
      
            // Get pointers to mailbox message slot
            volatile ImpMessage* msgOut = tinselSendSlot();
            
            beta[y] = 1.0f;
            
            tinselWaitUntil(TINSEL_CAN_SEND);
            msgOut->msgType = BACKWARD;
            msgOut->match = match[y];
            msgOut->stateNo = (y * NOOFHWROWS) + HWRowNo;
            msgOut->val = beta[y];
            
            // Propagate to previous column
            tinselKeySend(bwdKey, msgOut);
            
            volatile HostMessage* msgHost = tinselSendSlot();

            tinselWaitUntil(TINSEL_CAN_SEND);
            msgHost->msgType = BACKWARD;
            msgHost->observationNo = observationNo * LINRATIO;
            msgHost->stateNo = (y * NOOFHWROWS) + HWRowNo;
            msgHost->val = beta[y];

            tinselSend(host, msgHost);
            
            prevBeta[y] = beta[y];
            rdyFlags[y] |= PREVB;
            
            // Propagate beta to previous thread as prev beta
            
            tinselWaitUntil(TINSEL_CAN_SEND);
            msgOut->msgType = BWDLIN;
            msgOut->match = match[y];
            msgOut->stateNo = (y * NOOFHWROWS) + HWRowNo;
            msgOut->val = beta[y];
            
            // Propagate to previous column
            tinselSend(prevThread, msgOut);
        
        }
        
    }
    
        
    uint8_t fwdRecCnt = 0u;
    uint8_t bwdRecCnt = 0u;
    
    while (1u) {
        tinselWaitUntil(TINSEL_CAN_RECV);
        volatile ImpMessage* msgIn = tinselRecv();
        
        uint32_t index = (uint32_t)(msgIn->stateNo / NOOFHWROWS);
        
        // Handle forward messages
        if (msgIn->msgType == FORWARD) {
            
            fwdRecCnt++;
            
            for (uint32_t y = 0u; y < NOOFSTATEPANELS; y++) {
            
                if (msgIn->stateNo == ((y * NOOFHWROWS) + HWRowNo)) {
                    alpha[y] += msgIn->val * fwdSame;
                }
                else {
                    alpha[y] += msgIn->val * fwdDiff;
                }
                
                if (fwdRecCnt == NOOFSTATES) {
                    
                    // Multiply Alpha by Emission Probability
                    if (match[y] == 1u) {
                        alpha[y] = alpha[y] * (1.0f - (1.0f / ERRORRATE));
                    }
                    else {
                        alpha[y] = alpha[y] * (1.0f / ERRORRATE);
                    }
                    
                    // Previous alpha has been calculated
                    prevAlpha[y] = alpha[y];
                    rdyFlags[y] |= PREVA;
                    
                    // If we are an intermediate node propagate the alpha to the next column
                    // Else send the alpha out to the host
                    if (observationNo != (NOOFTARGMARK - 1u)) {
                        
                        // Get pointers to mailbox message slot
                        volatile ImpMessage* msgOut = tinselSendSlot();
                        
                        tinselWaitUntil(TINSEL_CAN_SEND);
                        msgOut->msgType = FORWARD;
                        msgOut->stateNo = (y * NOOFHWROWS) + HWRowNo;
                        msgOut->val = alpha[y];
                        
                        tinselKeySend(fwdKey, msgOut);
                        
                    }
                    
                    // Propagte alpha to previous thread as next alpha
                    
                    // Get pointers to mailbox message slot
                    volatile ImpMessage* msgOut = tinselSendSlot();
                    
                    tinselWaitUntil(TINSEL_CAN_SEND);
                    msgOut->msgType = FWDLIN;
                    msgOut->stateNo = (y * NOOFHWROWS) + HWRowNo;
                    msgOut->val = alpha[y];
                    
                    tinselSend(prevThread, msgOut);
                    
                    // Send to host
                    volatile HostMessage* msgHost = tinselSendSlot();

                    tinselWaitUntil(TINSEL_CAN_SEND);
                    msgHost->msgType = FORWARD;
                    msgHost->observationNo = observationNo * LINRATIO;
                    msgHost->stateNo = (y * NOOFHWROWS) + HWRowNo;
                    msgHost->val = alpha[y];

                    tinselSend(host, msgHost);
                
                }
            
            }
        
        }
        
        // Handle backward messages
        if (msgIn->msgType == BACKWARD) {
            
            bwdRecCnt++;
            
            float emissionProb = 0.0f;
         
            if (msgIn->match == 1u) {
                emissionProb = (1.0f - (1.0f / ERRORRATE));
            }
            else {
                emissionProb = (1.0f / ERRORRATE);
            }
            
            for (uint32_t y = 0u; y < NOOFSTATEPANELS; y++) {
            
                if (msgIn->stateNo == ((y * NOOFHWROWS) + HWRowNo)) {
                    
                    beta[y] += msgIn->val * bwdSame * emissionProb;
                    
                }
                else {
                    
                    beta[y] += msgIn->val * bwdDiff * emissionProb;
                    
                }
                
                if (bwdRecCnt == NOOFSTATES) {
                    
                    prevBeta[y] = beta[y];
                    rdyFlags[y] |= PREVB;
                    
                    if (observationNo != 0u) {
                        
                        // Get pointers to mailbox message slot
                        volatile ImpMessage* msgOut = tinselSendSlot();

                        tinselWaitUntil(TINSEL_CAN_SEND);
                        msgOut->msgType = BACKWARD;
                        msgOut->match = match[y];
                        msgOut->stateNo = (y * NOOFHWROWS) + HWRowNo;
                        msgOut->val = beta[y];
                        
                        // Propagate to previous column
                        tinselKeySend(bwdKey, msgOut);
                        
                        // Propagate beta to previous thread as prev beta
                        
                        tinselWaitUntil(TINSEL_CAN_SEND);
                        msgOut->msgType = BWDLIN;
                        msgOut->match = match[y];
                        msgOut->stateNo = (y * NOOFHWROWS) + HWRowNo;
                        msgOut->val = beta[y];
                        
                        // Propagate to previous column
                        tinselSend(prevThread, msgOut);

                    }

                    volatile HostMessage* msgHost = tinselSendSlot();;

                    tinselWaitUntil(TINSEL_CAN_SEND);
                    msgHost->msgType = BACKWARD;
                    msgHost->observationNo = observationNo * LINRATIO;
                    msgHost->stateNo = (y * NOOFHWROWS) + HWRowNo;
                    msgHost->val = beta[y];

                    tinselSend(host, msgHost);
                
                }
            
            }
        }
        
        // Handle forward messages
        if (msgIn->msgType == FWDLIN) {
            
            nextAlpha[index] = msgIn->val;
            rdyFlags[index] |= NEXTA;
            
        }
        
        // If we have both values for linear interpolation
        if ((rdyFlags[index] & PREVA) && (rdyFlags[index] & NEXTA)) {
            
            float totalDiff = prevAlpha[index] - nextAlpha[index];
            
            for (uint32_t x = 0u; x < (LINRATIO - 1u); x++) {
                
                alphaLin[index][x] = prevAlpha[index] - ((dmLocal[x] / totalDistance) * totalDiff);
                prevAlpha[index] = alphaLin[index][x];

            }
            
            for (uint32_t x = 0u; x < (LINRATIO - 1u); x++) {
                
                // Send to host
                volatile HostMessage* msgHost = tinselSendSlot();

                tinselWaitUntil(TINSEL_CAN_SEND);
                msgHost->msgType = FWDLIN;
                msgHost->observationNo = (observationNo * LINRATIO) + 1u + x;
                msgHost->stateNo = (index * NOOFHWROWS) + HWRowNo;
                msgHost->val = alphaLin[index][x];

                tinselSend(host, msgHost);
                
            }
            
            // Clear the ready flags to prevent re-transmission
            rdyFlags[index] &= (~PREVA);
            rdyFlags[index] &= (~NEXTA);
            
        }
        
        // Handle backward messages
        if (msgIn->msgType == BWDLIN) {
        
            nextBeta[index] = msgIn->val;
            rdyFlags[index] |= NEXTB;

        }
        
        
        for (uint32_t y = 0u; y < NOOFSTATEPANELS; y++) {
            
            // JPM ADD CHECK WHETHER MSGTYPE = BACKWARD TO PREVENT MULTIPLE CHECKS THROUGH THIS CODE
            // If we have received both values for linear interpolation
            if ((rdyFlags[y] & PREVB) && (rdyFlags[y] & NEXTB)) {
                
                float totalDiff = nextBeta[y] - prevBeta[y];
                
                for (uint32_t x = 0u; x < (LINRATIO - 1u); x++) {
                    
                    betaLin[y][x] = nextBeta[y] - ((dmLocal[(LINRATIO - 2u) - x] / totalDistance) * totalDiff);
                    nextBeta[y] = betaLin[y][x];
                    
                }
                
                for (uint32_t x = 0u; x < (LINRATIO - 1u); x++) {
                    
                    // Send to host
                    volatile HostMessage* msgHost = tinselSendSlot();

                    tinselWaitUntil(TINSEL_CAN_SEND);
                    msgHost->msgType = BWDLIN;
                    msgHost->observationNo = (observationNo * LINRATIO) + 1u + x;
                    msgHost->stateNo = (y * NOOFHWROWS) + HWRowNo;
                    msgHost->val = betaLin[y][(LINRATIO - 2u) - x];

                    tinselSend(host, msgHost);
                    
                }
                
                // Clear the ready flags to prevent re-transmission
                rdyFlags[y] &= (~PREVB);
                rdyFlags[y] &= (~NEXTB);
                
            }
        
        }
        
        // Free message slot
        tinselFree(msgIn);
    
    }*/
    return 0;
}

