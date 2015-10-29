#include <climits>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <sstream>
#include <sys/socket.h>
#include "client.hpp"
#include "server.hpp"

using namespace std;

// Prints all primes
void printMasterListAll(vector<int> masterList, int start) {
  // Find starting number location
  int startLocation = 0;
  int i = 0;
  while (start > masterList[i]) {
    startLocation = i+1;
    i++;
  }

  int length = masterList.size();
  for (int i = startLocation; i < length; i++) {
    cout << masterList[i] << " ";
  }
}

// Prints short list of primes
void printMasterListShort(vector<int> masterList, int start) {
  int length = masterList.size();

  // Find starting number location
  int startLocation = 0;
  int i = 0;
  while (start > masterList[i]) {
    startLocation = i+1;
    i++;
  }

  // cout << "length-10: " << (length-10) << endl;
  // cout << "startLocation: " << startLocation << endl;
  if (startLocation >= (length-10)) { // Print the whole list if less than 10

    for (int i = startLocation; i < length; i++) {
      cout << masterList[i] << " ";
    }

  } else { // Otherwise only print the first and last 5

    for (int i = startLocation; i < (startLocation+5); i++) {
      cout << masterList[i] << " ";
    }

    cout << "... ";

    for (int i = length-5; i < length; i++) {
      cout << masterList[i] << " ";
    }

  }
}

int main(int argc, char** argv) {

  // Starting Variables
  // Use port 9382 for consistency
  const char *port = "9382"; // Stick to using port 9382 for consistency (as opposed to letting the user pick).

  /* PREPARATION */

  // Validate # of arguments
  if (argc < 2) {
    cout << "Missing client/server argument." << endl;
    return 1;
  }

  // Validate first argument
  string cli_or_serv = argv[1];
  if (!((cli_or_serv.compare("client") == 0)
        || (cli_or_serv.compare("server") == 0))) {
    cout << "Enter 'client' or 'server' for the 1st argument." << endl;
    return 1;
  }

  /* END PREPARATION */

  // Separate Client and Server

  // Most of the preparation work is done in the client
  if (cli_or_serv.compare("client") == 0) {

    // Make sure thing# is entered
    if (argc < 3) {
      cout << "Enter a thing# (4-6) for the 2nd argument." << endl;
      return 1;
    }

    // Validate second argument
    int thing = atoi(argv[2]);
    if (thing < 4 || thing > 6) {
      cout << "Enter a thing# between 4 and 6 for the 2nd argument." << endl;
      return 1;
    }

    // Take input for the print method of the sieve.
    char printInput;
    cout << "Print all primes? (y/n): ";
    // Only accept positive integers.
    cin >> printInput;
    while ((char)printInput != 'y' && (char)printInput != 'n') {
      cout << "Enter 'y' or 'n'." << endl;
      cout << "Print all primes? (y/n): ";
      cin >> printInput;
    }

    bool printAll = false;
    if (printInput == 'y') {
      printAll = true;
    }

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
    cout << endl;

    // Prepare the thing hostname
    const char *begin = "thing-0";
    const char *end = ".cs.uwec.edu";
    char *host;
    host = (char *)malloc(strlen(begin)+strlen(end)+2);
    // Put the beginning in, then concatenate the argument and the end
    strcpy(host, begin);
    strcat(host, argv[2]);
    strcat(host, end);

    // Make the client
    Client client(host, port);

    cout << "host: " << client.getHost() << endl;
    cout << "port: " << client.getPort() << endl;

    int client_sock = client.setup(); //create client and connection between client and server

    vector<int> masterList = client.sieve(client_sock, upper);
    if (printAll) {
      cout << "ALL PRIMES: " << endl;
      printMasterListAll(masterList, lower);
      cout << endl;
    } else {
      cout << "PRIMES: " << endl;
      printMasterListShort(masterList, lower);
      cout << endl;
    }

    // Close the socket before exiting
    close(client_sock);

    // Free the host pointer
    free(host);
  }


  // Connect to client if *SERVER*
  if (cli_or_serv.compare("server") == 0) {

    Server server(port);

    cout << "server port: " << server.getPort() << endl;
    cout << endl;

    int server_sock = server.setup();

    server.sieve(server_sock);

    close(server_sock);
  }


  return 0;
}


