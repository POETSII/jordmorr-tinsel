// SPDX-License-Identifier: BSD-2-Clause
#include <stdint.h>
#include "model.h"
/***************************************************
 * Edit values between here ->
 * ************************************************/

// Cluster Layout

// Intraboard Mailbox Connections per Board 
const uint8_t mailboxPath[TinselMailboxesPerBoard][2u] = { 
    { 0, 0 },
    { 0, 1 },
    { 0, 2 },
    { 0, 3 },
    { 1, 3 },
    { 1, 2 },
    { 1, 1 },
    { 1, 0 },
    { 2, 0 },
    { 2, 1 },
    { 2, 2 },
    { 2, 3 },
    { 3, 3 },
    { 3, 2 },
    { 3, 1 },
    { 3, 0 }
};

// Intrabox Board Connections per Box 
const uint8_t boardPath[(TinselBoardsPerBox - 1u) * NOOFBOXES][2u] = { 
    { 0, 0 },
    { 1, 0 },
    { 2, 0 },
    { 3, 0 },
    { 4, 0 },
    { 5, 0 },
    { 5, 1 },
    { 4, 1 },
    { 3, 1 },
    { 3, 2 },
    { 4, 2 },
    { 5, 2 },
    { 5, 3 },
    { 4, 3 },
    { 3, 3 },
    { 3, 4 },
    { 4, 4 },
    { 5, 4 },
    { 5, 5 },
    { 4, 5 },
    { 3, 5 },
    { 3, 6 },
    { 4, 6 },
    { 5, 6 },
    { 5, 7 },
    { 4, 7 },
    { 3, 7 },
    { 2, 7 },
    { 1, 7 },
    { 0, 7 },
    { 0, 6 },
    { 1, 6 },
    { 2, 6 },
    { 2, 5 },
    { 1, 5 },
    { 0, 5 },
    { 0, 4 },
    { 1, 4 },
    { 2, 4 },
    { 2, 3 },
    { 1, 3 },
    { 0, 3 },
    { 0, 2 },
    { 1, 2 },
    { 2, 2 },
    { 2, 1 },
    { 1, 1 },
    { 0, 1 }
};

/***************************************************
 * <- And here
 * ************************************************/

