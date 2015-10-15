#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server {

  private:
    const char *port;
  public:
    Server(const char *port);
    void connect();
    const char *getPort();
    void setup();
  
};

#endif

