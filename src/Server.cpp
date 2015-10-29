#include "server.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <cmath>

using namespace std;

#define BACKLOG 10 // # of pending connections in the queue
// Max # of bytes
#define MAXDATASIZE 1000000

Server::Server(const char *port) {
  this->port = port;
}

// Only used for printing out IP address
void *Server::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Gets the server connected to the client
int Server::setup() {
  int sockfd, status, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);  // empty hints
  hints.ai_family = AF_UNSPEC;      // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // use TCP stream sockets
  hints.ai_flags = AI_PASSIVE;      // fill in the server's IP automatically

  if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  // loop through all the results and bind to the first we can
  bool success = false;
  p = servinfo;
  while ((!success) && p != NULL) { 
    // Attempt to create the socket
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
      perror("server: socket");
      // Move to next possible socket
      p = p->ai_next;
    } else {

      // Set socket options
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
             sizeof(int)) == -1) {
         perror("setsockopt");
      }

      // Bind to the socket. If successful, exit loop
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        // Unsuccessful. Move to next possible socket 
        p = p->ai_next;
      } else {
        success = true;
      }
    }
  }

  // Use pre-existing function to free pointer to addrinfo
  freeaddrinfo(servinfo);

  // If loop exited because p is null, bind failed
  if (p == NULL)  {
      fprintf(stderr, "server: failed to bind\n");
      exit(1);
  }

  // Wait for connection
  if (listen(sockfd, BACKLOG) == -1) {
      perror("listen");
      exit(1);
  }

  printf("server: waiting for connections...\n");

  // Accept connection (final step)
  sin_size = sizeof their_addr;
  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
  if (new_fd == -1) {
      perror("accept");
  }

  // inet_ntop converts the IP address into a human-readable form
  // (network to presentation)
  inet_ntop(their_addr.ss_family,
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s);
  printf("server: got connection from %s\n\n", s);

  // sockfd not needed anymore, make sure to close it
  close(sockfd);

  // Return the new socket file descriptor after accepting
  return new_fd;
}

int Server::sieve(int sockfd){

  int listMax;
  int numbytes = 1;
  if ((numbytes = recv(sockfd, (char *)&listMax, sizeof(unsigned long int), 0)) == -1) { 
    perror("Server: recv listMax");
    exit(1);
  }

  if (numbytes == 0) {
    // perror("Server: client closed connection");
    return 0;
  }

  //PREPARATION
  int* list = new int[listMax + 1];  //list recieved from Client
  int* sieveList = new int[listMax + 1]; //list we'll be giving back to client

  //Server is a bit different. It has to wait to get some data before doing anything instead of doing something then sending data and waiting for more
  //The while loop also starts right away because the only thing going on in here is either detection that the sieve is over or doing a sieve step
  numbytes = 1;
  while(1){
    // Receive data size of list in bytes
    numbytes = 0; // reset numbytes
    unsigned long int dataSize;
    if ((numbytes = recv(sockfd, (char *)&dataSize, sizeof(unsigned long int), 0)) == -1) { 
      perror("Server: recv");
      exit(1);
    }
    dataSize = ntohl(dataSize);

    // Receive list
    bool recvDone = false;
    char* buf = new char[MAXDATASIZE];
    int totalBytes = 0;

    // Loop until dataSize is reached
    while (!recvDone) {
      int currentBytes = 0;
      // Receive a chunk and add it to the buffer
      if ((currentBytes = recv(sockfd, buf+totalBytes, dataSize, 0)) == -1) { 
        perror("Server: recv");
        exit(1);
      }

      // If 0 bytes received, client hung up
      if (currentBytes == 0) {
        recvDone = true;
        return 0;
      } else {

        totalBytes += currentBytes;

        // Finished when dataSize is reached
        if (totalBytes >= dataSize) {
          recvDone = true;
        }
      }
    }

    // Copy the buffer into the list
    memcpy(list, buf, dataSize);

    // Received something from client
    if (numbytes != 0) {
      cout << "Recd: ";
      printList(list);
    } else {
      // If 0 bytes received, client hung up
      return 0;
    }

    // Only check up to the square root of the upper limit.
    int squareRootMax = (int)sqrt((double) listMax);
    if(list[1] > squareRootMax){ 
      // If all the numbers have been checked up to the square root, 
      // just send back the entire list to the client.

      // Send size of list in bytes
      unsigned long int dataSize = htonl(sizeof(int)*(list[0]+1)); // host to network byte-order
      if (send(sockfd, (const char *)&dataSize, sizeof(unsigned long int), 0) == -1) {
        perror("Server: send list dataSize");
      }
      
      // Send list
      if (send(sockfd, list, sizeof(int)*(list[0]+1), 0) == -1) {
        perror("Server: send list");
      }

      // Indicate last iteration
      cout << "Sent: ";
      printList(list);

    } else {

      // Do a step of the sieve (just like in Client):
      int j = 1;
      for(int i = 2; i <= list[0]; i++){ //reminder: list[0] is where the size of the actual list is stored
        // Add the current number to the new list only if it's not divisible
        // by the number that's being checked for divisibility
        if(list[i] % list[1] != 0){
          sieveList[j] = list[i];
          // Increment the 
          j++;
        }
      } // End sieve step

      // Add helper variables to the list
      sieveList[0] = j - 1; //sieveList size

      // Now the list is the way we want it, send it to the client

      // Send size of list in bytes
      unsigned long int dataSize = htonl(sizeof(int)*(sieveList[0]+1)); // host to network byte-order
      if (send(sockfd, (const char *)&dataSize, sizeof(unsigned long int), 0) == -1) {
        perror("Server: send sieveList dataSize");
      }

      // Send list
      if (send(sockfd, sieveList, sizeof(int)*(sieveList[0]+1), 0) == -1) {
        perror("Server: send sieveList");
      }

      // Output sent list
      cout << "Sent: ";
      printList(sieveList);
      cout << endl;

    } // end if/else

  } // end while loop

  // Delete allocated arrays
  delete[] list;
  delete[] sieveList;
}

// Assumes list[0] is length of list
void Server::printList(int *list) {
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

const char *Server::getPort() {
  return port;
}
                                                              
