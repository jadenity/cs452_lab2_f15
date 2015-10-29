#include "Client.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include <cmath>

//using namespace boost::asio;
using namespace std;

// Max # of bytes
#define MAXDATASIZE 1000000

//Constructor
Client::Client(const char *host, const char *port) {
  this->host = host;
  this->port = port;
}

// Only used for printing out IP address
void *Client::get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Gets the client connected to the server
int Client::setup() {
  int status, sockfd;				//error catching variables
  struct addrinfo hints, *servinfo, *p;		//defined in socket include
  char s[INET6_ADDRSTRLEN];			//also part of socket include

  memset(&hints, 0, sizeof hints);  // empty the struct
  hints.ai_family = AF_UNSPEC;      // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets

  // Prepare to connect
  if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  // loop until you find the first socket to connect to
  bool success = false;
  p = servinfo;
  while ((!success) && (p != NULL)) {

    // Attempt to create the socket
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) { // attempt socket creation
      perror("error in client: socket");
      // Move to next possible socket
      p = p->ai_next;
    } else {

      // Connect to the socket. If successful, exit loop
      if (connect(sockfd, p->ai_addr,
            p->ai_addrlen) == -1) { //attempt socket connection, to server in this case
        close(sockfd); 
        perror("error in client: connect");
        // Unsuccessful. Move to next possible socket
        p = p->ai_next;
      } else {
        success = true;
      }
    }
  }

  if (p == NULL) { //connection failure
    fprintf(stderr, "client: failed to connect\n");
    freeaddrinfo(servinfo);
    return 2;
  }

  // inet_ntop converts the IP address into a human-readable form
  // (network to presentation)
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                  s, sizeof s);
  cout << "Client: connecting to " << s << endl << endl;

  // Use pre-existing function to free pointer to addrinfo
  freeaddrinfo(servinfo);

  return sockfd;
}

vector<int> Client::sieve(int sockfd, unsigned long int listMax) {

  // Send listMax to Server
  if (send(sockfd, (const char *)&listMax, sizeof(unsigned long int), 0) == -1) {
    perror("Client: send dataSize");
  }

  //PREPARATION

  int* list = new int[listMax + 1];  //used to create sieveList
  int* sieveList = new int[listMax + 1]; //holds all numbers in list[] that aren't multiples of the 
                                          //first list number in the list, plus 2 helper variables. 
                                          //Starts out empty

  vector<int> masterList; //our return value at the end of the sieve

  // Populate sieveList with numbers 2 through listMax starting at sieveList[1] (not 0)

  for(int i = 2; i < listMax + 1; i++){
    // Place the number 1 left because starting at 2
    // and the 0th index will contain the size of the list
    list[i - 1] = i;
  }
  list[0] = listMax; //the first variable in a sieveList is the size of the list 
                      //(it's the next best thing we could think of next to "list.size()")

  // With a list made, start the sieve
  while(1) {
    if (list[1] > (int)sqrt((double) listMax)) {
      for(int i = 2; i < sieveList[0]+1; i++){ //add all numbers still in sieveList to the masterList
        masterList.push_back(sieveList[i]);
      }
      return masterList; // sieveList has something by now, 
                         // that we did at least one sieve step.
    } else {

      //first, record the first list number in the list received to the masterList
      masterList.push_back(list[1]);

      //do a step of the sieve:
      int j = 1;
      for(int i = 2; i <= list[0]; i++){ //Note: list[1] is prime. we want to check list[2] to the end.

        if(list[i] % list[1] != 0){ //in other words, "if this number is not a multiple of the 
                                    //prime number being checked"
          sieveList[j] = list[i];
          j++;
        }
      } //end sieve step

      // 0th element is size of list
      sieveList[0] = j - 1; //sieveList size. The minus 1 is there because j is ahead by 1

      // Also, record the first list number in the new sieveList into masterList
      masterList.push_back(sieveList[1]);

      // Now the list is the way we want it, send it to the server

      // Send size of list in bytes
      unsigned long int dataSize = htonl(sizeof(int)*(sieveList[0]+1)); // host to network byte-order

      if (send(sockfd, (const char *)&dataSize, sizeof(unsigned long int), 0) == -1) {
        perror("Client: send dataSize");
      }

      if (send(sockfd, sieveList, sizeof(int)*(sieveList[0]+1), 0) == -1) {
        perror("Client: send sieveList");
      }

      cout << "Sent: ";
      printList(sieveList);

      // With the data sent, we'll have to wait for Server to send the data back. Then we continue.
      // Receive data size
      int numbytes;
      if ((numbytes = recv(sockfd, (char *)&dataSize, sizeof(unsigned long int), 0)) == -1) { 
        perror("Client: recv");
        exit(1);
      }
      dataSize = ntohl(dataSize);

      // Receive list
      int totalBytes = 0; // reset numbytes
      bool recvDone = false;
      char* buf = new char[MAXDATASIZE];

      // Loop until dataSize is reached
      while (!recvDone) {
        int currentBytes = 0;
        // Receive a chunk and add it to the buffer
        if ((currentBytes = recv(sockfd, buf+totalBytes, dataSize, 0)) == -1) { 
          perror("Client: recv");
          exit(1);
        }

        // If 0 bytes received, client hung up
        if (currentBytes == 0) {
          recvDone = true;
          perror("client: server closed connection");
        } else {

          totalBytes += currentBytes;

          // Finished when dataSize is reached
          if (totalBytes == dataSize) {
            recvDone = true;
          }
        }
      }

      // Copy the buffer into the list
      memcpy(list, buf, dataSize);

      // received something from client
      if (totalBytes != 0) {
        printf("Recd: ");
        printList(list);
        cout << endl; 
      }

    } // end if/else

  } //end sieve while loop

  delete[] list;
  delete[] sieveList;
}

// Assumes list[0] is length of list
void Client::printList(int *list) {
  int length = list[0];
  if (list[0] <= 5) { // print whole list
    for (int i = 1; i <= length; i++) {
      cout << list[i] << " ";
    }
    cout << endl;
  } else { // print first five + ...
    for (int i = 1; i <= 5; i++) {
      cout << list[i] << " ";
    }
    cout << "..." << endl;
  }
}

const char *Client::getHost() {
  return host;
}

const char *Client::getPort() {
  return port;
}

