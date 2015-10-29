#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
      
  private:
    const char *host;
    const char *port;
  public:
    Client(const char *host, const char *port);
    const char *getHost();
    const char *getPort();
    int setup();
    void *get_in_addr(struct sockaddr *sa);
    // int *comm(int sockfd, int listMax);
    std::vector<int> sieve(int sockfd, unsigned long int listMax);
    void printList(int *list);

};

#endif

