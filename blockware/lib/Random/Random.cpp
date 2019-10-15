#include <stdlib.h>

double random()
{
  // Use current time as seed for random generator
  return (double)rand() / (float)RAND_MAX;
}
