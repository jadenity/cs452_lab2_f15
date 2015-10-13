#include "Server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <cstring>

using namespace std;

Server::Server(const char *port) {
  this->port = port;
  setup();
}

void Server::connect() {

}

void Server::setup() {
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof hints);  // empty hints
  hints.ai_family = AF_INET;        // use IPv4
  hints.ai_socktype = SOCK_STREAM;  // use TCP stream sockets
  hints.ai_flags = AI_PASSIVE;      // fill in the server's IP automatically

  if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
}

const char *Server::getPort() {
  return port;
}
                                                              
