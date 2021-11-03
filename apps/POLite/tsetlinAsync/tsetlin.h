// SPDX-License-Identifier: BSD-2-Clause
#ifndef _TSETLIN_H_
#define _TSETLIN_H_

//#define POLITE_DUMP_STATS
//#define POLITE_COUNT_MSGS
#include "myPOLite.h"
#include "params.h"

// Preprocessor Directives
#define PENALTY (0u)
#define REWARD (1u)

// ImpMessage types
const uint32_t BROADCAST = TRAINSIZE;
//const uint32_t BETAINDUCT =  1u;
//const uint32_t TERMINATION = 2u;

// State Types
const uint32_t ASYNCSTATE =  0u;
const uint32_t GLOBALSTATE =  1u;

// Vote Types
const uint16_t INIT =  2u;
const uint16_t CHALLENGE =  3u;
const uint16_t REQCHALLENGE =  4u;

// Flags

const uint32_t BCASTCHAL = (1 << 22); //1
const uint32_t BCASTCHALREQ = (1 << 23); //2

// Function Prototypes
static uint16_t randomNum(uint16_t lfsr);
static void update_ta(uint8_t* ta, uint8_t action);


struct ImpMessage {
    
    // Vote
    uint16_t vote;
    // Source Address
    uint32_t srcClause;
    // Datapoint X
    uint32_t dpX;
    // Datapoint Y
    uint32_t dpY;
    
};

struct ImpState {

    // Device id
    uint32_t id; 
    // State Type
    uint16_t state;
    // Message request flags
    uint32_t rdyFlags[5];
    // Message sent flags
    uint32_t sentFlags[5];
    // Clause Output
    uint32_t clauseOutput[5];
    // lfsr State
    uint16_t lfsr;
    // Node's clause number
    uint32_t clauseNo;
    // Tsetlin Automata
    uint8_t ta[NOOFLITERALS];
    // PRNG Seed
    uint32_t seed;
    // Previous Clause Output Oij*
    uint8_t prevClause[TRAINSIZE];
    // Class of clause
    uint8_t classNo;
    // Epoch Counter
    uint16_t epoch;
    
    // Example Pool
    // Dataset Datapoint
    uint32_t dpX;
    uint32_t dpY;
    // Votes
    int32_t iris[3];

    
    // Received Message Counter
    uint32_t recCnt; //

#ifdef TMDEBUG
    // Test Counter
    uint32_t testCnt;
#endif
   
};

struct ImpDevice : PDevice<ImpState, None, ImpMessage> {

    // Called once by POLite at start of execution
    inline void init() {
        
#ifdef TMDEBUG
        // Test Counter
        s->testCnt = 0u;
#endif
        
        // Intialise Received Counter
        s->recCnt = 0u;
        
        // Initialise Epochs
        s->epoch = 0u;

        // Clear Iris Counts
        for (uint32_t x = 0u; x < 3u; x++) {
            
            s->iris[x] = 0u;
        
        }
        
        // Clear flags and Clause bits
        for (uint32_t x = 0u; x < 5u; x++) {
            
            s->rdyFlags[x] = 0u;
            s->sentFlags[x] = 0u;
            s->clauseOutput[x] = 0u;
            
        }
        
        // Initilise Node
        s->lfsr = s->seed & 0xFFFF;
        
        // Determine Clause Class
        if (s->clauseNo < LOWERCLASSBND) {
            
            s->classNo = 0u;
            
        }
        else if (s->clauseNo >= UPPERCLASSBND) {
            
            s->classNo = 2u;
            
        }
        else {
            
            s->classNo = 1u;
            
        }
        
        *readyToSend = No;
        
    }

    // Send handler
    inline void send(volatile ImpMessage* msg) {
        
        // If the request is to multicast
        if (*readyToSend == Pin(BROADCAST)) {
            
            if ((s->rdyFlags[4] & BCASTCHAL) && !(s->sentFlags[4] & BCASTCHAL)) {
                
                
                if (s->epoch == 1u) {
                    msg->vote = INIT;
                }
                else {
                    msg->vote = CHALLENGE;
                }
                
                msg->srcClause = s->clauseNo;
                msg->dpX = s->dpX;
                msg->dpY = s->dpY;
                s->sentFlags[4] |= BCASTCHAL;
            
            }
            else if ((s->rdyFlags[4] & BCASTCHALREQ) && !(s->sentFlags[4] & BCASTCHALREQ)) {
            
                msg->srcClause = s->clauseNo;
                msg->vote = REQCHALLENGE;
                msg->dpX = 0u;
                msg->dpY = 0u;
                s->sentFlags[4] |= BCASTCHALREQ;
            
            }

        }
        else {
            // The request is for a unicast response
            for (uint32_t x = 0u; x < TRAINSIZE; x++) {
                
                if (*readyToSend == Pin(x)) {
                    
                    uint8_t wordNo = 0u;
                    uint32_t tempClauseNo = s->clauseNo;
    
                    // Find array index and remainder
                    while(tempClauseNo >= 32u){
                            tempClauseNo = tempClauseNo - 32u;
                            wordNo++;
                    }
                    
                    // Check if already sent
                    if (!(s->sentFlags[wordNo] & (1u << tempClauseNo))) {
                    
                        // Construct Response
                        msg->srcClause = s->clauseNo;
                        msg->dpX = 0u;
                        msg->dpY = 0u;
                        
                        // Find Vote
                        msg->vote = (((s->clauseOutput[wordNo]) >> tempClauseNo) & 0x1);
                        
                        // Mark Correct Sent Flag
                        s->sentFlags[wordNo] |= (1u << tempClauseNo);
                    
                    }
                    
                    
                }
                
            }
            
        }
        
        // Check for other messages to be sent
        
        uint8_t more = 0u;

        for (uint32_t x = 0u; x < TRAINSIZE; x++) {
            
            uint8_t wordNo = 0u;
            uint32_t index = x;

            // Find array index and remainder
            while(index >= 32u){
                    index = index - 32u;
                    wordNo++;
            }
            
            if ((s->rdyFlags[wordNo] & index) && !(s->sentFlags[wordNo] & index)) {
                
                *readyToSend = Pin(x);
                more = 1u;
                break;
                
            }
            
        }
        
        // If there are no more messages to send
        if (!more)  {
            *readyToSend = No;
        }
        
    
    }

    // Receive handler
    inline void recv(ImpMessage* msg, None* edge) {
       
        // Received Challenge Message
        if (msg->vote == INIT) {
            
            uint32_t clause = 0u;
            
            // Increment Received Counter
            s->recCnt++;
            
            // Calculate Clause 
            
            for (uint32_t x = 0u; x < NOOFINPUTLIT; x++) {
                     
                // If ta > n, include literal in clause calculation
                if (s->ta[x] >= INCBOUND) {
                     
                    clause &= msg->dpX & (1u << x);
                         
                 }
                 
            }
            
            // Update Local Clause Monitor
            uint8_t wordNo = 0u;
            uint32_t tempSrcClause = msg->srcClause;
            
            // Find array index and remainder
            while(tempSrcClause >= 32u){
                    tempSrcClause = tempSrcClause - 32u;
                    wordNo++;
            }
            
            if (clause == 1u) {
                
                s->clauseOutput[wordNo] |= (1u << tempSrcClause);
                
                // Indicate Response
                s->rdyFlags[wordNo] |= tempSrcClause;
                
                // Call send to correct exterior vote count
                *readyToSend = Pin(msg->srcClause);
    
            }
            else {
                
                s->clauseOutput[wordNo] &= ~(1u << tempSrcClause);
                
            }
            
        }
        else if (msg->vote == CHALLENGE) {
            
            //Train Tsetlin Machine
            
            // Calculate Clause 
            uint32_t clause = 0u;
            
            for (uint32_t x = 0u; x < NOOFINPUTLIT; x++) {
                     
                // If ta > n, include literal in clause calculation
                if (s->ta[x] >= INCBOUND) {
                     
                    clause &= msg->dpX & (1u << x);
                         
                }
                 
            }
            
            uint8_t classConfidence[NOOFCLASSES] = {0u};
            
            for (uint8_t x = 1u; x < (NOOFCLASSES + 1u); x++) {
                
                classConfidence[x - 1u] =  (msg->dpY >> x * 8u) & 0xFF;
                
            }
            
            // If clause is odd, Cj1. else Cj0
            if ((s->clauseNo & 1u) == 1u) {
                
                //Cj1
                
                // If the output y is 1
                if (s->classNo == (msg->dpY & 0xFF)) {
                    
                    //Provide TypeI Feedback
                    s->lfsr = randomNum(s->lfsr);
                    float random = (float)s->lfsr / (float)RANDMAX;
        
                    if (random < ((THRESH - classConfidence[s->classNo])/(2.0f * THRESH))) { 
                        
                        // Is the clause 'active'
                        if (clause == 1u) {
                            
                            // Provide TypeI feedback for Cj1
                            for (uint32_t y = 0u; y < NOOFINPUTLIT; y++) {
                                
                                // If literal is 1
                                if (((msg->dpX >> y) & 1u) == 1u) {
                                    
                                    // Literal is 1
                                    
                                    // Is the literal included in the clause?
                                    if (s->ta[y] >= INCBOUND) {
                                        
                                        // Literal is included
                                        
                                        s->lfsr = randomNum(s->lfsr);
                                        float random = (float)s->lfsr / (float)RANDMAX;
                                        
                                        if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                            
                                            // High probability action -> reward
                                            update_ta(&s->ta[y], REWARD);                                                                   //C1, L1, I1
                                            
                                        }
                                        else {
                                            
                                            // Low probability action -> inaction                                                   
                                            
                                        }
                                        
                                        
                                    }
                                    else {
                                        
                                        s->lfsr = randomNum(s->lfsr);
                                        float random = (float)s->lfsr / (float)RANDMAX;
                                        
                                        if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                            
                                            // High probability action -> penalty
                                            update_ta(&s->ta[y], PENALTY);                                                                   //C1, L1, I0
                                            
                                        }
                                        else {
                                            
                                            // Low probability action -> inaction
                                            
                                        }
                                        
                                    }
                                    

                                }
                                else {
                                    
                                    // Literal is 0
                                    
                                    // Is the literal included in the clause?
                                    if (s->ta[y] >= INCBOUND) {
                                        
                                        // Literal is included -> This case should never be valid                                           //C1, L0, I1
                                    
                                            
                                    }
                                    else {
                                        
                                        // Literal is excluded
                                        
                                        s->lfsr = randomNum(s->lfsr);
                                        float random = (float)s->lfsr / (float)RANDMAX;

                                        if (random >= 1.0f/LEARNINGSENSITIVITY) {

                                            // High probability action -> inaction                                                          
                                            
                                        }
                                        else {
                                            
                                            // Low probability action -> reward
                                            update_ta(&s->ta[y], REWARD);                                                                   //C1, L0, I0
                                            
                                        }
                                        
                                    }
                                    
                                }
                                
                            }
                            
                        }
                        else {
                            
                                // Clause is 'inactive'    
                                for (uint32_t y = 0u; y < NOOFINPUTLIT; y++) {
                                    
                                    // If literal is 1
                                    if (((msg->dpX >> y) & 1u) == 1u) {
                                        
                                        // Literal is 1
                                        
                                        // Is the literal included in the clause?
                                        if (s->ta[y] >= INCBOUND) {
                                            
#ifdef TMDEBUG
        // Test Counter
        s->testCnt++;
#endif 
                                            
                                            // Literal is included
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                                                                                  
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> penalty
                                                update_ta(&s->ta[y], PENALTY);                                                                  //C0, L1, I1
                                                
                                            }
                                            
                                            
                                        }
                                        else {
                                            
                                            // Literal is excluded
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> reward
                                                update_ta(&s->ta[y], REWARD);                                                                   //C0, L1, I0
                                                
                                            }
                                            
                                        }
                                        
                                    }
                                    else {
                                        
                                        // Literal is 0
                                        
                                        // Is the literal included in the clause?
                                        if (s->ta[y] >= INCBOUND) {
                                         
                                            // Literal is included
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                                                                                  
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> penalty
                                                update_ta(&s->ta[y], PENALTY);                                                                  //C0, L0, I1
                                                
                                            }
                                            
                                                
                                            
                                        }
                                        else {
                                            
                                            // Literal is excluded
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                                                                                  
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> reward
                                                update_ta(&s->ta[y], REWARD);                                                                  //C0, L0, I0
                                                
                                            }
                                            
                                            
                                            
                                        }
                                        
                                    }
                                    
                                }    
                                    
                            
                            
                        }
                        
                    }
                    
                }
                else {
                    
                    // The output y is 0
                    
                    // Provide TypeII Feedback for Cj1
                    s->lfsr = randomNum(s->lfsr);
                    float random = (float)s->lfsr / (float)RANDMAX;
        
                    if (random < ((THRESH + classConfidence[s->classNo])/(2.0f * THRESH))) {

                        // Only one combination requires feedback -> C1 L0 I0
                        if (clause == 1u) {
                            
                            for (uint32_t y = 0u; y < NOOFINPUTLIT; y++) {
                                
                                // If literal is 0
                                if (((msg->dpX >> y) & 1u) == 0u) {
                                 
                                    // Is the literal excluded from the clause?
                                    if (s->ta[y] <= EXCBOUND) {
                                     
                                        // Provide penalty feedback
                                        update_ta(&s->ta[y], PENALTY);                                                                  //C1, L0, I0
           
                                    }
           
                                }
           
                            }
                            
                        }

                    }
                    
                }
                
            }
            else {
                
                // Cj0
                
                // If the output y is 1
                if (s->classNo == (msg->dpY & 0xFF)) {
                    
                    // Provide TypeII Feedback for Cj0
                    s->lfsr = randomNum(s->lfsr);
                    float random = (float)s->lfsr / (float)RANDMAX;
        
                    if (random < ((THRESH + classConfidence[s->classNo])/(2.0f * THRESH))) {

                        // Only one combination requires feedback -> C1 L0 I0
                        if (clause == 1u) {
                            
                            for (uint32_t y = 0u; y < NOOFINPUTLIT; y++) {
                                
                                // If literal is 0
                                if (((msg->dpX >> y) & 1u) == 0u) {
                                 
                                    // Is the literal excluded from the clause?
                                    if (s->ta[y] <= EXCBOUND) {
                                     
                                        // Provide penalty feedback
                                        update_ta(&s->ta[y], PENALTY);                                                                  //C1, L0, I0
           
                                    }
           
                                }
           
                            }
                            
                        }

                    }
                    
                }
                else {
                    
                    // Output y is 0
                    
                    
                    // Provide TypeI Feedback
                    s->lfsr = randomNum(s->lfsr);
                    float random = (float)s->lfsr / (float)RANDMAX;
        
                    if (random < ((THRESH - classConfidence[s->classNo])/(2.0f * THRESH))) { 
                        
                        // Is the clause 'active'
                        if (clause == 1u) {
                            
                            // Provide TypeI feedback for Cj1
                            for (uint32_t y = 0u; y < NOOFINPUTLIT; y++) {
                                
                                // If literal is 1
                                if (((msg->dpX >> y) & 1u) == 1u) {
                                    
                                    // Literal is 1
                                    
                                    // Is the literal included in the clause?
                                    if (s->ta[y] >= INCBOUND) {
                                        
                                        // Literal is included
                                        
                                        s->lfsr = randomNum(s->lfsr);
                                        float random = (float)s->lfsr / (float)RANDMAX;
                                        
                                        if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                            
                                            // High probability action -> reward
                                            update_ta(&s->ta[y], REWARD);                                                                   //C1, L1, I1
                                            
                                        }
                                        else {
                                            
                                            // Low probability action -> inaction                                                   
                                            
                                        }
                                        
                                        
                                    }
                                    else {
                                        
                                        s->lfsr = randomNum(s->lfsr);
                                        float random = (float)s->lfsr / (float)RANDMAX;
                                        
                                        if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                            
                                            // High probability action -> penalty
                                            update_ta(&s->ta[y], PENALTY);                                                                   //C1, L1, I0
                                            
                                        }
                                        else {
                                            
                                            // Low probability action -> inaction
                                            
                                        }
                                        
                                    }
                                    

                                }
                                else {
                                    
                                    // Literal is 0
                                    
                                    // Is the literal included in the clause?
                                    if (s->ta[y] >= INCBOUND) {
                                        
                                        // Literal is included -> This case should never be valid                                           //C1, L0, I1
                                    
                                            
                                    }
                                    else {
                                        
                                        // Literal is excluded
                                        
                                        s->lfsr = randomNum(s->lfsr);
                                        float random = (float)s->lfsr / (float)RANDMAX;

                                        if (random >= 1.0f/LEARNINGSENSITIVITY) {

                                            // High probability action -> inaction                                                          
                                            
                                        }
                                        else {
                                            
                                            // Low probability action -> reward
                                            update_ta(&s->ta[y], REWARD);                                                                   //C1, L0, I0
                                            
                                        }
                                        
                                    }
                                    
                                }
                                
                            }
                            
                        }
                        else {
                            
                                // Clasue is 'inactive'    
                                for (uint32_t y = 0u; y < NOOFINPUTLIT; y++) {
                                    
                                    // If literal is 1
                                    if (((msg->dpX >> y) & 1u) == 1u) {
                                        
                                        // Literal is 1
                                        
                                        // Is the literal included in the clause?
                                        if (s->ta[y] >= INCBOUND) {
                                            
                                            // Literal is included
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                                                                                  
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> penalty
                                                update_ta(&s->ta[y], PENALTY);                                                                  //C0, L1, I1
                                                
                                            }
                                            
                                            
                                        }
                                        else {
                                            
                                            // Literal is excluded
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> reward
                                                update_ta(&s->ta[y], REWARD);                                                                   //C0, L1, I0
                                                
                                            }
                                            
                                        }
                                        
                                    }
                                    else {
                                        
                                        // Literal is 0
                                        
                                        // Is the literal included in the clause?
                                        if (s->ta[y] >= INCBOUND) {
                                         
                                            // Literal is included
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                                                                                  
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> penalty
                                                update_ta(&s->ta[y], PENALTY);                                                                  //C0, L0, I1
                                                
                                            }
                                            
                                                
                                            
                                        }
                                        else {
                                            
                                            // Literal is excluded
                                            
                                            s->lfsr = randomNum(s->lfsr);
                                            float random = (float)s->lfsr / (float)RANDMAX;
                                            
                                            if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                
                                                // High probability action -> inaction
                                                                                                                  
                                                
                                            }
                                            else {
                                                
                                                // Low probability action -> reward
                                                update_ta(&s->ta[y], REWARD);                                                                  //C0, L0, I0
                                                
                                            }
                                            
                                            
                                            
                                        }
                                        
                                    }
                                    
                                }    
                                    
                            
                            
                        }
                        
                    }
                    
                    
                }
                
            }
            
            // Recalculate Clause
            uint32_t newClause = 0u;
            
            for (uint32_t x = 0u; x < NOOFINPUTLIT; x++) {
                     
                // If ta > n, include literal in clause calculation
                if (s->ta[x] >= INCBOUND) {
                     
                    newClause &= msg->dpX & (1u << x);
                         
                 }
                 
            }
            
            // Check if clause value has changed
            if (newClause != clause) {
                
                uint8_t wordNo = 0u;
                uint32_t tempClauseNo = msg->srcClause;

                // Find array index and remainder
                while(tempClauseNo >= 32u){
                        tempClauseNo = tempClauseNo - 32u;
                        wordNo++;
                }
                
                // Correct Local Clause Array
                if (clause == 1u) {
                
                    s->clauseOutput[wordNo] |= (1u << tempClauseNo);
    
                }
                else {
                    
                    s->clauseOutput[wordNo] &= ~(1u << tempClauseNo);
                    
                }
                
                // Indicate Response
                s->rdyFlags[wordNo] |= tempClauseNo;
                
                // Call send to correct exterior vote count
                *readyToSend = Pin(msg->srcClause);
                
            }
            
        }
        // Is the message a response to update the class confidences?
        else if (msg->vote < 2u) {
            
            // Update Vote Count
            
            // Determine Source Clause Class
            if (msg->srcClause < LOWERCLASSBND) {
                
                s->classNo = 0u;
                
            }
            else if (msg->srcClause >= UPPERCLASSBND) {
                
                s->classNo = 2u;
                
            }
            else {
                
                s->classNo = 1u;
                
            }
            
            // Determine Source Clause Polarity
            // If odd, -, if even, +
            if ((msg->srcClause & 1u) == 1u) {
                
                if (msg->vote == 0u) {
                    // Odd clause, 1->0 = ++
                    s->iris[s->classNo]++;
                    
                }
                else {
                    // Odd clause, 0->1 = --
                    s->iris[s->classNo]--;
                }
                
            }
            else {
                
                if (msg->vote == 0u) {
                    // Even clause, 1->0 = --
                    s->iris[s->classNo]--;
                    
                }
                else {
                    // Even clause, 0->1 = ++
                    s->iris[s->classNo]++;
                }
                
            }
            
        }
        
    }

    // Called by POLite when system becomes idle
    inline bool step() {
        
        // Code for managing network bandwidth
        /*
            // Determine whether node should issue challenge
            double randNo = (double)randomNum();
            double challengeProb = randNo / (float)0xFFFF;
            
            if (challengeProb > BCASTPROB) {
                
                // Issue challenge
                
                
            }
        */
        
        // Clear flags and Clause bits
        for (uint32_t x = 0u; x < 5u; x++) {
            
            s->rdyFlags[x] = 0u;
            s->sentFlags[x] = 0u;
            
        }
        
        // First iteration or re-evalutation requires a global broadcast
        if (s->epoch < (NOOFEPOCHS + 1u)) {
            
            // Increment epochs
            s->epoch++;
            
            // Indicate Initial Challenge Broadcast
            s->rdyFlags[4] |= BCASTCHAL;
            
            // Broadcast challenge
            *readyToSend = Pin(BROADCAST);
            
            // Inidicate wish to continue
            return true;
            
        }
       
        // There is nothing to be sent
        return false;
        
    }

    // Optionally send message to host on termination
    inline bool finish(volatile ImpMessage* msg) {
        
        msg->srcClause = s->id;
        msg->vote = (float)s->testCnt;
        //msg->vote = (float)s->dpY;
        
        return true;
        
    }
};


/***************************************************
* Support Functions
****************************************************/

// Simple LFSR-based Pseudo-Random Number Generator

static uint16_t randomNum(uint16_t lfsr) {

    uint16_t bit;

    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr = ((lfsr >> 1) | (bit << 15));
     
    return lfsr;

}

// TA Update Function
static void update_ta(uint8_t* ta, uint8_t action) {
    
    // If action is reward
    if (action == REWARD) {
        
        // Is the ta currently included?
        if (*ta >= INCBOUND) {
            
            // ta is currently included. Increment if not already maximum.
            if (*ta < UPPERBOUND) {
                *ta = *ta + 1u;
            }
        }
        else {
            
            // ta is currently excluded. Decrement if not already minimum
            if (*ta > LOWERBOUND) {
                *ta = *ta - 1u;
            }
            
        }
        
    }
    
    // If action is penalty
    if (action == PENALTY) {
        
        // Is the ta currently included?
        if (*ta >= INCBOUND) {
            
            // ta is currently included. Decrement.
            *ta = *ta - 1u;
            
        }
        else {
            
            // ta is currently excluded. Increment
            *ta = *ta + 1u;
                
        }
        
    }
    
}

#endif