#include <cstdlib>
#include <iostream>
#include <string.h>
#include "sieve.hpp"

using namespace std;

int main(int argc, char** argv) {
      
  cout << "Enter a lower limit" << endl;
  int lower = 0;
  cin >> lower;
  cout << "Enter an upper limit" << endl;
  int upper = 0;
  cin >> upper;
      
  bool* list = sieve(upper);
      
  for (int m = lower; m <= upper; m++) {
    if (!list[m]) cout << m << " ";
  }
  
  cout << endl;
  return 0;
}

