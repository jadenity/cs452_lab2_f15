#include <climits>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <sstream>
#include "sieve.hpp"
#include "client.hpp"
#include "server.hpp"

using namespace std;

int main(int argc, char** argv) {

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
  // Use port 9382 for consistency
  const char *port = "9383";

  // Attempt to connect to server if client
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

    // Prepare the thing hostname
    const char *begin = "thing-0";
    const char *end = ".cs.uwec.edu";
    char *host;
    host = (char *)malloc(strlen(begin)+strlen(end)+2);
    strcpy(host, begin);
    strcat(host, argv[2]);
    strcat(host, end);

    // Make the const char * out of the concatenated host string
    //const char *host_c = (host.str()).c_str();
    Client *client = new Client(host, port);

    cout << "host: " << (*client).getHost() << endl;
    cout << "port: " << (*client).getPort() << endl;

    (*client).setup(); //create client and connection between client and server

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

    (*client).closeSocket();

    delete[] list;
    delete client;
    free(host);
  }


  // Connect to client if server
  if (cli_or_serv.compare("server") == 0) {

    Server *server = new Server(port);

    cout << "server port: " << (*server).getPort() << endl;


    (*server).setup();


    delete server;
  }


  return 0;
}

