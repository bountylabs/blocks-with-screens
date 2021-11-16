// Not sure why I need #ifndef only in this file.
#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

static inline float randomf()
{
  // Use current time as seed for random generator
  return (float)rand() / (float)RAND_MAX;
}

static inline float time_random(const int min, const float max)
{
  return min + (randomf() * (max - min));
}

static inline float time_random(const float max)
{
    return time_random(0.0f, max);
}

static inline float time_random(const int min, const int max)
{
  return min + (randomf() * (max - min));
}

#endif
