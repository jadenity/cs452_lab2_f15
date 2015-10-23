#include "server.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>

//Global Variables
int new_fd;

using namespace std;

#define BACKLOG 10 // # of pending connections in the queue
// Max # of bytes
#define MAXDATASIZE 100

Server::Server(const char *port) {
  this->port = port;
}

// get sockaddr, IPv4 or IPv6:
void *Server::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Server::setup() {
  int sockfd, status;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);  // empty hints
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;  // use TCP stream sockets
  hints.ai_flags = AI_PASSIVE;      // fill in the server's IP automatically

  if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
 
  // ORIGINAL LOOP
  // loop through all the results and bind to the first we can
  // for (p = servinfo; p != NULL; p = p->ai_next) {
  //     if ((sockfd = socket(p->ai_family, p->ai_socktype,
  //             p->ai_protocol)) == -1) {
  //         perror("server: socket");
  //         continue;
  //     }

  //     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
  //             sizeof(int)) == -1) {
  //         perror("setsockopt");
  //         exit(1);
  //     }

  //     if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
  //         close(sockfd);
  //         perror("server: bind");
  //         continue;
  //     }

  //     break;
  // }
  // END ORIGINAL LOOP

  // loop through all the results and bind to the first we can
  bool success = false;
  p = servinfo;
  while ((!success) && p != NULL) { 
    // Attempt to create the socket
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
      perror("server: socket");
      // Move to next possible socket
      p = p->ai_next;
    } else {

      // Set socket options
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
             sizeof(int)) == -1) {
         perror("setsockopt");
      }

      // Bind to the socket. If successful, exit loop
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        // Move to next possible socket 
        p = p->ai_next;
      } else {
        success = true;
      }
    }
  }

  // Use pre-existing function to free pointer to addrinfo
  freeaddrinfo(servinfo);

  if (p == NULL)  {
      fprintf(stderr, "server: failed to bind\n");
      exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
      perror("listen");
      exit(1);
  }


  printf("server: waiting for connections...\n");

  sin_size = sizeof their_addr;
  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
  if (new_fd == -1) {
      perror("accept");
  }

  // inet_ntop converts the IP address into a human-readable form
  // (network to presentation)
  inet_ntop(their_addr.ss_family,
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s);
  printf("server: got connection from %s\n", s);

  // Want to use threads instead of forking child processes...

  // Separate communication out once connection is established.
  comm(new_fd);

  close(sockfd); 
  return 0;
}

//mostly for testing sending data between server and client
void Server::comm(int new_fd) {
  int numbytes;
  int buf[MAXDATASIZE];
  int nums[6] = {5, 1, 2, 3, 4, 5}; //first value is list length, the rest are the actual numbers in the list

  cout << "Sending";

  for(int i = 0; i < (sizeof (nums))/(sizeof (nums[0])); i++){
    cout << " " <<nums[i];
  }

  cout << "..." << endl;

  if (send(new_fd, nums, sizeof(int)*6, 0) == -1) {
    perror("send");
  }

  cout << "Waiting to receive..." << endl;

  //the above sends data to the client. The following recieves data from the client.

  while (numbytes != 0) {

    if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(1);
    }

    // received something from client
    if (numbytes != 0) {
      printf("recieved:");
      for(int i = 1; i < (buf[0]+1); i++){
        printf(" %d", buf[i]);
      }
      printf("\n");
    } else {
      perror("server: client closed connection");
    }

  }

  // Make sure to close file descriptors when finished with them.
  close(new_fd);
}

const char *Server::getPort() {
  return port;
}

void Server::closeSocket() {
  close(new_fd);
}
                                                              
