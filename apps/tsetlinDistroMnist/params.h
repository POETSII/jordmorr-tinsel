// SPDX-License-Identifier: BSD-2-Clause
#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <stdint.h>

// Parameters

/***************************************************
 * Edit values between here ->
 * ************************************************/

// Tsetlin Machine Size
//#define NOOFCLAUSES (100u)
#define NOOFFEATURES (784u)
#define NOOFLITERALS (NOOFFEATURES * 2u)
#define TRAINSIZE (10000u)
#define TESTSIZE (10000u)
#define NOOFCLASSES (10u)
#define CLAUSESPERCLASS (200u)
// Datapoints per node -> TRAINSIZE / NOOFDEVICES -> 10000 / 2000
#define DPPERNODE (5u)
#define CHALLENGESPERROUND (CLAUSESPERCLASS * DPPERNODE * NOOFCLASSES)

// Array Sizes
#define FLAGWORDS (313u)
#define DPWORDS (25u) // NOOFFEATURES / 32

// No Of Pins -> (classes * clausesPerClass) + classes + 1 for the overall broadcast address
#define POLITE_NUM_PINS (2011)
// Total dFlag -> (classes * clausesPerClass * DPPERNODE) + classes + 1 for the overall broadcast address
#define TOTALFLAGS (10011)

// Model Values
#define BCASTPROB (0.2f)

// Hyper Parameters
#define LEARNINGSENSITIVITY (10.0f)
#define THRESH (50.0f)

#define EXCBOUND (99u)
#define INCBOUND (100u)
#define NUMSTATES (200u)

//#define NOOFEPOCHS (500u)
#define NOOFEPOCHS (9u)

#define LOWERCLASSBND (40u)
#define UPPERCLASSBND (80u)

#define RANDMAX (65535.0f) // 0xFFFF

// Threashold for 0/1 pixel decision
#define IMAGE_THRESHOLD 64

// Pre-processor selectors
#define TMDEBUG (1)

/***************************************************
 * <- And here
 * ************************************************/

#endif
