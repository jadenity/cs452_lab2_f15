#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
      
  private:
    std::string host;
    int port;    
  public:
    Client(std::string host, int port);
    void connect();
    virtual ~Client();

};

#endif

