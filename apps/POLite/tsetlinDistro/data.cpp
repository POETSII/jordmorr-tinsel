// SPDX-License-Identifier: BSD-2-Clause
#include "data.h"
/***************************************************
 * Edit values between here ->
 * ************************************************/

// Input/Output Training Data

// Binarised Inputs - Training

const uint8_t in_train[TRAINSIZE][NOOFFEATURES] {
    
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0},
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,1,0,0,0,1,1,0,0,0},
            
};

// Binarised Output

const uint8_t out_train[TRAINSIZE] {
    
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    
};

// Binarised Inputs - Test

const uint8_t in_test[TESTSIZE][NOOFFEATURES] {
  
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0},//
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0},//
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0},//
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0},//
    {0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0},//
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0},//
    {0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0},//
    {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0},//
    {0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,0,1,1,0,0,0,1,0,0,1,0,0,0,0,0},//
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1},//
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0,1},//
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0,1},//
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0,0},//
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0,1},//
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1},//
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0,0},//
    {0,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1},//
    {0,1,0,0,0,0,1,0,0,0,1,1,0,0,0,1},//
    {0,0,1,1,0,0,0,1,0,0,1,1,0,0,0,1},//
    
};


const uint8_t out_test[TESTSIZE] {
    
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
  
};

/***************************************************
 * <- And here
 * ************************************************/
