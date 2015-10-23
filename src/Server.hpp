#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server {

  private:
    const char *port;
  public:
    Server(const char *port);
    const char *getPort();
    int setup();
    void *get_in_addr(struct sockaddr *sa);
    void closeSocket();
    void comm();
  
};

#endif

