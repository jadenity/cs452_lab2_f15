//#include <boost/asio.hpp>
#include "Client.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include <cmath>

//Global Variables
// int sockfd;

//using namespace boost::asio;
using namespace std;

// Max # of bytes
#define MAXDATASIZE 10000000

//Constructor
Client::Client(const char *host, const char *port) {
  this->host = host;
  this->port = port;
}

// get sockaddr, IPv4 or IPv6:
void *Client::get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * Creates client and does a few things with it.
 * This is the bulk of the code concerning implementing sockets.
 */
int Client::setup() {
  int status, sockfd;				//error catching variables
  struct addrinfo hints, *servinfo, *p;		//defined in socket include
  char s[INET6_ADDRSTRLEN];			//also part of an include

  memset(&hints, 0, sizeof hints);  // empty the struct
  hints.ai_family = AF_UNSPEC;      // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets

  // Prepare to connect
  if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  /*
 // ORIGINAL LOOP 
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, 
            p->ai_protocol)) == -1) { //attempt socket creation
      perror("error in client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr,
          p->ai_addrlen) == -1) { //attempt socket connection, to server in this case
      close(sockfd); 
      perror("error in client: connect");
      continue;
    }

    break;
  }
 // END ORIGINAL LOOP
 */

  // loop until you find the first socket to connect to
  bool success = false;
  p = servinfo;
  while ((!success) && (p != NULL)) {

    // Attempt to create the socket
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) { // attempt socket creation
      perror("error in client: socket");
      // Move to next possible socket
      p = p->ai_next;
    } else {

      // Connect to the socket. If successful, exit loop
      if (connect(sockfd, p->ai_addr,
            p->ai_addrlen) == -1) { //attempt socket connection, to server in this case
        close(sockfd); 
        perror("error in client: connect");
        // Unsuccessful. Move to next possible socket
        p = p->ai_next;
      } else {
        success = true;
      }
    }
  }

  if (p == NULL) { //connection failure
    fprintf(stderr, "client: failed to connect\n");
    freeaddrinfo(servinfo);
    return 2;
  }

  // inet_ntop converts the IP address into a human-readable form
  // (network to presentation)
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                  s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo);

  // Connection complete. Begin communication.

  // comm();

  return sockfd;
}

//we need this to actually do the sieve, not just do a thing and that's it.
// int *Client::comm(int sockfd, int listMax) {
//   int numbytes;
//   int buf[MAXDATASIZE];
//   vector<int> newList;

//   //recieve numbers from server
//   if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) { 
//     //triggers if recieving too much data at once
//     perror("recv");
//     exit(1);
//   }

//   printf("client: received\n");

//   int total = 0;
//   for(int i = 1; i < buf[0]+1; i++){
//     printf(" %d", buf[i]);

//     if((buf[i] % 2) == 1){
//       // nums[total+1] = buf[i]; //for sending numbers back
//       newList.push_back(buf[i]);
//       total++;
//       printf("!");
//     }

//     if(i+1 >= buf[0]+1){
//       // Add the total to the beginning of the new list
//       // for receiving purposes
//       newList.insert(newList.begin(),total);
//     }
//   }

//   // Make a pointer to the internal array the vector uses.
//   int *newArray = &newList[0];

//   // int listSize = buf[0];
//   // cout << "client received: ";
//   // for (int i = 0; i < listSize; i++) {
//   //   cout << buf[i];
//   // }
//   // cout << endl;

//   // int *newArray = sieve(buf, listMax);

//   //send numbers to server (let's send odd numbers back)
//   if (send(sockfd, newArray, sizeof(int)*(newArray[0]+1), 0) == -1) {
//     perror("send");
//   }

//   // Return the new array after having run the seive
//   return newArray;
// }

// Takes in a list where the first number is the size of the list
// int *Client::sieve(int *list, int listMax) {
//   vector<int> newList;
//   int total = 0;
//   int listSize = list[0];
//   int numToCheck = list[1];

//   for(int i = numToCheck; i <= listSize; i++){
//     // If list[i] is not a multiple of numToCheck, add it to the new list
//     if ((list[i] % numToCheck) != 0) {
//       newList.push_back(list[i]);
//       total++;
//     }
//   }

//   newList.insert(newList.begin(), total);

//   // Make a pointer to the internal array the vector uses.
//   int *newArray = &newList[0];


//   cout << "newArray size: " << newArray[0] << endl;;
//   cout << "newArray: ";
//   for (int i = 0; i < newArray[0]; i++) {
//     cout << newArray[i];
//   }
//   cout << endl;

//   return newArray;
// }

vector<int> Client::sieve(int sockfd, int listMax) {

  //PREPARATION
  int list[listMax + 1];  //used to create sieveList
  int sieveList[listMax + 1]; //holds all numbers in list[] that aren't multiples of the 
                                          //first list number in the list, plus 2 helper variables. 
                                          //Starts out empty
  vector<int> masterList; //our return value at the end of the sieve

  //populate sieveList with numbers 2 through listMax starting at sieveList[1] (not 0)
  for(int i = 2; i < listMax + 1; i++){
    list[i - 1] = i;
  }
  list[0] = listMax; //the first variable in a sieveList is the size of the list 
                      //(it's the next best thing we could think of next to "list.size()")

  //With a list made, start the sieve
  while(1){
    if(list[1] > (int)sqrt((double) listMax)){
      for(int i = 2; i < sieveList[0]+1; i++){ //add all numbers still in sieveList to the masterList
        // masterList[masterLength] = sieveList[i];
        // masterLength++;
        masterList.push_back(sieveList[i]);
        // cout << "adding " << sieveList[i] << " to masterList" << endl;
        // cout << "masterList: ";
        // for (int j = 0; j < masterList.size(); j++) {
        //   cout << masterList.at(j) << " ";
        // }
        // cout << endl;
      }
      return masterList; //we're assuming sieveList has something by now, 
                            //that we did at least one sieve step. may need an if statement just in case
    } else {

      //first, record the first list number in the list received to the masterList
      masterList.push_back(list[1]);
      // cout << "adding " << list[1] << " to masterList" << endl;

      //do a step of the sieve:
      // cout << endl << "doing sieve with " << list[1] << endl;
      int j = 1;
      for(int i = 2; i <= list[0]; i++){ //Note: list[1] is prime. we want to check list[2] to the end.

        if(list[i] % list[1] != 0){ //in other words, "if this number is not a multiple of the 
                                    //prime number being checked"
          // cout << "  " << list[i] << " mod " << list[1] << " != 0" << endl;
          sieveList[j] = list[i];
          j++;
        }
      } //end sieve step
      // cout << endl;

      //Add helper variables to the list
      sieveList[0] = j - 1; //sieveList size. The minus 1 is there because j is ahead by 1
      // sieveList[j] = 0; //"end of list" variable. It belongs just past the actual list

      //Also, record the first list number in the new sieveList into masterList
      masterList.push_back(sieveList[1]);
      // cout << "adding " << sieveList[1] << " to masterList" << endl;

      //now the list is the way we want it, send it to the server
      /*
      code block to send data to the other machine, called Server
      */

      // Send size of list in bytes
      unsigned long int dataSize = htonl(sizeof(sieveList)); // host to network byte-order
      cout << "Sending dataSize: " << sizeof(sieveList) << endl;
      if (send(sockfd, (const char *)&dataSize, sizeof(unsigned long int), 0) == -1) {
        perror("Client: send dataSize");
      }

      if (send(sockfd, sieveList, sizeof(sieveList), 0) == -1) {
        perror("Client: send sieveList");
      }

      cout << "Sent: ";
      // for (int i = 0; i <= sieveList[0]; i++) {
      //   cout << sieveList[i] << " ";
      // }
      // cout << endl;
      printList(sieveList);



      //With the data sent, we'll have to wait for Server to send the data back. Then we do more.
      /*
      code block to recieve data from the Server. data is loaded into list[]
      */
      // Receive data size
      int numbytes;
      // unsigned long int dataSize;
      if ((numbytes = recv(sockfd, (char *)&dataSize, sizeof(unsigned long int), 0)) == -1) { 
        //triggers if recieving too much data at once
        perror("Client: recv");
        exit(1);
      }
      dataSize = ntohl(dataSize);
      if (numbytes != 0) {
        cout << "RECEIVED dataSize: " << dataSize << endl;
      }

      // Receive list
      int totalBytes = 0; // reset numbytes
      bool recvDone = false;
      char buf[MAXDATASIZE];
      while (!recvDone) {
        int currentBytes = 0;
        if ((currentBytes = recv(sockfd, buf+totalBytes, dataSize, 0)) == -1) { 
          //triggers if recieving too much data at once
          perror("Client: recv");
          exit(1);
        }
        cout << "currentBytes: " << currentBytes << endl;
        if (currentBytes == 0) {
          recvDone = true;
          perror("client: server closed connection");
        } else {

          totalBytes += currentBytes;

          if (totalBytes == dataSize) {
            recvDone = true;
          }
        }
      }

      memcpy(list, buf, dataSize);

      // received something from client
      if (totalBytes != 0) {
        // cout << "list size: " << list[0] << endl;
        printf("Recvd: ");
        // for(int i = 0; i < (list[0]+1); i++){
        //   printf(" %d", list[i]);
        // }
        // printf("\n");
        printList(list);
        cout << endl; 
      }

    } // end if/else
  } //end sieve while loop

}

// Assumes list[0] is length of list
void Client::printList(int *list) {
  int length = list[0];
  if (list[0] <= 5) { // print whole list
    for (int i = 1; i <= length; i++) {
      cout << list[i] << " ";
    }
    cout << endl;
  } else { // print first five + ...
    for (int i = 1; i <= 5; i++) {
      cout << list[i] << " ";
    }
    cout << "..." << endl;
  }
}

const char *Client::getHost() {
  return host;
}

const char *Client::getPort() {
  return port;
}

