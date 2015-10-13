#include <climits>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include "sieve.hpp"
#include "client.hpp"
#include "server.hpp"

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
      

  // Use port 9382 for consistency
  const char *port = "9382";

  Client *client = new Client("thing-04.cs.uwec.edu", "9382"); 

  cout << "host: " << (*client).getHost() << endl;
  cout << "port: " << (*client).getPort() << endl;


  Server *server = new Server("9382");

  cout << "server port: " << (*server).getPort() << endl;










  // false means prime, true means composite
  bool* list = sieve(upper);
      
  for (int m = lower; m <= upper; m++) {
    if (!list[m]) cout << m << " ";
  }

  cout << endl;

  delete[] list;
  return 0;
}

