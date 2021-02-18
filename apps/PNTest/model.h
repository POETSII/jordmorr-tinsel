// SPDX-License-Identifier: BSD-2-Clause
#ifndef _MODEL_H_
#define _MODEL_H_

//#include <stdint.h>
#include "imputation.h"
// Parameters

/***************************************************
 * Edit values between here ->
 * ************************************************/

// Pre-processor Switches
//#define PRINTDIAG (1)
extern const uint32_t place[NOOFPLACES];
extern const uint32_t netlist[NOOFELEMENTS][7u];
extern const uint8_t mailboxPath[TinselMailboxesPerBoard][2u];
extern const uint8_t boardPath[(TinselBoardsPerBox - 1u) * NOOFBOXES][2u];

/***************************************************
 * <- And here
 * ************************************************/

#endif

