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

void Client::setup() {
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof hints);  // empty the struct
  hints.ai_family = AF_INET;        // use IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets

  // Prepare to connect
  status = getaddrinfo(host, port, &hints, &servinfo);

  // servinfo now points to a linked list of 1 or more struct addrinfos
  freeaddrinfo(servinfo);
}

const char *Client::getHost() {
  return host;
}

const char *Client::getPort() {
  return port;
}
