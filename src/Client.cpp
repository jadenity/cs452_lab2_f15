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
  int sockfd, status, numbytes;			//error catching variables
  struct addrinfo hints, *servinfo, *p;		//defined in 
  char s[INET6_ADDRSTRLEN];			//??
  char buf[MAXDATASIZE];			//used for message output

  memset(&hints, 0, sizeof hints);  // empty the struct
  hints.ai_family = AF_INET;        // use IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets

  // Prepare to connect
  if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  // loop until you find the first socket to connect to
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) { //attempt socket creation
      perror("error in client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) { //attempt socket connection, to server in this case
      close(sockfd); 
      perror("error in client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) { //connection failure
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                  s, sizeof s);
  printf("client: connecting to %s\n", s);

  // servinfo now points to a linked list of 1 or more struct addrinfos
  freeaddrinfo(servinfo);

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) { //triggers if recieving too much data at once (?)
    perror("recv");
    exit(1);
  }

  buf[numbytes] = '\0';

  printf("client: received '%s'\n",buf);

  // cout << "Client sleeping..." << endl; //TESTING: make sure the the connection continues after recieving a message from the server

  // sleep(10);

  close(sockfd); //closes socket connection. Seems to end the server as well.

  return 0;
}

const char *Client::getHost() {
  return host;
}

const char *Client::getPort() {
  return port;
}
