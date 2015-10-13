#include <boost/asio.hpp>
#include "Client.hpp"

using namespace boost::asio;

Client::Client(std::string host, int port) {
  this->host = host;
  this->port = port;
}

void Client::connect() {
  io_service io_service;
  ip::tcp::resolver resolver(io_service);
  ip::tcp::resolver::query query(this->host, "daytime");
  //ip::tcp::socket socket();
}

Client::~Client() {

}
                  
