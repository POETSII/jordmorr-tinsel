// SPDX-License-Identifier: BSD-2-Clause
#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <stdint.h>

// Parameters

/***************************************************
 * Edit values between here ->
 * ************************************************/

// Tsetlin Machine Size
#define NOOFCLAUSES (150u)
#define NOOFLITERALS (32u)
#define TRAINSIZE (120u)
#define TESTSIZE (30u)
#define NOOFCLASSES (3u)

// Model Values
#define BCASTPROB (0.2f)

// Hyper Parameters
#define LEARNINGSENSITIVITY (3u)
#define THRESH (10.0f)

#define LOWERBOUND (0u)
#define EXCBOUND (49u)
#define INCBOUND (50u)
#define UPPERBOUND (99u)

#define NOOFEPOCHS (500u)

#define NOOFINPUTLIT (32u)

#define LOWERCLASSBND (40u)
#define UPPERCLASSBND (80u)

#define RANDMAX (0xFFFF)

// Pre-processor selectors
#define TMDEBUG (1)

/***************************************************
 * <- And here
 * ************************************************/

#endif
