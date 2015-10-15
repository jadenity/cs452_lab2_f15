//#include <boost/asio.hpp>
#include "Client.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <cstring>

//using namespace boost::asio;
using namespace std;

Client::Client(const char *host, const char *port) {
  this->host = host;
  this->port = port;
  setup();
}

void Client::connect() {

}

int Client::setup() {
  int sockfd, status, numbytes;
  struct addrinfo hints, *servinfo, *p;
  char s[INET6_ADDRSTRLEN];
  char buf[100];

  memset(&hints, 0, sizeof hints);  // empty the struct
  hints.ai_family = AF_INET;        // use IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets

  // Prepare to connect
  if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  // loolp until you find the first socket to connect to
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("error in client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd); 
      perror("error in client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                  s, sizeof s);
  printf("client: connecting to %s\n", s);

  // servinfo now points to a linked list of 1 or more struct addrinfos
  freeaddrinfo(servinfo);

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    perror("recv");
    exit(1);
  }

  buf[numbytes] = '\0';

  printf("client: received '%s'\n",buf);

  close(sockfd);

  return 0;
}

const char *Client::getHost() {
  return host;
}

const char *Client::getPort() {
  return port;
}
