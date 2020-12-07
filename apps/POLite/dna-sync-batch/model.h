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

extern const uint8_t hmm_labels[NOOFSTATES][NOOFOBS];
extern const uint32_t observation[NOOFOBS][2];
extern const float dm[NOOFOBS-1];

#endif
