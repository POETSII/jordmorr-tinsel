// SPDX-License-Identifier: BSD-2-Clause
#ifndef _MODEL_H_
#define _MODEL_H_

#include <stdint.h>

// Parameters

/***************************************************
 * Edit values between here ->
 * ************************************************/

// Model Parameters
#define TRAINSIZE (120u)
#define TESTSIZE (30u)
#define TRAININGEPOCHS (500u)

// Hyper Parameters
#define LEARNINGSENSITIVITY (3u)
#define THRESH (10.0f)
#define NOOFINPUTLIT (32u)
#define NOOFCLAUSES (960u)
#define NOOFCLASSES (3u)
#define NOOFCLAUSESPERCLASS (320u)
#define NOOFSTATES (100u)

#define LOWERBOUND (0u)
#define EXCBOUND (49u)
#define INCBOUND (50u)
#define UPPERBOUND (99u)

// Pre-processor Switches
//#define PRINTDIAG (1)

/***************************************************
 * <- And here
 * ************************************************/

extern const uint8_t in_train[TRAINSIZE][NOOFINPUTLIT];
extern const uint8_t out_train[TRAINSIZE];
extern const uint8_t in_test[TESTSIZE][NOOFINPUTLIT];
extern const uint8_t out_test[TESTSIZE];

#endif

