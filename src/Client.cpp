//#include <boost/asio.hpp>
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

//Global Variables
int sockfd;

//using namespace boost::asio;
using namespace std;

// Max # of bytes
#define MAXDATASIZE 100

//Constructor
Client::Client(const char *host, const char *port) {
  this->host = host;
  this->port = port;
}

// get sockaddr, IPv4 or IPv6:
void *Client::get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * Creates client and does a few things with it.
 * This is the bulk of the code concerning implementing sockets.
 */
int Client::setup() {
  int status;				//error catching variables
  struct addrinfo hints, *servinfo, *p;		//defined in socket include
  char s[INET6_ADDRSTRLEN];			//also part of an include

  memset(&hints, 0, sizeof hints);  // empty the struct
  hints.ai_family = AF_UNSPEC;      // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets

  // Prepare to connect
  if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  /*
 // ORIGINAL LOOP 
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, 
            p->ai_protocol)) == -1) { //attempt socket creation
      perror("error in client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr,
          p->ai_addrlen) == -1) { //attempt socket connection, to server in this case
      close(sockfd); 
      perror("error in client: connect");
      continue;
    }

    break;
  }
 // END ORIGINAL LOOP
 */

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
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo);

  // Connection complete. Begin communication.

  comm();

  return 0;
}

void Client::comm() {
  int numbytes;
  int buf[MAXDATASIZE];
  int nums[6];

  //recieve numbers from server
  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) { 
    //triggers if recieving too much data at once
    perror("recv");
    exit(1);
  }

  printf("client: received");

  int j = 0;
  for(int i = 1; i < buf[0]+1; i++){
    printf(" %d", buf[i]);

    if((buf[i] % 2) == 1){
      nums[j+1] = buf[i]; //for sending numbers back
      j++;
      printf("!");
    }

    if(i+1 >= buf[0]+1){

      nums[0] = j;
    }
  }

  printf("\n");

  for(int i = 0; i < j+1; i++){
    printf(" %d", nums[i]);
  }
  printf("\n");

  //send numbers to server (let's send odd numbers back)
  if (send(sockfd, nums, sizeof(int)*(nums[0]+1), 0) == -1) {
    perror("send");
  }

}

/*int *Client::clientSieve(){
  return primes;
}*/

void Client::closeSocket(){
  close(sockfd);
}

const char *Client::getHost() {
  return host;
}

const char *Client::getPort() {
  return port;
}

