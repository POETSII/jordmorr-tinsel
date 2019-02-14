#ifndef _SSSP_H_
#define _SSSP_H_

#include <POLite.h>

// Vertex state
struct SSSPState {
  // Is this the source vertex?
  bool isSource;
  // The shortest known distance to this vertex
  int32_t dist;
};

// Vertex behaviour
struct SSSPDevice : PDevice<SSSPState,int32_t,int32_t> {
  inline void init() {
    *readyToSend = s->isSource ? Pin(0) : No;
  }
  inline void send(int32_t* msg) {
    *msg = s->dist;
    *readyToSend = No;
  }
  inline void recv(int32_t* dist, int32_t* weight) {
    int32_t newDist = *dist + *weight;
    if (newDist < s->dist) {
      s->dist = newDist;
      *readyToSend = Pin(0);
    }
  }
  inline bool step() { return false; }
  inline bool finish(int32_t* msg) {
    *msg = s->dist;
    return true;
  }
};

#endif