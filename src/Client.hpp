#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
      
  private:
    const char *host;
    const char *port;
  public:
    Client(const char *host, const char *port);
    void connect();
    const char *getHost();
    const char *getPort();
    void setup();

};

#endif

