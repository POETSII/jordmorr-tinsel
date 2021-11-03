// SPDX-License-Identifier: BSD-2-Clause
#ifndef _MODEL_H_
#define _MODEL_H_

#include "params.h"

// Parameters

/***************************************************
 * Edit values between here ->
 * ************************************************/

// Moved to params.h

/***************************************************
 * <- And here
 * ************************************************/

extern const uint8_t in_train[TRAINSIZE][NOOFINPUTLIT];
extern const uint8_t out_train[TRAINSIZE];
extern const uint8_t in_test[TESTSIZE][NOOFINPUTLIT];
extern const uint8_t out_test[TESTSIZE];

#endif
