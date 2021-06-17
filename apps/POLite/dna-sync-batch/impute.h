// SPDX-License-Identifier: BSD-2-Clause
#ifndef _IMPUTE_H_
#define _IMPUTE_H_

//#define POLITE_DUMP_STATS
//#define POLITE_COUNT_MSGS
#include "myPOLite.h"
#include "params.h"

// ImpMessage types
const uint32_t FORWARD = 0;
const uint32_t BACKWARD =  1;
const uint32_t ACCUMULATE = 2;
const uint32_t LINEARINTERP = 3;

// Flags

const uint32_t ALPHA = (1 << 0); //1
const uint32_t BETA = (1 << 1); //2
const uint32_t ACCA = (1 << 2); //4
const uint32_t ALPHAPOST = (1 << 3); //8
const uint32_t BETAPOST = (1 << 4); //16
const uint32_t ALLELECNTS = (1 << 5); //32
const uint32_t ALPHALIN = (1 << 6); //64
const uint32_t BETALIN = (1 << 7); //128

struct ImpMessage {
    
    // message type
    uint32_t msgtype;
    // state number
    uint32_t stateNo;
    // match
    uint32_t match;
    // message value
    float val;
    
};

struct ImpState {
    
    // Device id
    uint32_t id; //
    // Message Counters
    uint32_t fwdRecCnt, bwdRecCnt, accaCnt[LINRATIO]; //
    // Mesh Coordinates
    uint32_t x, y; //
    // Node Label
    uint32_t label; //
    // Match
    uint32_t match; //
    // Ready Flags
    uint32_t rdyFlags; //
    // Next Ready Flags
    uint32_t nxtRdyFlags; //
    // Sent Flags
    uint32_t sentFlags; //
    // Target Haplotype Counter
    uint32_t targCnt; //
    // Alpha Counter
    uint32_t alphaCnt; //
    // Beta Counter
    uint32_t betaCnt; //
    // Current Index
    uint32_t currentIndex; //
    // Old Index
    uint32_t oldIndex; //
    // Old Index
    uint32_t alphaTag; //
    // Accumulator Count
    uint32_t accSendCnt; //
    // Node Alphas
    float alpha; //
    // Node Betas
    float beta; //
    // Major Posterior Probability
    float posterior[LINRATIO][NOOFHAPS]; //
    // Local Genetic Distances
    float dmLocal[LINRATIO];
    // Major Posterior Probability
    float majPosterior[LINRATIO]; //
    // Minor Posterior Probability
    float minPosterior[LINRATIO]; //
    // Node Betas
    float fwdSame; //
    // Node Betas
    float fwdDiff; //
    // Node Betas
    float bwdSame; //
    // Node Betas
    float bwdDiff; //
    // Node Old Alphas
    float oldAlpha; //
    // Node Old Betas
    float oldBeta; //
    // Node Old Alphas
    float oldOldAlpha; //
    // Node Old Betas
    float oldOldBeta; //
    // Total genetic distance
    float totalDistance;
    // Alpha Linear Interpolation Values
    float alphaLin[LINRATIO - 1u];
    // Beta Linear Interpolation Values
    float betaLin[LINRATIO - 1u];
    // Beta Linear Interpolation Storage
    float nextBeta;

    
#ifdef IMPDEBUG
        // Sent Counter
        uint32_t sentCnt; //
#endif
    
};

struct ImpDevice : PDevice<ImpState, None, ImpMessage> {

    // Called once by POLite at start of execution
    inline void init() {
        
        s->fwdRecCnt = 0u;
        s->bwdRecCnt = 0u;
        s->alphaCnt = 0u;
        s->betaCnt = 0u;
        s->targCnt = 0u;
        
        s->rdyFlags = 0u;
        s->nxtRdyFlags = 0u;
        s->sentFlags = 0u;
        s->alphaTag = 0u;
        
        s->alpha = 0.0f;
        s->beta = 0.0f;
        s->oldAlpha = 0.0f;
        s->oldBeta = 0.0f;
        
        s->currentIndex = 0u;
        
        s->totalDistance = 0.0f;
        
        for (uint32_t x = 0u; x < LINRATIO; x++) {
            
            // Calculate Total Distance
            s->totalDistance += s->dmLocal[x];
            
            // Clear maj/min posteriors
            s->majPosterior[x] = 0.0f;
            s->minPosterior[x] = 0.0f;
            
            // Clear accumulator message counts
            s->accaCnt[x] = 0u;
            
        }
        
#ifdef IMPDEBUG
        s->sentCnt = 0u;
#endif
        
        *readyToSend = No;
        
    }

    // Send handler
    inline void send(volatile ImpMessage* msg) {
        
        if (*readyToSend == Pin(FORWARD)) {
            
            msg->msgtype = FORWARD;
            msg->stateNo = s->y;
            msg->val = s->oldAlpha;
            
            s->rdyFlags &= (~ALPHA);
            s->sentFlags |= ALPHA;
    
        }
        
        if (*readyToSend == Pin(BACKWARD)) {
            
            msg->msgtype = BACKWARD;
            msg->stateNo = s->y;
            msg->match = s->match;
            msg->val = s->oldBeta;
            
            s->rdyFlags &= (~BETA);
            s->sentFlags |= BETA;

        }
        
        if (*readyToSend == Pin(ACCUMULATE)) {
            
            msg->msgtype = ACCUMULATE;
            msg->stateNo = s->oldIndex;            
            msg->match = (s->accSendCnt << 16u) + s->label;
            msg->val = s->posterior[s->accSendCnt][s->oldIndex];
            
            s->accSendCnt++;
            
            if (s->accSendCnt >= 10u) {
                s->rdyFlags &= (~ACCA);
                s->sentFlags |= ACCA;
            }
            
        }
        
        if (*readyToSend == Pin(LINEARINTERP)) {
            
            if (s->rdyFlags & ALPHALIN) {
                msg->msgtype = LINEARINTERP;
                msg->stateNo = ALPHA;
                msg->val = s->oldAlpha;
                
                s->rdyFlags &= (~ALPHALIN);
                s->sentFlags |= ALPHALIN;
        
            }
            else if (s->rdyFlags & BETALIN) {
                msg->msgtype = LINEARINTERP;
                msg->stateNo = BETA;
                msg->val = s->oldBeta;
                
                s->rdyFlags &= (~BETALIN);
                s->sentFlags |= BETALIN;
                
            }
            
        }
        
        if ((s->rdyFlags & ALPHA) && !(s->sentFlags & ALPHA)) {
            // Have We calculated alpha but not sent it?
            
            *readyToSend = Pin(FORWARD);
            
        }
        else if ((s->rdyFlags & BETA) && !(s->sentFlags & BETA)) {
            // Have We calculated beta but not sent it?
            
            *readyToSend = Pin(BACKWARD);
            
        }
        else if ((s->rdyFlags & ACCA) && !(s->sentFlags & ACCA)) {
            // Have We calculated alpha but not sent it?
            
            *readyToSend = Pin(ACCUMULATE);
            
        }
        else if ((s->rdyFlags & ALPHALIN) && !(s->sentFlags & ALPHALIN)) {
            // Have We calculated alpha but not sent the linear interpolation message?
            
            *readyToSend = Pin(LINEARINTERP);
            
        }
        else if ((s->rdyFlags & BETALIN) && !(s->sentFlags & BETALIN)) {
            // Have We calculated alpha but not sent the linear interpolation message?
            
            *readyToSend = Pin(LINEARINTERP);
            
        }
        else {
            *readyToSend = No;
        }
        
    }

    // Receive handler
    inline void recv(ImpMessage* msg, None* edge) {
        
        // Received Alpha Inductive Message (Forward Algo)
        if (msg->msgtype == FORWARD) {

            if (msg->stateNo == s->y) {
                
                s->alpha += msg->val * s->fwdSame;
             
            }
            else {
                
                s->alpha += msg->val * s->fwdDiff;
                
            }
            
            s->fwdRecCnt++;
            
            // Has the summation of the alpha been calculated?
            if (s->fwdRecCnt == NOOFSTATES) {
                
                // Calculate the final alpha
                if (s->match != 2u) {
                    if (s->match == 1u) {
                        s->alpha = s->alpha * (1.0f - (1.0f / ERRORRATE));
                    }
                    else {
                        s->alpha = s->alpha * (1.0f / ERRORRATE);
                    }
                }
                
                // Increase number of alphas calculated
                s->alphaCnt++;
                // flag that alpha has been updated in cycle (used for linear interp posterior)
                s->alphaTag = 1u;
                
                // Calculate Posterior
                s->posterior[0u][s->alphaCnt - 1u] = s->posterior[0u][s->alphaCnt - 1u] * s->alpha;
                
                // Send alpha inductively if not in last column
                if ( (s->x) != (NOOFTARG - 1u) ) {
                    s->nxtRdyFlags |= ALPHA;
                }
                
                // Send alpha for linear interpolation if not in the first column
                if ( (s->x) != 0u ) {
                    s->nxtRdyFlags |= ALPHALIN;
                }
                
                // Account for no linear interpolation in final column
                if ((s->x == NOOFTARG - 1u) && (s->y != NOOFSTATES - 1u)){

                    s->currentIndex = s->alphaCnt - 1u;
                    s->nxtRdyFlags |= ACCA;
                    
                }
                
                s->fwdRecCnt = 0u;
                
            }
        
        }
        
        // Received Beta Inductive Message (Reverse Algo)
        if (msg->msgtype == BACKWARD) {
            
            float emissionProb = 0.0f;
            
            if (msg->match == 2u) {
                emissionProb = 1.0f;
            }
            else if (msg->match == 1u) {
                emissionProb = (1.0f - (1.0f / ERRORRATE));
            }
            else {
                emissionProb = (1.0f / ERRORRATE);
            }
            
            if (msg->stateNo == s->y) {
                
                s->beta += msg->val * s->bwdSame * emissionProb;
              
            }
            else {
                
                s->beta += msg->val * s->bwdDiff * emissionProb;
                
            }
            
            s->bwdRecCnt++;
            
            // Has the summation of the beta been calculated?
            if (s->bwdRecCnt == NOOFSTATES) {
                
                // Increase number of betas calculated
                s->betaCnt++;
                
                // Send beta message if we are not the first node
                if ((s->x) != 0u) {
                    s->nxtRdyFlags |= BETA;
                }
                
                // Send beta for linear interpolation if not in the first column
                if ( (s->x) != 0u ) {
                    s->nxtRdyFlags |= BETALIN;
                }
                
                // Calculate linear interpolation values
                float prevBeta = s->beta;
                float totalDiff = s->nextBeta - prevBeta;
            
                for (uint32_t x = 0u; x < (LINRATIO - 1u); x++) {
                    
                    s->betaLin[x] = s->nextBeta - ((s->dmLocal[(LINRATIO - 2u) - x] / s->totalDistance) * totalDiff);
                    s->nextBeta = s->betaLin[x];
                    
                    // Calculate Linear Interpolation Posteriors
                    s->posterior[x + 1u][s->betaCnt - 1u] = s->posterior[x + 1u][s->betaCnt - 1u] * s->betaLin[x];
                    
                }
                
                // Calculate Current Posterior
                s->posterior[0u][s->betaCnt - 1u] = s->posterior[0u][s->betaCnt - 1u] * s->beta;
                
                
                // Send accumulation message if posterior probability is complete
                if ( (s->alphaCnt >= s->betaCnt) && (s->y != NOOFSTATES - 1u) && (s->x != (NOOFTARG / 2u)) ) {
                    s->currentIndex = s->betaCnt - 1u;
                    s->nxtRdyFlags |= ACCA;
                }
                
                s->bwdRecCnt = 0u;

            }
        
        }
        
        // Received Linear Interpolation Message
        if (msg->msgtype == LINEARINTERP) {
            
            // Determine Message type -> Alpha / Beta
            if (msg->stateNo == ALPHA) {
                
                uint32_t offset = 0u;
                
                // Calculate linear interpolation values
                float nextAlpha = msg->val;
                float prevAlpha = s->oldOldAlpha;
                float totalDiff = prevAlpha - nextAlpha;
                
                if (s->alphaTag) {
                    offset = 3u;
                }
                else {
                    offset = 2u;
                }
                
                for (uint32_t x = 0u; x < (LINRATIO - 1u); x++) {
            
                    s->alphaLin[x] = prevAlpha - ((s->dmLocal[x] / s->totalDistance) * totalDiff);
                    prevAlpha = s->alphaLin[x];
                    
                    s->posterior[x + 1u][s->alphaCnt - offset] = s->posterior[x + 1u][s->alphaCnt - offset] * s->alphaLin[x];

                }
                
                // Send accumulation message if posterior probability is complete
                if ((s->betaCnt >= s->alphaCnt) && (s->y != NOOFSTATES - 1)) {
                    s->currentIndex = s->alphaCnt - offset;
                    s->nxtRdyFlags |= ACCA;
                }
                
            }
            else if (msg->stateNo == BETA) {
                
                s->nextBeta = msg->val;
                
            }
            
        }
        
        // Received Accumulation Message (Forward Algo)
        if (msg->msgtype == ACCUMULATE) {
            
            // Get Linear Interp Postition
            uint32_t linPos = (msg->match >> 16u) & 0xFFFF;
            
            s->accaCnt[linPos]++;
            
            // If the transmitting node is a major allele . . 
            if ((msg->match & 1u) == 0u) {
                
                s->majPosterior[linPos] += msg->val;
                
            }
            else {
                
                s->minPosterior[linPos] += msg->val;
                
            }
            
#ifdef IMPDEBUG
    s->sentCnt++;
#endif
            
            if (s->accaCnt[linPos] == NOOFSTATES - 1u) {
                
                // If the final node is a major allele . . (stateNo is holding the posterior index)
                if (s->label == 0u) {
                    
                    s->majPosterior[linPos] += s->posterior[linPos][msg->stateNo];
                    
                }
                else {
                    
                    s->minPosterior[linPos] += s->posterior[linPos][msg->stateNo];
                    
                }
                
                s->nxtRdyFlags |= ALLELECNTS;

                s->accaCnt[linPos] = 0u;
                
            }
            
        }

    }

    // Called by POLite when system becomes idle
    inline bool step() {
        
        // Copy Values Over to enable both stages of processing (induction and accumulation)
        
        s->oldOldAlpha = s->oldAlpha;
        s->oldOldBeta = s->oldBeta;
        s->oldAlpha = s->alpha;
        s->oldBeta = s->beta;
        s->oldIndex = s->currentIndex;
        
        // Clear alpha/beta values ready for next timestep
        s->alpha = 0.0f;
        s->beta = 0.0f;
        
        // Clean accumulator send count
        s->accSendCnt = 0u;
        
        // Transfer and Clear Ready Flags
        s->rdyFlags = s->nxtRdyFlags;
        s->nxtRdyFlags = 0u;
        s->alphaTag = 0u;
        
        // Clear sent flags
        s->sentFlags = 0u;
        
        // Clear Posterior values -> JPM This needs reading if data is to be transferred out
        for (uint32_t x = 0u; x < (LINRATIO - 1u); x++) {
            s->majPosterior[x] = 0.0f;
            s->minPosterior[x] = 0.0f;
        }
        
        // Calculate and send initial alphas
        if ((s->x == 0) && (s->targCnt < NOOFHAPS)) {
            
            s->alpha = 1.0f / NOOFSTATES;
            s->oldAlpha = 1.0f / NOOFSTATES;
            s->rdyFlags |= ALPHA;
            
            // Increase number of alphas calculated
            s->alphaCnt++;
            
        }
        // Calculate and send initial betas
        else if (((s->x) == (NOOFTARG - 1u)) && (s->targCnt < NOOFHAPS)) {
            
            s->beta = 1.0f;
            s->oldBeta = 1.0f;
            s->rdyFlags |= BETA;
            s->rdyFlags |= BETALIN;
            
            // Increase number of betas calculated
            s->betaCnt++;
            
        }
        
        s->targCnt++;
        
        if ((s->rdyFlags & ALPHA) && !(s->sentFlags & ALPHA)) {
            
            *readyToSend = Pin(FORWARD);
            
            return true;
            
        }
        
        if ((s->rdyFlags & BETA) && !(s->sentFlags & BETA)) {
            
            *readyToSend = Pin(BACKWARD);
            
            return true;
            
        }
        
        if ((s->rdyFlags & BETALIN) && !(s->sentFlags & BETALIN)) {
            
            *readyToSend = Pin(LINEARINTERP);
            
            return true;
            
        }
        
        if ((s->rdyFlags & ACCA) && !(s->sentFlags & ACCA)) {
            
            *readyToSend = Pin(ACCUMULATE);
            
            return true;
            
        }
        
        // There is nothing to be sent
        return false;
        
    }

    // Optionally send message to host on termination
    inline bool finish(volatile ImpMessage* msg) {
        
        msg->msgtype = s->id;
#ifdef IMPDEBUG
        msg->val = (float)s->sentCnt;
        //msg->val = s->posterior[0u][0u];
        //msg->val = (float)s->accaCnt[0u];
#else
        msg->val = 1.0f;
#endif
        return true;
        
    }
};



#endif