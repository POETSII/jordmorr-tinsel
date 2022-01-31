/*****************************************************
 * Tsetlin Machine - Single Thread Version for Iris Dataset
 * ***************************************************
 * USAGE:
 * To Be Completed ...
 * 
 * PLEASE NOTE:
 * Binarised Iris Dataset From -> https://github.com/cair/TsetlinMachine
 * 
 * ssh jordmorr@ayres.cl.cam.ac.uk
 * scp -r C:\Users\drjor\Documents\tinsel\apps\tsetlinSingleIris jordmorr@ayres.cl.cam.ac.uk:~/tinsel/apps
 * scp jordmorr@ayres.cl.cam.ac.uk:~/tinsel/apps/tsetlinSingleIris/results.csv C:\Users\drjor\Documents\tinsel\apps\tsetlinSingleIris
 * ****************************************************/
 
 

#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/*****************************************************
* Pre-processor Definitions / Global Declarations
* ***************************************************/

#define PENALTY (0u)
#define REWARD (1u)

/*****************************************************
* Function Prototypes
* ***************************************************/

static void update_ta(uint8_t* ta, uint8_t action);

/*****************************************************
* Global Variables
* ***************************************************/

// Define Tsetlin Automata
uint8_t ta[NOOFCLASSES][NOOFCLAUSESPERCLASS][NOOFINPUTLIT] = {0u};

/*****************************************************
* Main Function
* ***************************************************/

int main() {
    
    // INITIALISATION
    
    // Seed random number generator
    time_t t;
    srand((unsigned) time(&t));
    
    // DEFINE TSETLIN MACHINE
    
    // Initialise automata states close to active/inactive boundary
    
    for (uint32_t i = 0u; i < NOOFCLASSES; i++) {
        for (uint32_t j = 0u; j < NOOFCLAUSESPERCLASS; j++) {
            for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
            
                double random = (double)rand() / (double)RAND_MAX;
                
                if (random <= 0.5f) {
                    ta[i][j][k] = EXCBOUND;
                }
                else {
                    ta[i][j][k] = INCBOUND;
                }
            
            }
        }
    }
    
    // Define clauses
    
    uint8_t clause[NOOFCLASSES][NOOFCLAUSESPERCLASS] = {0u};
    
    // Define Counter for printing
    uint32_t epochCnt = 1u;
    
    // Start timer for mesh creation and mapping
    struct timeval start_train, finish_train, diff_train;
    gettimeofday(&start_train, NULL);
    
    // TRAIN TSETLIN MACHINE
    
    // Calculate Clauses
    
    for (uint32_t t = 0u; t < (TRAININGEPOCHS * TRAINSIZE); t++) {
        
        uint32_t set = rand() % TRAINSIZE;
        
        // Calculate Clauses
        
        for (uint32_t i = 0u; i < NOOFCLASSES; i++) {
        
            for (uint32_t j = 0u; j < NOOFCLAUSESPERCLASS; j++) {
                     
                 uint8_t tempClause = 1u;
                 
                 for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                     
                    // If ta > n, include literal in clause calculation
                    if (ta[i][j][k] >= INCBOUND) {
                         
                        tempClause &= in_train[set][k] & 0x1;
                             
                     }
                     
                 }
                 
                 clause[i][j] = tempClause;
                 
            }
        
        }
        
        
        // Calculate clause total
    
        int32_t classConfidence[NOOFCLASSES] = {0};
        
        for (uint32_t i = 0u; i < NOOFCLASSES; i++) {
        
            for (uint32_t j = 0u; j < NOOFCLAUSESPERCLASS; j++) {
                
                // Is the clause 'active'
                if (clause[i][j] == 1u) {
                    
                    // If clause is odd, increment clause total, if even, decrement clause total
                    if (j % 2u != 0u) {
                        
                        classConfidence[i]++;
                        
                    }
                    else {
                        
                        classConfidence[i]--;
                        
                    }
                    
                }
                
            }
        
        }
        
        // Change the Y output into a one-hot encoding
        
        uint8_t outputClass[3u] = {0u};
        
        outputClass[out_train[set]] = 1u;
        
        
        // Provide feedback
        for (uint32_t i = 0u; i < NOOFCLASSES; i++) {
            
            for (uint32_t j = 0u; j < NOOFCLAUSESPERCLASS; j++) {
                
                    // If clause is odd, Cj1. else Cj0
                    if (j % 2u != 0u) {
                        
                        //Cj1
                        
                        // If the output y is 1
                        if (outputClass[i] == 1u) {
                            
                            //Provide TypeI Feedback
                            double random = (double)rand() / (double)RAND_MAX;
                
                            if (random < ((THRESH - classConfidence[i])/(2.0f * THRESH))) { 
                                
                                // Is the clause 'active'
                                if (clause[i][j] == 1u) {
                                    
                                    // Provide TypeI feedback for Cj1
                                    for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                                        
                                        // If literal is 1
                                        if (in_train[set][k] == 1u) {
                                            
                                            // Literal is 1
                                            
                                            // Is the literal included in the clause?
                                            if (ta[i][j][k] >= INCBOUND) {
                                                
                                                // Literal is included
                                                
                                                double random = (double)rand() / (double)RAND_MAX;
                                                
                                                if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                    
                                                    // High probability action -> reward
                                                    update_ta(&ta[i][j][k], REWARD);                                                                   //C1, L1, I1
                                                    
                                                }
                                                else {
                                                    
                                                    // Low probability action -> inaction                                                   
                                                    
                                                }
                                                
                                                
                                            }
                                            else {
                                                
                                                double random = (double)rand() / (double)RAND_MAX;
                                                
                                                if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                    
                                                    // High probability action -> penalty
                                                    update_ta(&ta[i][j][k], PENALTY);                                                                   //C1, L1, I0
                                                    
                                                }
                                                else {
                                                    
                                                    // Low probability action -> inaction
                                                    
                                                }
                                                
                                            }
                                            

                                        }
                                        else {
                                            
                                            // Literal is 0
                                            
                                            // Is the literal included in the clause?
                                            if (ta[i][j][k] >= INCBOUND) {
                                                
                                                // Literal is included -> This case should never be valid                                           //C1, L0, I1
                                            
                                                    
                                            }
                                            else {
                                                
                                                // Literal is excluded
                                                
                                                double random = (double)rand() / (double)RAND_MAX;

                                                if (random >= 1.0f/LEARNINGSENSITIVITY) {

                                                    // High probability action -> inaction                                                          
                                                    
                                                }
                                                else {
                                                    
                                                    // Low probability action -> reward
                                                    update_ta(&ta[i][j][k], REWARD);                                                                   //C1, L0, I0
                                                    
                                                }
                                                
                                            }
                                            
                                        }
                                        
                                    }
                                    
                                }
                                else {
                                    
                                        // Clasue is 'inactive'    
                                        for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                                            
                                            // If literal is 1
                                            if (in_train[set][k] == 1u) {
                                                
                                                // Literal is 1
                                                
                                                // Is the literal included in the clause?
                                                if (ta[i][j][k] >= INCBOUND) {
                                                    
                                                    // Literal is included
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                                                                                          
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> penalty
                                                        update_ta(&ta[i][j][k], PENALTY);                                                                  //C0, L1, I1
                                                        
                                                    }
                                                    
                                                    
                                                }
                                                else {
                                                    
                                                    // Literal is excluded
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> reward
                                                        update_ta(&ta[i][j][k], REWARD);                                                                   //C0, L1, I0
                                                        
                                                    }
                                                    
                                                }
                                                
                                            }
                                            else {
                                                
                                                // Literal is 0
                                                
                                                // Is the literal included in the clause?
                                                if (ta[i][j][k] >= INCBOUND) {
                                                 
                                                    // Literal is included
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                                                                                          
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> penalty
                                                        update_ta(&ta[i][j][k], PENALTY);                                                                  //C0, L0, I1
                                                        
                                                    }
                                                    
                                                        
                                                    
                                                }
                                                else {
                                                    
                                                    // Literal is excluded
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                                                                                          
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> reward
                                                        update_ta(&ta[i][j][k], REWARD);                                                                  //C0, L0, I0
                                                        
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
                            double random = (double)rand() / (double)RAND_MAX;
                
                            if (random < ((THRESH + classConfidence[i])/(2.0f * THRESH))) {
    
                                // Only one combination requires feedback -> C1 L0 I0
                                if (clause[i][j] == 1u) {
                                    
                                    for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                                        
                                        // If literal is 0
                                        if (in_train[set][k] == 0u) {
                                         
                                            // Is the literal excluded from the clause?
                                            if (ta[i][j][k] <= EXCBOUND) {
                                             
                                                // Provide penalty feedback
                                                update_ta(&ta[i][j][k], PENALTY);                                                                  //C1, L0, I0
                   
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
                        if (outputClass[i] == 1u) {
                            
                            // Provide TypeII Feedback for Cj0
                            double random = (double)rand() / (double)RAND_MAX;
                
                            if (random < ((THRESH + classConfidence[i])/(2.0f * THRESH))) {
    
                                // Only one combination requires feedback -> C1 L0 I0
                                if (clause[i][j] == 1u) {
                                    
                                    for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                                        
                                        // If literal is 0
                                        if (in_train[set][k] == 0u) {
                                         
                                            // Is the literal excluded from the clause?
                                            if (ta[i][j][k] <= EXCBOUND) {
                                             
                                                // Provide penalty feedback
                                                update_ta(&ta[i][j][k], PENALTY);                                                                  //C1, L0, I0
                   
                                            }
                   
                                        }
                   
                                    }
                                    
                                }

                            }
                            
                        }
                        else {
                            
                            // Output y is 0
                            
                            
                            // Provide TypeI Feedback
                            double random = (double)rand() / (double)RAND_MAX;
                
                            if (random < ((THRESH - classConfidence[i])/(2.0f * THRESH))) { 
                                
                                // Is the clause 'active'
                                if (clause[i][j] == 1u) {
                                    
                                    // Provide TypeI feedback for Cj1
                                    for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                                        
                                        // If literal is 1
                                        if (in_train[set][k] == 1u) {
                                            
                                            // Literal is 1
                                            
                                            // Is the literal included in the clause?
                                            if (ta[i][j][k] >= INCBOUND) {
                                                
                                                // Literal is included
                                                
                                                double random = (double)rand() / (double)RAND_MAX;
                                                
                                                if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                    
                                                    // High probability action -> reward
                                                    update_ta(&ta[i][j][k], REWARD);                                                                   //C1, L1, I1
                                                    
                                                }
                                                else {
                                                    
                                                    // Low probability action -> inaction                                                   
                                                    
                                                }
                                                
                                                
                                            }
                                            else {
                                                
                                                double random = (double)rand() / (double)RAND_MAX;
                                                
                                                if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                    
                                                    // High probability action -> penalty
                                                    update_ta(&ta[i][j][k], PENALTY);                                                                   //C1, L1, I0
                                                    
                                                }
                                                else {
                                                    
                                                    // Low probability action -> inaction
                                                    
                                                }
                                                
                                            }
                                            

                                        }
                                        else {
                                            
                                            // Literal is 0
                                            
                                            // Is the literal included in the clause?
                                            if (ta[i][j][k] >= INCBOUND) {
                                                
                                                // Literal is included -> This case should never be valid                                           //C1, L0, I1
                                            
                                                    
                                            }
                                            else {
                                                
                                                // Literal is excluded
                                                
                                                double random = (double)rand() / (double)RAND_MAX;

                                                if (random >= 1.0f/LEARNINGSENSITIVITY) {

                                                    // High probability action -> inaction                                                          
                                                    
                                                }
                                                else {
                                                    
                                                    // Low probability action -> reward
                                                    update_ta(&ta[i][j][k], REWARD);                                                                   //C1, L0, I0
                                                    
                                                }
                                                
                                            }
                                            
                                        }
                                        
                                    }
                                    
                                }
                                else {
                                    
                                        // Clasue is 'inactive'    
                                        for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                                            
                                            // If literal is 1
                                            if (in_train[set][k] == 1u) {
                                                
                                                // Literal is 1
                                                
                                                // Is the literal included in the clause?
                                                if (ta[i][j][k] >= INCBOUND) {
                                                    
                                                    // Literal is included
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                                                                                          
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> penalty
                                                        update_ta(&ta[i][j][k], PENALTY);                                                                  //C0, L1, I1
                                                        
                                                    }
                                                    
                                                    
                                                }
                                                else {
                                                    
                                                    // Literal is excluded
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> reward
                                                        update_ta(&ta[i][j][k], REWARD);                                                                   //C0, L1, I0
                                                        
                                                    }
                                                    
                                                }
                                                
                                            }
                                            else {
                                                
                                                // Literal is 0
                                                
                                                // Is the literal included in the clause?
                                                if (ta[i][j][k] >= INCBOUND) {
                                                 
                                                    // Literal is included
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                                                                                          
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> penalty
                                                        update_ta(&ta[i][j][k], PENALTY);                                                                  //C0, L0, I1
                                                        
                                                    }
                                                    
                                                        
                                                    
                                                }
                                                else {
                                                    
                                                    // Literal is excluded
                                                    
                                                    double random = (double)rand() / (double)RAND_MAX;
                                                    
                                                    if (random >= 1.0f/LEARNINGSENSITIVITY) {
                                                        
                                                        // High probability action -> inaction
                                                                                                                          
                                                        
                                                    }
                                                    else {
                                                        
                                                        // Low probability action -> reward
                                                        update_ta(&ta[i][j][k], REWARD);                                                                  //C0, L0, I0
                                                        
                                                    }
                                                    
                                                    
                                                    
                                                }
                                                
                                            }
                                            
                                        }    
                                            
                                    
                                    
                                }
                                
                            }
                            
                            
                        }
                        
                    }
                
            }
            
        }
        
        
        // Code for breakpoint debugging of training epochs JPM
        if (t % 120u == 0u) {
            volatile uint32_t check = 0u;
/*            
            char ep[4];
            sprintf(ep, "%d", epochCnt);
            char* extension = ".csv";
            char fileSpec[strlen(ep)+strlen(extension)+1];
            
            //Create a file pointer
            FILE *fp;
            
            snprintf( fileSpec, sizeof( fileSpec ), "%s%s", ep, extension );
            
            // open the file for writing
            fp = fopen (fileSpec,"w");
            
            for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                for (uint32_t i = 0u; i < NOOFCLASSES; i++) {
                    for (uint32_t j = 0u; j < NOOFCLAUSESPERCLASS; j++) {
                        
                        if ((i == (NOOFCLASSES - 1u)) && (j == (NOOFCLAUSESPERCLASS - 1u))) {
                            
                            fprintf(fp, "%d", ta[i][j][k]);
                            
                        }
                        else {
                            
                            fprintf(fp, "%d,", ta[i][j][k]);
                            
                        }

                    }
                }
                
                fprintf(fp, "\n");
                
            }
            
            epochCnt++;
            
            // close the file 
            fclose (fp);
*/            
        }
        
        
    }
        
    // Record train time
    gettimeofday(&finish_train, NULL);
    timersub(&finish_train, &start_train, &diff_train);
    double train_duration = (double) diff_train.tv_sec + (double) diff_train.tv_usec / 1000000.0;    
    
    // TEST TSETLIN MACHINE
    
    uint32_t correctCount = 0u;
    
    for (uint32_t set = 0u; set < TESTSIZE; set++) {
        
        // Calculate Clauses
        
        for (uint32_t i = 0u; i < NOOFCLASSES; i++) {
        
            for (uint32_t j = 0u; j < NOOFCLAUSESPERCLASS; j++) {
                     
                 uint8_t tempClause = 1u;
                 
                 for (uint32_t k = 0u; k < NOOFINPUTLIT; k++) {
                     
                    // If ta > n, include literal in clause calculation
                    if (ta[i][j][k] >= INCBOUND) {
                         
                        tempClause &= in_test[set][k] & 0x1;
                             
                     }
                     
                 }
                 
                 clause[i][j] = tempClause;
                 
            }
        
        }
        
        
        // Calculate clause total
    
        int32_t classConfidence[NOOFCLASSES] = {0};
        
        for (uint32_t i = 0u; i < NOOFCLASSES; i++) {
        
            for (uint32_t j = 0u; j < NOOFCLAUSESPERCLASS; j++) {
                
                // Is the clause 'active'
                if (clause[i][j] == 1u) {
                    
                    // If clause is odd, increment clause total, if even, decrement clause total
                    if (j % 2u != 0u) {
                        
                        classConfidence[i]++;
                        
                    }
                    else {
                        
                        classConfidence[i]--;
                        
                    }
                    
                }
                
            }
        
        }
        
        // Derive Predicted Output Class
        
        uint8_t predictedClass = 0u;
        
        for (uint32_t i = 1u; i < NOOFCLASSES; i++) {
            
            if (classConfidence[i] > classConfidence[predictedClass]) {
                
                predictedClass = i;
                
            }
            
        }
        
        // Check if predicted class is correct
        if (predictedClass == out_test[set]) {
            
            correctCount++;
            
        }
        
        
    }
    
    // Calculate Accuracy of Tsetlin Machine
    double accuracy = (double)correctCount / TESTSIZE;
    
    printf("Current Accuracy = %f\n", accuracy);
    
    printf("Train Duration = %lf \n", train_duration);
    
    
       
    return 0;
}

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