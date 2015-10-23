#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
      
  private:
    const char *host;
    const char *port;
  public:
    Client(const char *host, const char *port);
    const char *getHost();
    const char *getPort();
    void closeSocket();
    int setup();
    void *get_in_addr(struct sockaddr *sa);
    void comm();

};

#endif

