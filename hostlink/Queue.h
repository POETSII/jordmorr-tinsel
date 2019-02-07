// Simple queue implementation (circular buffer)

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <assert.h>

template <typename T> struct Queue {
  int capacity;
  int front, back;
  int size;
  T* data;

  Queue(int n) {
    capacity = n+1;
    data = new T [capacity];
    size = 0;
  }

  inline int space() {
    return (capacity-1-size);
  }

  inline T first() {
    return data[front];
  }

  inline T index(int i) {
    return data[(front+i) % capacity];
  }

  inline void enq(T elem) {
    assert(space() > 0);
    data[back] = elem;
    back++;
    size++;
  }

  inline T deq() {
    assert(size > 0);
    T elem = data[front];
    front++;
    size--;
    return elem;
  }

  inline void drop(int n) {
    front = (front+n) % capacity;
    size = size - n;
  }

  inline bool canEnq() {
    return space() > 0;
  }

  inline bool canDeq() {
    return size > 0;
  }

  ~Queue() {
    delete [] data;
  }
};

#endif
