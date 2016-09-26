#ifndef Sockets_class_h
#define Sockets_class_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <vector>

namespace lbtlat
{

/// Connection parameters.
const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 80;
const int MAXRECVUS = 2000;

/** @class Sockets
     @author Jose Borelli<borelli@mpia.de>
     @brief This class is used to create and use sockets connections
     @todo This class needs a logger. */
class Sockets
{
 public:
 
  /** @brief class constructor */
  Sockets();

  /** @brief class destructor */
  virtual ~Sockets();
  
  /** @brief  Creates an endpoint for communication. 
  @return true upon successful completion. */ 
  bool create();
  
  /** @brief Assigns an address to the socket.
 @param port the value that will be converted from host to network byte order
 @return true upon successful completion. */
  bool bind ( const int port );
  
  /** @brief This function shall listen for socket connections and limit the queue of incoming connections.
 @return Upon successful completions, listen() shall return true. */
  bool listen() const;
  
  /** @brief Used to accept a connection request on the server socket.
 @note  A socket that has been established as a server can accept connection
 requests from multiple clients.  The server's original socket does not  become part of the connection; 
 instead, `accept' makes a new socket which participates in the connection. The server's original 
 socket remains available for listening for further connection requests.
 @param new_socket new socket for the connection
 @return true if the connection was accepted, otherwise false. */ 
  bool accept ( Sockets& new_socket) const;
  
  /** @brief closes the socket connection. */
  void close();
  
  /** @brief wait for microseconds.
  @param time time (microseconds) to wait for the data */
  int wait(int time) const;
  
  /** @brief connect shall attempt to make a connection on a socket.
  @param host  defines the Name or IP address of the host
  @param port   defines the port number to connect.
  @return Upon successful completion, connect() shall return true. */
  bool connect ( const std::string host, const int port );

  /** @brief Sends a message to a connected socket.
 @param s string with the message to send
 @return Upon successful completion, send shall return true.*/
  bool send ( const std::string s) const;
  
  /** @brief receives a message from a connected socket
 @param s string where the message should be stored.
 @return Upon successful completion, recv shall return the number of  bytes recived and the string shall be allocated in s. */
  int recv ( std::string& s) const;
  
  /** @brief Overloaded recv function: receives a vector of ushort numbers from a connected socket.
  @param data the vector to allocate the ushort numbers.
  @return Upon successful completion, recv shall return the number of  bytes recived (!=0). */
  int recv ( std::vector<ushort> & data) const;
  
  /** @brief Overloaded recv function: receives a vector of integer numbers from a connected socket.
  @param data the vector to allocate the integer numbers.
  @return Upon successful completion, recv shall return the number of  bytes recived (!=0). */
  int recv ( std::vector<int> & data) const;
   
   /** @brief Activates/deactivates the blocking of the file descriptor. 
   @param b flag to activate/deactivate the blocking. */
  void set_non_blocking ( const bool b);
  
  /** @brief Defines the connection time out, either in seconds or milliseconds. */
  void set_connect_timeout(int sec, int usec=0) {writeTimeout.tv_sec= sec; writeTimeout.tv_usec= usec; } ;

  /** @brief Returns the status of the connection. */
  bool is_valid() const { return m_sock != -1; }

 private:

  int m_sock;
  sockaddr_in m_addr;
  
  struct timeval writeTimeout;
};
};
#endif
