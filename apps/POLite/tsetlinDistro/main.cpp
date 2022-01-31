/*****************************************************
 * Tsetlin Machine - Single Threaded Distributed Simulation
 * ***************************************************
 * This code simulates the distributed, parallelised, version of the Tsetlin Machine as set out in the paper below
 * 
 * USAGE:
 * To Be Completed ...
 * 
 * PLEASE NOTE:
 * To Be Completed ...
 * 
 * 
 * ****************************************************/
 
//***********************
//PREPROCESSOR DEFINTIONS
//***********************
#define NOOFDEVICES (120u)
#define MAXMESSAGES (NOOFDEVICES + CHALLENGESPERROUND)

#define MAXFANOUT (NOOFDEVICES + 1u)

#define No (POLITE_NUM_PINS)

/**
 * Returns polarity of k-th literal: odd literals are negated (1), even literals are positive (0)
 */
#define POLARITY(k) ((k)&1)

/**
 * Calculate the value of k-th literal on the given input respecting literal polarity
 */
#define LITERAL_VALUE(lit, k) (lit ^ POLARITY(k))

/**
 * Determine include (1) or exclude (0) decision based on a TA state
 */
#define INCLUDE_LITERAL(state) ((state) >= INCBOUND)

//***********************
//LIBRARY INCLUDES
//***********************
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "params.h"
#include "data.h"

//***********************
//TYPE DEFINITIONS
//***********************
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
    uint32_t rdyFlags[FLAGWORDS];
    // Message sent flags
    uint32_t sentFlags[FLAGWORDS];
    // Clause Output
    uint32_t clauseOutput[FLAGWORDS];
    // lfsr State
    uint16_t lfsr;
    // Node's clause number
    uint32_t clauseNo;
    // Next clause number
    uint32_t nextNegClass;
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
    // Own Class Word Number
    uint32_t ownWordNo;
    // Own Class Index Number
    uint32_t ownIndex;
    // Own Pin Number
    uint32_t ownPinNo;
    
    // Example Pool
    // Dataset Datapoint
    uint32_t dpX;
    uint32_t dpY;
    // Votes
    int8_t cSum[3];

    
    // Received Message Counter
    uint32_t recCnt; //
    
#ifdef TMDEBUG
    // Test Counter
    uint32_t testCnt;
    uint32_t lastMsg;
#endif
   
};

//***********************
//GLOBAL VARIABLES
//***********************

struct ImpMessage message[NOOFDEVICES][MAXMESSAGES] = {{ 0u }};
uint32_t ptrMsg[NOOFDEVICES] = { 0u };
struct ImpState node[NOOFDEVICES] = {{ 0u }}; 

uint32_t Pin[POLITE_NUM_PINS];

// [PinNumber][Allocation up to all destinations][actual destinations]
int32_t pinDests[POLITE_NUM_PINS][MAXFANOUT];

//Ready To Send
uint32_t readyToSend[NOOFDEVICES];

//***********************
//GLOBAL CONSTANTS
//***********************

// ImpMessage types
const uint32_t BROADCAST = TRAINSIZE + NOOFCLASSES;
//const uint32_t BETAINDUCT =  1u;
//const uint32_t TERMINATION = 2u;

// State Types
const uint32_t ASYNCSTATE =  0u;
const uint32_t GLOBALSTATE =  1u;

// Vote Types
// 0u and 1 are requested to increment / decrement cSum
const uint16_t INIT =  2u;
const uint16_t CHALLENGE =  3u;
const uint16_t REQCHALLENGE =  4u;

// Flags

const uint32_t INITCHAL = (1 << 27); //1

//***********************
//FUNCTION PROTOTYPES
//***********************
static void initialise(void);
static void initialiseNodes(void);
static bool step(void);
static void sends(void);
static void sendMessage(struct ImpMessage outbndMsg, uint32_t pin);
static void receives(void);
static ImpMessage receiveMessage(uint32_t node);
static bool messagesToHandle(void);
static uint16_t randomNum(uint16_t lfsr);

//***********************
//MAIN FUNCTION
//***********************
int main(void) {
    
    // Simulate x86 Side Initialisation
    initialise();
    
    // Simulate Node Side Initialisation
    initialiseNodes();
    
    // Simulate Node-Side Execution
    while( step() ) {
        
        
        while( messagesToHandle() ) {
            
            // Simulate Node Side Sends -> JPM implementation currently assumes all sends possible per timestep
            sends();
            
            // Simulate Node Side Receives -> JPM implementation currently assumes all receives possible per timestep
            receives();
        
        }
        
    }
    
    
	printf("Program Complete . . \n");
	return 0;
}

//***********************
//FUNCTION DEFINITIONS
//***********************
static void initialise(void) {
    
    //INITIALIZE PINS
    
    // Initialise Pins
    for (uint32_t x = 0u; x < POLITE_NUM_PINS; x++) {
        
        Pin[x] = x;
        
    }
    
    // Initialise Pin Dests to Stop Value
    for (uint32_t i = 0u; i < POLITE_NUM_PINS; i++) {
        for (uint32_t j = 0u; j < MAXFANOUT; j++) {
                
            pinDests[i][j] = -1;
                
        }
    }

    
    // CREATE EDGE MAPPING
    
    // Add unicast edges
    for (uint32_t i = 0u; i < TRAINSIZE; i++) {
                
        pinDests[i][0u] = i;
                
    }
    
    uint32_t cnt = 0u;
    
    // Add class specific broadcast addresses
    for (uint32_t i = TRAINSIZE; i < (TRAINSIZE + NOOFCLASSES); i++) {
        for (uint32_t j = 0u; j < CLAUSESPERCLASS; j++) {
                
            pinDests[i][j] = cnt;
            cnt++;
            
        }
    }
    
    // Add complete broadcast address for intialisation
    for (uint32_t i = 0u; i < TRAINSIZE; i++) {
                
        pinDests[(TRAINSIZE + NOOFCLASSES)][i] = i;
                
    }
    
    // INITIALIZE DEVICES
    for (uint32_t x = 0; x < TRAINSIZE; x++) {
                
            // Initialise device IDs
            node[x].id = x;
            
            // Assign node it's clause number
            node[x].clauseNo = x;
            
            // Assign node it's class number
            node[x].classNo = x / CLAUSESPERCLASS;
            
            // Set initial states of Tsetlin Automata
            for (uint32_t y = 0; y < NOOFLITERALS; y += 2u) {
            
                double random = (double)rand() / (double)RAND_MAX;
                    
                if (random <= 0.5f) {
                    node[x].ta[y] = INCBOUND;
                    node[x].ta[y+1u] = EXCBOUND;
                }
                else {
                    node[x].ta[y] = EXCBOUND;
                    node[x].ta[y+1u] = INCBOUND;
                }
                
                node[x].seed = rand();
            
            }
            
            uint32_t datapoint = 0u;
                
            // Accumulate Datapoint In
            for (uint32_t y = 0; y < NOOFFEATURES; y++) {
                
                datapoint = (datapoint << 1u) + in_train[x][y];
                
            }
            
            node[x].dpX = datapoint;
                
            node[x].dpY = out_train[x]; 
             
    }
    
}

static void initialiseNodes(void) {
    
    // For All Nodes
    for (uint32_t x = 0; x < TRAINSIZE; x++) {
        
        // Intialise Received Counter
        node[x].recCnt = 0u;
        
        // Initialise Epochs
        node[x].epoch = 0u;

        // Clear cSum Counts
        for (uint32_t y = 0u; y < 3u; y++) {
            
            node[x].cSum[y] = 0u;
        
        }
        
        // Clear flags and Clause bits
        for (uint32_t y = 0u; y < FLAGWORDS; y++) {
            
            node[x].rdyFlags[y] = 0u;
            node[x].sentFlags[y] = 0u;
            node[x].clauseOutput[y] = 0u;
            
        }
        
        // Initilise Node
        node[x].lfsr = node[x].seed & 0xFFFF;
        
        // Initialise next class. This will be rolled to classNo + 1 on first step
        node[x].nextNegClass = node[x].classNo;
        
        // Calculate own pin number
        node[x].ownPinNo = TRAINSIZE + node[x].classNo;
        
        uint32_t wordNo = 0u;
        uint32_t index = node[x].ownPinNo;

        // Find array index and remainder
        while(index >= 32u){
                index = index - 32u;
                wordNo++;
        }
        
        // Set word number and index for own class
        node[x].ownWordNo = wordNo;
        node[x].ownIndex = index;
        
        readyToSend[x] = No;
        
    }
    
}

static bool step(void) {
    
    bool keepRunning = false;
    
    // For All Nodes
    for (uint32_t x = 0; x < TRAINSIZE; x++) {
        
        // Clear flags and Clause bits
        for (uint32_t y = 0u; y < FLAGWORDS; y++) {
            
            node[x].rdyFlags[y] = 0u;
            node[x].sentFlags[y] = 0u;
            
        }
        
        // First iteration or re-evalutation requires a global broadcast
        if (node[x].epoch < (NOOFEPOCHS + 1u)) {
            
            do {
                // Calculate next negative class
                node[x].nextNegClass = node[x].nextNegClass + 1u;
                // Wrap nextNegClass without modulo instruction
                if (node[x].nextNegClass == NOOFCLASSES) {
                    node[x].nextNegClass = 0u;
                }
           } while (node[x].nextNegClass == node[x].classNo);
            
            // Is this the first epoch? -> Initialisation
            if (node[x].epoch == 0u) {
                
                // Indicate Initial Challenge Broadcast
                node[x].rdyFlags[FLAGWORDS - 1u] |= INITCHAL;
                
                // Broadcast initialisation challenge
                readyToSend[x] = Pin[BROADCAST];
                
            }
            else {
                
                // Mark own class
                node[x].rdyFlags[node[x].ownWordNo] |= (1u << node[x].ownIndex);
                
                // Calculate pin number
                uint32_t pinNo = TRAINSIZE + node[x].nextNegClass;
                
                uint32_t wordNo = 0u;
                uint32_t index = pinNo;

                // Find array index and remainder
                while(index >= 32u){
                        index = index - 32u;
                        wordNo++;
                }
                
                // Mark next non-matching class multicast Flag
                node[x].rdyFlags[wordNo] |= (1u << index);
                
                // Broadcast initialisation challenge
                readyToSend[x] = Pin[pinNo];
                
            }
            
            // Increment epochs
            node[x].epoch++;
            
            // Inidicate wish to continue
            keepRunning = true;
        
        }
        
    }
    
    return keepRunning;
    
}

static void sends(void) {
 
    // For All Nodes
    for (uint32_t y = 0; y < TRAINSIZE; y++) {
        
        // Check Whether node has any messages to send
        while(readyToSend[y] != No) {
            
            // Outbound Message
            ImpMessage outbndMsg;
        
            // Find Pin
            for (uint32_t x = 0u; x < POLITE_NUM_PINS; x++) {
                
                if (readyToSend[y] == Pin[x]) {
                    
                    uint32_t wordNo = 0u;
                    uint32_t tempClauseNo = x;

                    // Find array index and remainder
                    while(tempClauseNo >= 32u){
                            tempClauseNo = tempClauseNo - 32u;
                            wordNo++;
                    }
                    
                    // Is this a unicast message?
                    if (x < TRAINSIZE) {
                        
                        // Check if not already sent
                        if (!(node[y].sentFlags[wordNo] & (1u << tempClauseNo))) {
                        
                            // Construct Response
                            outbndMsg.srcClause = node[y].clauseNo;
                            outbndMsg.dpX = 0u;
                            outbndMsg.dpY = 0u;
                            
                            // Find Vote
                            outbndMsg.vote = (((node[y].clauseOutput[wordNo]) >> tempClauseNo) & 1u);
                            
                            // Mark Correct Sent Flag
                            node[y].sentFlags[wordNo] |= (1u << tempClauseNo);
                        
                        }
                    
                    }
                    else {
                        // This is a request to broadcast a challange
                        
                        // Check if not already sent
                        if (!(node[y].sentFlags[wordNo] & (1u << tempClauseNo))) {
                        
                            if (node[y].epoch == 1u) {
                                outbndMsg.vote = INIT;
                            }
                            else {
                                outbndMsg.vote = CHALLENGE;
                            }
                            
                            outbndMsg.srcClause = node[y].clauseNo;
                            outbndMsg.dpX = node[y].dpX;
                            outbndMsg.dpY = (node[y].cSum[2] << 24u) + (node[y].cSum[1] << 16u) + (node[y].cSum[0] << 8u) + (node[y].dpY);
                            
                            // Mark Correct Sent Flag
                            node[y].sentFlags[wordNo] |= (1u << tempClauseNo);
                        
                        }
                        
                    }
                    
                    // Call simulated message handler
                    sendMessage(outbndMsg, Pin[x]);
                }
                
            }
            
            // Check for other messages to be sent
            
            uint8_t more = 0u;

            for (uint32_t x = 0u; x < POLITE_NUM_PINS; x++) {
                
                uint8_t wordNo = 0u;
                uint32_t index = x;

                // Find array index and remainder
                while(index >= 32u){
                        index = index - 32u;
                        wordNo++;
                }
                
                if ( (node[y].rdyFlags[wordNo] & (1 << index)) && !(node[y].sentFlags[wordNo] & (1 << index)) ) {
                    
                    readyToSend[y] = Pin[x];
                    more = 1u;
                    break;
                    
                }
                
            }
            
            // If there are no more messages to send
            if (!more)  {
                readyToSend[y] = No;
            }
            
        }
    
    }
    
}

static void sendMessage(struct ImpMessage outbndMsg, uint32_t pin) {
 
    uint32_t i = 0u;
    
    do {
        // Copy Outbound Message to Destinations
        message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].dpX = outbndMsg.dpX;
        message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].dpY = outbndMsg.dpY;
        message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].srcClause = outbndMsg.srcClause;
        message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].vote = outbndMsg.vote;
        
        // Increment Counter
        ptrMsg[pinDests[pin][i]]++;
        i++;
        
    }
    while (pinDests[pin][i] != -1);
}

static void receives(void) {
    
    // For All Nodes
    for (uint32_t y = 0u; y < TRAINSIZE; y++) {
     
        // Perform receives whilever there are messages to be retrieved
        while(ptrMsg[y] > 0u) {
            
            ImpMessage inbndMsg = receiveMessage(y);
            
            // Received Challenge Message
            if (inbndMsg.vote == INIT) {

                
                uint32_t clause = 1u;
                
                // Increment Received Counter
                node[y].recCnt++;
                
                // Calculate Clause 
                
                for (uint32_t x = 0u; clause && x < NOOFLITERALS; x++) {
                         
                    // If ta > n, include literal in clause calculation
                    if (INCLUDE_LITERAL(node[y].ta[x])) {
                         
                        clause &= (inbndMsg.dpX >> (31u - x)) & 1u;
                             
                     }
                     
                }
                
                // Update Local Clause Monitor
                uint8_t wordNo = 0u;
                uint32_t tempSrcClause = inbndMsg.srcClause;
                
                // Find array index and remainder
                while(tempSrcClause >= 32u){
                        tempSrcClause = tempSrcClause - 32u;
                        wordNo++;
                }
                
                if (clause == 1u) { 
                    
                    node[y].clauseOutput[wordNo] |= (1u << tempSrcClause);
                    
                    // Indicate Response
                    node[y].rdyFlags[wordNo] |= (1u << tempSrcClause);
                    
                    // Call send to correct exterior vote count
                    readyToSend[y] = Pin[inbndMsg.srcClause];
        
                }
                /*else {
                    
                    s->clauseOutput[wordNo] &= ~(1u << tempSrcClause);
                    
                }*/
                
            }
            else if (inbndMsg.vote == CHALLENGE) {

                // Seperate cSums and dpY from msg->dpY JPM this will need changing for values liable to exceed 8 bits
                uint8_t dpY = inbndMsg.dpY & 0xFF;
                
                float classSum = (float)((inbndMsg.dpY >> (8u * (node[y].classNo + 1u))) & 0xFF);
                
                // Limit Class Sum Between Thresholds 
                if(classSum<-THRESH)
                    classSum = -THRESH;
                else if(classSum>THRESH)
                    classSum = THRESH;
                
                // Determine if received message is from same class
                uint8_t same = (dpY == node[y].classNo) ? 1u : 0u;
                    
                uint8_t clause = 1;
                uint8_t inc = 0u;
                // calculate conjunction over k literals
                // (we can stop early if output becomes false)
                for(uint32_t k=0; clause && k<NOOFLITERALS; k++) {
                    if(INCLUDE_LITERAL(node[y].ta[k])) {
                        uint8_t lit = (inbndMsg.dpX >> (k/2u)) & 1u;
                        clause &= LITERAL_VALUE(lit, k);
                        inc = 1;
                    }
                }
                if(0u && !inc) //JPM THIS CODE APPEARS TO DO NOTHING
                    clause = 0;
                
                // inverse the decision for negatively-voting clauses
                uint8_t negClause;
                
                if(node[y].clauseNo & 1u)
                    negClause = !same;
                else
                    negClause = same;
                
                if(negClause) {
                    
                    // Type I Feedback
                    
                    // Generate random number and calculate feedback probability
                    node[y].lfsr = randomNum(node[y].lfsr);
                    float randNo = node[y].lfsr / RANDMAX;
                    float feedbackProbability = (THRESH - classSum) / (2.0f * THRESH);
                    
                    
                    if(randNo < feedbackProbability) {
                            for(uint32_t k=0; k<NOOFLITERALS; k++) {
                                
                                uint8_t lit = (inbndMsg.dpX >> (k/2u)) & 1u;
                                
                                if(clause && LITERAL_VALUE(lit, k)) { // clause is 1 and literal is 1
                                    node[y].lfsr = randomNum(node[y].lfsr);
                                    randNo = node[y].lfsr / RANDMAX;
                                    if(randNo < ((1.0-1.0/LEARNINGSENSITIVITY))) {
                                        // Promote literal
                                        if (node[y].ta[k] < (NUMSTATES - 1u)) {
                                            node[y].ta[k]++;
                                        }
                                    }
                                }
                                else { // clause is 0 or literal is 0
                                    node[y].lfsr = randomNum(node[y].lfsr);
                                    randNo = node[y].lfsr / RANDMAX;
                                    if(randNo < ((1.0/LEARNINGSENSITIVITY))) {
                                        // Demote literal
                                        if (node[y].ta[k] > 0u) {
                                            node[y].ta[k]--;
                                        } 
                                    }
                                }
                                
                            }
                    }
                }
                else {
                    
                    // Type II Feedback
                    node[y].lfsr = randomNum(node[y].lfsr);
                    float randNo = node[y].lfsr / RANDMAX;
                    float feedbackProbability = (THRESH + classSum) / (2.0f * THRESH);
                    
                    if(randNo < feedbackProbability) {
                            // only if clause is 1
                        if(clause) {
                            for(uint32_t k=0; k<NOOFLITERALS; k++) {
                                    uint8_t lit = (inbndMsg.dpX >> (k/2u)) & 1u;
                                    if(!LITERAL_VALUE(lit, k) && !INCLUDE_LITERAL(node[y].ta[k])) {// if literal is 0 and excluded
                                        // Promote literal
                                        if (node[y].ta[k] < (NUMSTATES - 1u)) {
                                            node[y].ta[k]++;
                                        }
                                    }
                            }
                        }
                    }
                }
                
                uint8_t newClause = 1u;
                uint8_t newInc = 0u;
                // calculate conjunction over k literals
                // (we can stop early if output becomes false)
                for(uint32_t k=0; newClause && k<NOOFLITERALS; k++) {
                    if(INCLUDE_LITERAL(node[y].ta[k])) {
                        uint8_t lit = (inbndMsg.dpX >> (k/2u)) & 1u;
                        newClause &= LITERAL_VALUE(lit, k);
                        newInc = 1;
                    }
                }
                if(0u && !newInc) //JPM THIS CODE APPEARS TO DO NOTHING
                    newClause = 0;
                    
                uint8_t wordNo = 0u;
                uint32_t tempClauseNo = inbndMsg.srcClause;

                // Find array index and remainder
                while(tempClauseNo >= 32u){
                        tempClauseNo = tempClauseNo - 32u;
                        wordNo++;
                }
                
                // Check if clause value has changed compared to the stored value
                if (newClause != ((node[y].clauseOutput[wordNo] >> tempClauseNo) & 1u)) {
                    
                    // Correct Local Clause Array
                    if (newClause == 1u) {
                    
                        node[y].clauseOutput[wordNo] |= (1u << tempClauseNo);
        
                    }
                    else {
                        
                        node[y].clauseOutput[wordNo] &= ~(1u << tempClauseNo);
                        
                    }
                    
                    // Indicate Response
                    node[y].rdyFlags[wordNo] |= (1u << tempClauseNo);
                    
                    // Call send to correct exterior vote count
                    readyToSend[y] = Pin[inbndMsg.srcClause];
                   
                }
                
        }
        else if (inbndMsg.vote < 2u) {
            
            // Update Vote Count
            
            uint32_t sourceClass;
            
            // JPM THIS NEEDS CHANGING IF NOOFCLASSESCHANGES
            if (inbndMsg.srcClause < LOWERCLASSBND) {
                sourceClass = 0u;
            }
            else if (inbndMsg.srcClause >= UPPERCLASSBND) {
                sourceClass = 2u;
            }
            else {
                sourceClass = 1u;
            }
            
            // Determine Source Clause Polarity
            // If odd, -, if even, +
            if (inbndMsg.srcClause & 1u) {
                
                if (inbndMsg.vote == 0u) {
                    // Odd clause, 1->0 = ++
                    node[y].cSum[sourceClass]++;
                    
                }
                else {
                    // Odd clause, 0->1 = --
                    node[y].cSum[sourceClass]--;
                }
                
            }
            else {
                
                if (inbndMsg.vote == 0u) {
                    // Even clause, 1->0 = --
                    node[y].cSum[sourceClass]--;
                    
                }
                else {
                    // Even clause, 0->1 = ++
                    node[y].cSum[sourceClass]++;
                }
                
            }
           
        }
            
            
            
            
            
        }
        // Debug
        printf("%d ", y);
        for (uint32_t w = 0u; w < FLAGWORDS; w++) {
            printf("%x ", node[y].clauseOutput[w]);
        }
        printf("\n");
    }
    
}

static ImpMessage receiveMessage(uint32_t node) {
    
    ImpMessage inbndMsg;
    
    inbndMsg.dpX = message[node][ptrMsg[node] - 1u].dpX;
    inbndMsg.dpY = message[node][ptrMsg[node] - 1u].dpY;
    inbndMsg.srcClause = message[node][ptrMsg[node] - 1u].srcClause;
    inbndMsg.vote = message[node][ptrMsg[node] - 1u].vote;
    
    ptrMsg[node]--;
    
    return inbndMsg;
    
}

static bool messagesToHandle(void) {
 
    bool response = false;
    
    for (uint32_t x = 0u; !(response) && x < NOOFDEVICES; x++) {
        
        if (readyToSend[x] != No) {
            
            response = true;
            
        }
        
    }
    
    return response;
    
}

static uint16_t randomNum(uint16_t lfsr) {

    uint16_t bit;

    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr = ((lfsr >> 1) | (bit << 15));
     
    return lfsr;

}