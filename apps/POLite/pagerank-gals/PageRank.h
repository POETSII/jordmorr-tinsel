// SPDX-License-Identifier: BSD-2-Clause
#ifndef _PAGERANK_H_
#define _PAGERANK_H_

#define POLITE_DUMP_STATS
#define POLITE_COUNT_MSGS
#define NUM_ITERATIONS 5

#include <POLite.h>

struct PageRankMessage {
  // Time step
  uint16_t t;
  // Page rank score for sender at time step t
  float val;
};

struct PageRankState {
  // Current time step of device
  uint16_t t;
  // Count messages sent and received
  uint16_t sent;
  uint16_t received, receivedNext;
  // Current temperature of device
  float acc, accNext;
  // Score for the current timestep
  float score;
  // Fan-in and fan-out for this vertex
  uint16_t fanIn, fanOut;
  // Total number of vertices in the graph
  uint32_t numVertices;
};

struct PageRankDevice : PDevice<PageRankState, None, PageRankMessage> {

  // Called once by POLite at start of execution
  void init() {
    s->score = 1.0/s->numVertices;
    s->acc = 0.0;
    s->accNext = 0.0;
    s->t = 0;
    *readyToSend = Pin(0);
  }

  // We call this on every state change
  inline void change() {
    // Proceed to next time step?
    if (s->sent && s->received == s->fanIn) {
      s->score = 0.15/s->numVertices + 0.85*s->acc;
      if (s->t < NUM_ITERATIONS) {
        s->acc = s->accNext;
        s->received = s->receivedNext;
        s->accNext = s->receivedNext = 0;
        s->sent = 0;
        s->t++;
        *readyToSend = Pin(0);
      }
      else {
        *readyToSend = No;
      }
    }
  }

  // Send handler
  inline void send(volatile PageRankMessage* msg) {
    msg->val = s->score/s->fanOut;
    msg->t = s->t;
    s->sent = 1;
    *readyToSend = No;
    change();
  }

  // Receive handler
  inline void recv(PageRankMessage* msg, None* edge) {
    if (msg->t == s->t) {
      // Receive temperature for this time step
      s->acc += msg->val;
      s->received++;
      change();
    }
    else {
      // Receive temperature for next time step
      s->accNext += msg->val;
      s->receivedNext++;
    }
  }

  // Called by POLite when system becomes idle
  inline bool step() {
    *readyToSend = No;
    return false;
  }

  // Optionally send message to host on termination
  inline bool finish(volatile PageRankMessage* msg) {
    msg->val = s->score;
    return true;
  }
};

#endif
