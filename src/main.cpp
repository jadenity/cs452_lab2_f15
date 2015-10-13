#include <climits>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include "sieve.hpp"

using namespace std;

int main(int argc, char** argv) {
      
  // Take input for the lower limit of the sieve.
  int lower;
  cout << "Enter a lower limit: ";
  // Only accept positive integers.
  while (!(cin >> lower) || lower < 1) {
    cout << "Make sure to enter a positive integer." << endl;
    // Clear cin and ignore any garbage input.
    cin.clear();
    cin.ignore(INT_MAX, '\n');
    cout << "Enter a lower limit: ";
  }
  
  cout << "  Lower limit: " << lower << endl;

  // Clear cin before continuing.
  cin.clear();
  cin.ignore(INT_MAX, '\n');

  // Take input for the upper limit of the sieve.
  int upper;
  cout << "Enter an upper limit: ";
  // Only accept an upper limit that is no less than the lower limit.
  while (!(cin >> upper) || (upper < lower)) {
    cout << "Make sure to enter a number no less than the lower limit." << endl;
    // Clear cin and ignore any garbage input.
    cin.clear();
    cin.ignore(INT_MAX, '\n');
    cout << "Enter an upper limit: ";
  }

  cout << "  Upper limit: " << upper << endl;
      
  // false means prime, true means composite
  bool* list = sieve(upper);
      
  for (int m = lower; m <= upper; m++) {
    if (!list[m]) cout << m << " ";
  }

  cout << endl;

  delete[] list;
  return 0;
}

