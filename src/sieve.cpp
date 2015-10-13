#include <string.h>
#include <cstdlib>
#include <math.h>
#include <iostream>

using namespace std;

bool* sieve(const int upperLimit) {
  int squareRootN = (int) sqrt((double) upperLimit);
  bool *list = new bool[upperLimit + 1];
  memset(list, 0, sizeof(bool) * (upperLimit + 1));

  for (int m = 2; m <= squareRootN; m++) {
    if (!list[m]) {
      for (int k = m * m; k <= upperLimit; k += m) {
        list[k] = true;
      }
    }
  }

  return list;
}
