#include <climits>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <sstream>
#include <sys/socket.h>
#include "sieve.hpp"
#include "client.hpp"
#include "server.hpp"

using namespace std;


// Assumes list[0] is the length of list
void printMasterList(vector<int> masterList) {
  int length = masterList.size();

  if (length <= 10) { // print the whole list if less than 10
    for (int i = 0; i < length; i++) {
      cout << masterList[i] << " ";
    }
  } else { // otherwise only print the first and last 5
    for (int i = 0; i < 5; i++) {
      cout << masterList[i] << " ";
    }
    cout << "... ";
    for (int i = length-5; i < length; i++) {
      cout << masterList[i] << " ";
    }
  }
}

int main(int argc, char** argv) {

  //Starting Variables
  const char *port = "9384"; // Stick to using port 9382 for consistency (as opposed to letting the user pick).
  // int upper = 65482; //hardcoded list bounds. Defined up here, both Server and Client knows list bounds
  int upper = 214424; //hardcoded list bounds. Defined up here, both Server and Client knows list bounds
  // int upper = 100; //hardcoded list bounds. Defined up here, both Server and Client knows list bounds

  //Note: Can probably set "upper" through user input later

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
  //const char *port = "9383"; //moved up to the start

  // Attempt to connect to server if *CLIENT*
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
    Client client(host, port);

    cout << "host: " << client.getHost() << endl;
    cout << "port: " << client.getPort() << endl;

    int client_sock = client.setup(); //create client and connection between client and server

    //int upper = 25; //****Hardcoded list bounds. Moved up so both Client and Server know list bounds
    int startList[upper];

    for (int i = 0; i < upper-1; i++) { // end 1 early because starting at 2
      startList[i] = i+2; // always start at 2
    }

    //outputs the original list. Not really useful
    // cout << "startList: ";

    // for (int i = 0; i < upper-1; i++) {
    //   cout << startList[i] << " ";
    // }
    // cout << endl;

    // client.comm(client_sock, upper); //Mostly contained test code at this point
    vector<int> masterList = client.sieve(client_sock, upper);
    cout << "FINAL masterList: ";

    // for (int i = 0; i < masterList.size(); i++) {
    //   cout << masterList.at(i) << " ";
    // }
    // cout << endl;
    printMasterList(masterList);
    cout << endl;
    cout << "masterList.size(): " << masterList.size() << endl;






//The following takes in user input. It's commented out because we're currently hardcoding list bounds.
//That means we always start at 2 and always end at "upper", defined above.

//    // Take input for the lower limit of the sieve.
//    int lower;
//    cout << "Enter a lower limit: ";
//    // Only accept positive integers.
//    while (!(cin >> lower) || lower < 1) {
//      cout << "Make sure to enter a positive integer." << endl;
//      // Clear cin and ignore any garbage input.
//      cin.clear();
//      cin.ignore(INT_MAX, '\n');
//      cout << "Enter a lower limit: ";
//    }
//    
//    cout << "  Lower limit: " << lower << endl;
//
//    // Clear cin before continuing.
//    cin.clear();
//    cin.ignore(INT_MAX, '\n');
//
//    // Take input for the upper limit of the sieve.
//    int upper;
//    cout << "Enter an upper limit: ";
//    // Only accept an upper limit that is no less than the lower limit.
//    while (!(cin >> upper) || (upper < lower)) {
//      cout << "Make sure to enter a number no less than the lower limit." << endl;
//      // Clear cin and ignore any garbage input.
//      cin.clear();
//      cin.ignore(INT_MAX, '\n');
//      cout << "Enter an upper limit: ";
//    }
//
//    cout << "  Upper limit: " << upper << endl;
//
//    // false means prime, true means composite
//    bool* list = sieve(upper);
//        
//    for (int m = lower; m <= upper; m++) {
//      if (!list[m]) cout << m << " ";
//    }
//
//    cout << endl;

    close(client_sock);

//  delete[] list;
    free(host);
  }


  // Connect to client if *SERVER*
  if (cli_or_serv.compare("server") == 0) {

    Server server(port);

    cout << "server port: " << server.getPort() << endl;

    int server_sock = server.setup();

    // server.comm(server_sock);
    server.sieve(server_sock, upper);

    close(server_sock);
  }


  return 0;
}


