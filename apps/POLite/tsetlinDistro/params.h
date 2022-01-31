// SPDX-License-Identifier: BSD-2-Clause
#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <stdint.h>

// Parameters

/***************************************************
 * Edit values between here ->
 * ************************************************/
 
// No Of Pins
#define POLITE_NUM_PINS (124u)

// Tsetlin Machine Size
//#define NOOFCLAUSES (100u)
#define NOOFFEATURES (16u)
#define NOOFLITERALS (NOOFFEATURES * 2u)
#define TRAINSIZE (120u)
#define TESTSIZE (30u)
#define NOOFCLASSES (3u)
#define CLAUSESPERCLASS (40u)
#define CHALLENGESPERROUND (CLAUSESPERCLASS * 2u)
#define FLAGWORDS (4u)

// Model Values
#define BCASTPROB (0.2f)

// Hyper Parameters
#define LEARNINGSENSITIVITY (3u)
#define THRESH (10.0f)

#define EXCBOUND (99u)
#define INCBOUND (100u)
#define NUMSTATES (200u)

//#define NOOFEPOCHS (500u)
#define NOOFEPOCHS (10u)

#define LOWERCLASSBND (40u)
#define UPPERCLASSBND (80u)

#define RANDMAX (65535.0f) // 0xFFFF

// Pre-processor selectors
#define TMDEBUG (1)

/***************************************************
 * <- And here
 * ************************************************/

#endif
