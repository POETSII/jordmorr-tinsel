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
#define NOOFDEVICES (2000u)
#define MAXMESSAGES (TRAINSIZE)

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
    // Source Data Point
    uint32_t srcDP;
    // Datapoint X
    uint32_t dpX[DPWORDS];
    // Datapoint Y
    uint8_t dpY;
    // cSums
    int8_t cSum[NOOFCLASSES];
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
    uint32_t deviceNo;
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
    //uint32_t ownWordNo;
    // Own Class Index Number
    //uint32_t ownIndex;
    // Own Pin Number
    //uint32_t ownPinNo;
    // DP Counter
    uint32_t dpCnt[2u];
    
    // Example Pool
    // Dataset Datapoint
    uint32_t dpX[DPPERNODE][DPWORDS];
    uint8_t dpY[DPPERNODE];
    // Votes
    int8_t cSum[DPPERNODE][NOOFCLASSES];

    
#ifdef TMDEBUG
    // Test Counter
    uint32_t testCnt;
    uint32_t lastMsg;
#endif
   
};

//***********************
//GLOBAL VARIABLES
//***********************

//struct ImpMessage message[NOOFDEVICES][MAXMESSAGES] = {{ 0u }};
auto message = new ImpMessage [NOOFDEVICES][MAXMESSAGES];
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
const uint32_t BROADCAST = NOOFDEVICES + NOOFCLASSES;
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

const uint32_t INITCHAL = (1 << 26); //1

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
void read_set(int X[][NOOFFEATURES], int y[], int num, const char* images, const char* labels);

//***********************
//MAIN FUNCTION
//***********************
int main(void) {
    
    // Handle Simulator Idiosyncrasies
    // Allocate Message Buffers
    /*
    ImpMessage** message  = NULL;
    if( !(message = new ImpMessage [NOOFDEVICES][MAXMESSAGES] )) {
       printf("Error: out of memory.\n");
       exit(1);
    }
    */
    
    //Read MNIST dataset
    //read_set(in_train, out_train, TRAINSIZE, "train-images-idx3-ubyte", "train-labels-idx1-ubyte");
    
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
    
/*    
    for (uint32_t dig = 0; dig < TESTSIZE; dig++) {
        for (uint32_t val = 0; val < NOOFFEATURES; val++) {
            if (val % 28 == 0) {
                printf("\n");
            }
            printf("%d", in_test[dig][val]);
        }
        printf("\n");
    }
*/    
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
    for (uint32_t i = 0u; i < NOOFDEVICES; i++) {
                
        pinDests[i][0u] = i;
                
    }
    
    uint32_t cnt = 0u;
    
    // Add class specific broadcast addresses
    for (uint32_t i = NOOFDEVICES; i < (NOOFDEVICES + NOOFCLASSES); i++) {
        for (uint32_t j = 0u; j < CLAUSESPERCLASS; j++) {
                
            pinDests[i][j] = cnt;
            cnt++;
            
        }
    }
    
    // Add complete broadcast address for intialisation
    for (uint32_t i = 0u; i < NOOFDEVICES; i++) {
                
        pinDests[(NOOFDEVICES + NOOFCLASSES)][i] = i;
                
    }
    
    // INITIALIZE DEVICES
    for (uint32_t x = 0; x < NOOFDEVICES; x++) {
                
            // Initialise device IDs
            node[x].id = x;
            
            // Assign node it's clause number
            node[x].deviceNo = x;
            
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
            
            
            for (uint32_t z = 0u; z < DPPERNODE; z++) {
            
                uint32_t datapoint[DPWORDS] = { 0u };
                    
                // Accumulate Datapoint In
                for (uint32_t y = 0; y < NOOFFEATURES; y++) {
                    
                    datapoint[y / 32u] = (datapoint[y / 32u] << 1u) + in_test[(x * DPPERNODE) + z][y];
                    
                }
                
                for (uint32_t i = 0u; i < DPWORDS; i++) {
                    
                    node[x].dpX[z][i] = datapoint[i];
                
                }
                
                node[x].dpY[z] = out_test[(x * DPPERNODE) + z];
            }
             
    }
    
}

static void initialiseNodes(void) {
    
    // For All Nodes
    for (uint32_t x = 0; x < NOOFDEVICES; x++) {
        
        // Initialise Epochs
        node[x].epoch = 0u;

        for (uint32_t z = 0u; z < DPPERNODE; z++) {
            
            // Clear cSum Counts
            for (uint32_t y = 0u; y < NOOFCLASSES; y++) {
                
                node[x].cSum[z][y] = 0;
            
            }
        
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
        /*
        node[x].ownPinNo = NOOFDEVICES + node[x].classNo;
        
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
        */
        // Clear datapoint counter
        node[x].dpCnt[0] = 0u;
        node[x].dpCnt[1] = 0u;
        
        readyToSend[x] = No;
        
    }
    
}

static bool step(void) {
    
    bool keepRunning = false;
    
    // For All Nodes
    for (uint32_t x = 0; x < NOOFDEVICES; x++) {
        
        // Clear flags
        for (uint32_t y = 0u; y < FLAGWORDS; y++) {
            
            node[x].rdyFlags[y] = 0u;
            node[x].sentFlags[y] = 0u;
            
        }
        
        // First iteration or re-evalutation requires a global broadcast
        if (node[x].epoch < (NOOFEPOCHS + 1u)) {
            
            // Is this the first epoch? -> Initialisation
            if (node[x].epoch == 0u) {
                
                // Indicate Initial Challenge Broadcast
                node[x].rdyFlags[FLAGWORDS - 1u] |= INITCHAL;
                
                // Broadcast initialisation challenge
                readyToSend[x] = Pin[BROADCAST];
                
            }
            else {
                
                do {
                    // Calculate next negative class
                    node[x].nextNegClass = node[x].nextNegClass + 1u;
                    // Wrap nextNegClass without modulo instruction
                    if (node[x].nextNegClass == NOOFCLASSES) {
                        node[x].nextNegClass = 0u;
                    }
                } while (node[x].nextNegClass == node[x].classNo);
                
                uint32_t ownClassWord = 0u;
                uint32_t ownClassIndex = TRAINSIZE + node[x].classNo;
                
                // Find array index and remainder
                while(ownClassIndex >= 32u){
                        ownClassIndex = ownClassIndex - 32u;
                        ownClassWord++;
                }
                
                // Mark own class flag
                node[x].rdyFlags[ownClassWord] |= (1u << ownClassIndex);
                
                uint32_t nextClassWord = 0u;
                uint32_t nextClassIndex = TRAINSIZE + node[x].nextNegClass;

                // Find array index and remainder
                while(nextClassIndex >= 32u){
                        nextClassIndex = nextClassIndex - 32u;
                        nextClassWord++;
                }
                
                // Mark next non-matching class multicast Flag
                node[x].rdyFlags[nextClassWord] |= (1u << nextClassIndex);
                
                // Calculate pin number for next neg class
                
                uint32_t negPinNo = NOOFDEVICES + node[x].nextNegClass;
                
                // Broadcast initialisation challenge
                readyToSend[x] = Pin[negPinNo];
                
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
    for (uint32_t y = 0; y < NOOFDEVICES; y++) {
        
        // Check Whether node has any messages to send
        while(readyToSend[y] != No) {
            
            // Outbound Message
            ImpMessage outbndMsg;
        
            // Find Pin
            for (uint32_t x = 0u; x < POLITE_NUM_PINS; x++) {
                
                if (readyToSend[y] == Pin[x]) {
                    
                    // Is this a unicast message?
                    if (x < NOOFDEVICES) {
                        
                        // Look through DP's allcoated to that device for messages to send
                        uint32_t lclDPCnt = 0u;
                        bool dpFound = false;
                        
                        while((lclDPCnt < DPPERNODE) && !(dpFound)) {
                        
                            uint32_t flagWord = 0u;
                            uint32_t flagBit = (x * DPPERNODE) + lclDPCnt;

                            // Find array index and remainder
                            while(flagBit >= 32u){
                                    flagBit = flagBit - 32u;
                                    flagWord++;
                            }
                            
                            // Check if not already sent
                            if ((node[y].rdyFlags[flagWord] & (1 << flagBit)) && !(node[y].sentFlags[flagWord] & (1u << flagBit))) {
                            
                                // Construct Response
                                
                                // Source DP contains only clause number
                                outbndMsg.srcDP = node[y].deviceNo;
                                
                                // Find Vote
                                outbndMsg.vote = (((node[y].clauseOutput[flagWord]) >> flagBit) & 1u);
                                
                                
                                outbndMsg.dpY = lclDPCnt;
                                
                                // Remaining message variables redundant
                                for (uint32_t z = 0u; z < DPWORDS; z++) {
                                    
                                    outbndMsg.dpX[z] = 0u;
                                
                                }
                                
                                for (uint32_t z = 0u; z < NOOFCLASSES; z++) {
                                    
                                    outbndMsg.cSum[z] = 0;
                                
                                }
                                
                                
                                // Mark Correct Sent Flag
                                node[y].sentFlags[flagWord] |= (1u << flagBit);
                                
                                // Indicate a message was found
                                dpFound = true;
                            
                            }
                            
                            // Increment local DP counter
                            lclDPCnt++;
                        
                        }
                        
                    }
                    else {
                        // This is a request to broadcast a challange
                        
                        // Calculate Flag Values
                        uint32_t wordNo = 0u;
                        uint32_t bitNo = TRAINSIZE + (x - NOOFDEVICES);

                        // Find array index and remainder
                        while(bitNo >= 32u){
                                bitNo = bitNo - 32u;
                                wordNo++;
                        }
                        
                        // Check if not already sent
                        if (!(node[y].sentFlags[wordNo] & (1u << bitNo))) {
                        
                            if (node[y].epoch == 1u) {
                                outbndMsg.vote = INIT;
                            }
                            else {
                                outbndMsg.vote = CHALLENGE;
                            }
                            
                            uint32_t cntIndex;
                        
                            if (node[y].epoch == 1u || node[y].classNo == (x - NOOFDEVICES)) {
                                cntIndex = 0u;
                            }
                            else {
                                cntIndex = 1u;
                            }
                                
                            outbndMsg.srcDP = (node[y].deviceNo * DPPERNODE) + node[y].dpCnt[cntIndex];
                            
                            for (uint32_t z = 0u; z < DPWORDS; z++) {
                                
                                outbndMsg.dpX[z] = node[y].dpX[node[y].dpCnt[cntIndex]][z];
                            
                            }
                            
                            outbndMsg.dpY = node[y].dpY[node[y].dpCnt[cntIndex]];
                            
                            for (uint32_t z = 0u; z < NOOFCLASSES; z++) {
                                
                                outbndMsg.cSum[z] = node[y].cSum[node[y].dpCnt[cntIndex]][z];
                            
                            }
                            
                            node[y].dpCnt[cntIndex]++;

                            if (node[y].dpCnt[cntIndex] == DPPERNODE) {
                                
                                // Mark Correct Sent Flag
                                node[y].sentFlags[wordNo] |= (1u << bitNo);
                                
                                //Reset DP Counter
                                node[y].dpCnt[cntIndex] = 0u;
                            
                            }
                        
                        }
                        
                    }
                
                    // Call simulated message handler
                    sendMessage(outbndMsg, Pin[x]);
                }
                
            }
            
            // Check for other messages to be sent
            
            uint8_t more = 0u;

            for (uint32_t x = 0u; x < TOTALFLAGS; x++) {
                
                uint32_t wordNo = 0u;
                uint32_t index = x;

                // Find array index and remainder
                while(index >= 32u){
                        index = index - 32u;
                        wordNo++;
                }
                
                if ( (node[y].rdyFlags[wordNo] & (1 << index)) && !(node[y].sentFlags[wordNo] & (1 << index)) ) {
                    
                    
                    // Determine Pin Number from flag number
                    uint32_t pinNo = 0u;
                    
                    if (x < TRAINSIZE) {
                        // Unicast Pin
                        
                        uint32_t index = x;

                        // Find array index and remainder
                        while(index >= 5u){
                                index = index - 5u;
                                pinNo++;
                        }
                        
                    }
                    else {
                        // Broadcast Pin
                        
                        pinNo = NOOFDEVICES + (x - TRAINSIZE);
                    }
                    
                    readyToSend[y] = Pin[pinNo];
                    more = 1u;
                    break;
                    
                }
                
            }
                
            // If there are no more messages to send
            if (!more)  {
                readyToSend[y] = No;
                printf("Node %d sent\n", y);
            }
        } 
    }
    
}

static void sendMessage(struct ImpMessage outbndMsg, uint32_t pin) {
 
    uint32_t i = 0u;
    
    do {
        // Copy Outbound Message to Destinations
        for (uint32_t x = 0u; x < DPWORDS; x++) {
            message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].dpX[x] = outbndMsg.dpX[x];
        }
        for (uint32_t x = 0u; x < NOOFCLASSES; x++) {
            message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].cSum[x] = outbndMsg.cSum[x];
        }
        message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].dpY = outbndMsg.dpY;
        message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].srcDP = outbndMsg.srcDP;
        message[pinDests[pin][i]][ptrMsg[pinDests[pin][i]]].vote = outbndMsg.vote;
        
        // Increment Counter
        ptrMsg[pinDests[pin][i]]++;
        i++;
        
    }
    while (pinDests[pin][i] != -1);
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


static void receives(void) {
    
    // For All Nodes
    for (uint32_t y = 0u; y < NOOFDEVICES; y++) {
     
        // Perform receives whilever there are messages to be retrieved
        while(ptrMsg[y] > 0u) {
            
            ImpMessage inbndMsg = receiveMessage(y);
            
            // Received Challenge Message
            if (inbndMsg.vote == INIT) {

                
                uint32_t clause = 1u;
                
                // Calculate Clause 
                
                for (uint32_t x = 0u; clause && x < NOOFLITERALS; x++) {
                    
                    // Update Local Clause Monitor
                    uint32_t dpWord = 0u;
                    uint32_t dpBit = x;
                    
                    // Find array index and remainder
                    while(dpBit >= 32u){
                            dpBit = dpBit - 32u;
                            dpWord++;
                    }
                         
                    // If ta > n, include literal in clause calculation
                    if (INCLUDE_LITERAL(node[y].ta[x])) {
                         
                        uint8_t lit = (inbndMsg.dpX[dpWord] >> (dpBit >> 1u)) & 1u;
                        clause &= LITERAL_VALUE(lit, x);
                             
                     }
                     
                }
                
                if (clause == 1u) { 
                    
                    // Update Local Clause Monitor
                    uint32_t flagWord = 0u;
                    uint32_t flagBit = inbndMsg.srcDP;
                    
                    // Find array index and remainder
                    while(flagBit >= 32u){
                            flagBit = flagBit - 32u;
                            flagWord++;
                    }
                    
                    node[y].clauseOutput[flagWord] |= (1u << flagBit);
                    
                    // Indicate Response
                    node[y].rdyFlags[flagWord] |= (1u << flagBit);
                    
                    uint32_t pinNo = 0u;
                    uint32_t index = inbndMsg.srcDP;

                    // Find array index and remainder
                    while(index >= 5u){
                            index = index - 5u;
                            pinNo++;
                    }
                    
                    // Call send to correct exterior vote count
                    readyToSend[y] = Pin[pinNo];
        
                }

                
            }
            else if (inbndMsg.vote == CHALLENGE) {
                
                printf("Node %d received challenge for DP %d\n", y, inbndMsg.srcDP);
                
                float classSum = (float)(inbndMsg.cSum[node[y].classNo]);
                
                // Limit Class Sum Between Thresholds 
                if(classSum<-THRESH)
                    classSum = -THRESH;
                else if(classSum>THRESH)
                    classSum = THRESH;
                
                // Determine if received message is from same class
                uint8_t same = (inbndMsg.dpY == node[y].classNo) ? 1u : 0u;
                    
                uint8_t clause = 1;
                uint8_t inc = 0u;
                // calculate conjunction over k literals
                // (we can stop early if output becomes false)
                for (uint32_t x = 0u; clause && x < NOOFLITERALS; x++) {
                    
                    // Update Local Clause Monitor
                    uint32_t dpWord = 0u;
                    uint32_t dpBit = x;
                    
                    // Find array index and remainder
                    while(dpBit >= 32u){
                            dpBit = dpBit - 32u;
                            dpWord++;
                    }
                         
                    // If ta > n, include literal in clause calculation
                    if (INCLUDE_LITERAL(node[y].ta[x])) {
                         
                        uint8_t lit = (inbndMsg.dpX[dpWord] >> (dpBit >> 1u)) & 1u;
                        clause &= LITERAL_VALUE(lit, x);
                        inc = 1;     
                     }
                     
                }
                /*
                for(uint32_t k=0; clause && k<NOOFLITERALS; k++) {
                    if(INCLUDE_LITERAL(node[y].ta[k])) {
                        uint8_t lit = (inbndMsg.dpX >> (k >> 1u)) & 1u;
                        clause &= LITERAL_VALUE(lit, k);
                        inc = 1;
                    }
                }
                */
                if(0u && !inc) //JPM THIS CODE APPEARS TO DO NOTHING
                    clause = 0;
                
                // inverse the decision for negatively-voting clauses
                uint8_t negClause;
                
                if(node[y].deviceNo & 1u)
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
                            
                            uint32_t dpWord = 0u;
                            uint32_t dpBit = k;
                            
                            // Find array index and remainder
                            while(dpBit >= 32u){
                                    dpBit = dpBit - 32u;
                                    dpWord++;
                            }
                            
                            uint8_t lit = (inbndMsg.dpX[dpWord] >> (dpBit >> 1u)) & 1u;
                            
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
                                
                                uint32_t dpWord = 0u;
                                uint32_t dpBit = k;
                                
                                // Find array index and remainder
                                while(dpBit >= 32u){
                                        dpBit = dpBit - 32u;
                                        dpWord++;
                                }
                                    
                                uint8_t lit = (inbndMsg.dpX[dpWord] >> (dpBit >> 1u)) & 1u;
                                
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
                for (uint32_t x = 0u; newClause && x < NOOFLITERALS; x++) {
                    
                    // Update Local Clause Monitor
                    uint32_t dpWord = 0u;
                    uint32_t dpBit = x;
                    
                    // Find array index and remainder
                    while(dpBit >= 32u){
                            dpBit = dpBit - 32u;
                            dpWord++;
                    }
                         
                    // If ta > n, include literal in clause calculation
                    if (INCLUDE_LITERAL(node[y].ta[x])) {
                         
                        uint8_t lit = (inbndMsg.dpX[dpWord] >> (dpBit >> 1u)) & 1u;
                        newClause &= LITERAL_VALUE(lit, x);
                        newInc = 1;     
                     }
                     
                }
                if(0u && !newInc) //JPM THIS CODE APPEARS TO DO NOTHING
                    newClause = 0;
                    
                uint32_t flagWord = 0u;
                uint32_t flagBit = inbndMsg.srcDP;

                // Find array index and remainder
                while(flagBit >= 32u){
                        flagBit = flagBit - 32u;
                        flagWord++;
                }
                
                // Check if clause value has changed compared to the stored value
                if (newClause != ((node[y].clauseOutput[flagWord] >> flagBit) & 1u)) {
                    
                    // Correct Local Clause Array
                    if (newClause == 1u) {
                    
                        node[y].clauseOutput[flagWord] |= (1u << flagBit);
        
                    }
                    else {
                        
                        node[y].clauseOutput[flagWord] &= ~(1u << flagBit);
                        
                    }
                    
                    // Indicate Response
                    node[y].rdyFlags[flagWord] |= (1u << flagBit);
                    
                    uint32_t pinWord = 0u;
                    uint32_t pinBit = inbndMsg.srcDP;
                    
                    // Find array index and remainder
                    while(pinBit >= 5u){
                            pinBit = pinBit - 5u;
                            pinWord++;
                    }
                    
                    // Call send to correct exterior vote count
                    readyToSend[y] = Pin[pinWord];
                   
                }
                
            }
            else if (inbndMsg.vote < 2u) {
                
                printf("Node %d received update from clause %d\n", y, inbndMsg.srcDP);
                
                // Update Vote Count
                
                // Determine Source Class
                uint32_t sourceClass = 0u;
                uint32_t x = 1u;
                
                while(inbndMsg.srcDP >= (x * CLAUSESPERCLASS)) {
                    
                    sourceClass++;
                    x++;
                    
                }
                
                // Determine Source Clause Polarity
                // If odd, -, if even, +
                if (inbndMsg.srcDP & 1u) {
                    
                    if (inbndMsg.vote == 0u) {
                        // Odd clause, 1->0 = ++
                        node[y].cSum[inbndMsg.dpY][sourceClass]++;
                        
                    }
                    else {
                        // Odd clause, 0->1 = --
                        node[y].cSum[inbndMsg.dpY][sourceClass]--;
                    }
                    
                }
                else {
                    
                    if (inbndMsg.vote == 0u) {
                        // Even clause, 1->0 = --
                        node[y].cSum[inbndMsg.dpY][sourceClass]--;
                        
                    }
                    else {
                        // Even clause, 0->1 = ++
                        node[y].cSum[inbndMsg.dpY][sourceClass]++;
                    }
                    
                }
               
            }
            
            
        }
/*        
        // Debug
        printf("%d ", y);
        for (uint32_t w = 0u; w < FLAGWORDS; w++) {
            printf("%x ", node[y].clauseOutput[w]);
        }
        printf("\n");

*/        

        //printf("Node %d received all messages\n", y);

    }
    
}

static ImpMessage receiveMessage(uint32_t node) {
    
    ImpMessage inbndMsg;
    
    for (uint32_t x = 0u; x < DPWORDS; x++) {
        inbndMsg.dpX[x] = message[node][ptrMsg[node] - 1u].dpX[x];
    }
    for (uint32_t x = 0u; x < NOOFCLASSES; x++) {
        inbndMsg.cSum[x] = message[node][ptrMsg[node] - 1u].cSum[x];
    }
    inbndMsg.dpY = message[node][ptrMsg[node] - 1u].dpY;
    inbndMsg.srcDP = message[node][ptrMsg[node] - 1u].srcDP;
    inbndMsg.vote = message[node][ptrMsg[node] - 1u].vote;
    
    ptrMsg[node]--;
    
    return inbndMsg;
    
}

static uint16_t randomNum(uint16_t lfsr) {

    uint16_t bit;

    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr = ((lfsr >> 1) | (bit << 15));
     
    return lfsr;

}
/*
void read_set(int X[][NOOFFEATURES], int y[], int num, const char* images, const char* labels)
{
	FILE * fp;
	int skip;
	unsigned char b;
	
	fp = fopen(images, "r");
	if (fp == NULL) {
		printf("Error opening %s\n", images);
		exit(EXIT_FAILURE);
	}
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < NOOFFEATURES; j++) {
			fread(&b, 1, 1, fp);
			X[i][j] = b > IMAGE_THRESHOLD ? 1 : 0;
		}
	}
	fclose(fp);
	
	fp = fopen(labels, "r");
	if (fp == NULL) {
		printf("Error opening %s\n", labels);
		exit(EXIT_FAILURE);
	}
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	for (int i = 0; i < num; i++) {
		fread(&b, 1, 1, fp);
		y[i] = (int) b;
	}
	fclose(fp);
}
*/