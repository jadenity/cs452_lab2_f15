#include <string.h>
#include <cstdlib>
#include <math.h>
#include <iostream>

using namespace std;

bool* sieve(const int upperLimit) {

  int squareRootN = (int) sqrt((double) upperLimit);
  // Using a boolean list allows prime = false, composite = true.
  bool *list = new bool[upperLimit + 1];
  // Clear out the memory for the list.
  memset(list, 0, sizeof(bool) * (upperLimit + 1));

  // 1 is not prime by definition.
  list[1] = true;
  // Only go up to the square root of the upper limit.
  for (int m = 2; m <= squareRootN; m++) {
    // Start at the next composite number.
    if (!list[m]) {
      // Mark all multiples of m as composite.
      for (int k = m * m; k <= upperLimit; k += m) {
        list[k] = true;
      }
    }
  }

  return list;
}
