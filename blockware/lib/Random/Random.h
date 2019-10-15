// Not sure why I need #ifndef only in this file.
#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

static inline double random()
{
  // Use current time as seed for random generator
  return (double)rand() / (float)RAND_MAX;
}

static inline void randomSeed()
{
  // initialize srand
  randomSeed(ESP.getCycleCount());
}

#endif
