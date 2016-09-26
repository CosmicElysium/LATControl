#include "Sockets.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>

using namespace std;

namespace lbtlat
{

//! The constructor of the class 
Sockets::Sockets() :
  m_sock ( -1 )
{
  //default 1 sec. 0 usec.
  writeTimeout.tv_sec =  1;
  writeTimeout.tv_usec = 0;	
  
  memset ( &m_addr, 0, sizeof ( m_addr ) );
}

//! The destructor of the class 
Sockets::~Sockets()
{
  if ( is_valid() )
    ::close ( m_sock );
}

//! Closes the socket connection
void Sockets::close()
{
  if ( is_valid() )
  {
    ::close ( m_sock );
    m_sock = -1;
  }
}

//! Creates an endpoint for communication.
bool Sockets::create()
{
  m_sock = socket ( AF_INET, SOCK_STREAM, 0 );

  if ( ! is_valid() )
    return false;

  // TIME_WAIT - argh
  int on = 1;
  
  if ( setsockopt ( m_sock, IPPROTO_TCP, TCP_NODELAY, ( const char* ) &on, sizeof ( on ) ) == -1 )
    return false;
  
  return true;
}


//! This function assigns an address to the socket.
bool Sockets::bind ( const int port )
{

  if ( ! is_valid() )
    {
      return false;
    }

  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = INADDR_ANY;
  m_addr.sin_port = htons ( port );

  int bind_return = ::bind ( m_sock, ( struct sockaddr * ) &m_addr, sizeof ( m_addr ) );

  if ( bind_return < 0 )
    {
      return false;
    }

  return true;
}

//! This function shall listen for socket connections and limit the queue of incoming connections.
bool Sockets::listen() const
{
  if ( ! is_valid() )
    {
      return false;
    }

  int listen_return = ::listen ( m_sock, MAXCONNECTIONS );

  if ( listen_return == -1 )
    {
      return false;
    }

  return true;
}

//! This function is used to accept a connection request on the server socket.
bool Sockets::accept ( Sockets& new_socket ) const
{
  int addr_length = sizeof ( m_addr );
  new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

  if ( new_socket.m_sock <= 0 )
    return false;
  else
    return true;
}

//! Sends a message to a connected socket.
bool Sockets::send ( const std::string s ) const
{
  int status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );

  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}

//! receives a message from a connected socket
int Sockets::recv ( std::string& s ) const
{
  char buf [ MAXRECV + 1 ];

  s = "";

  int ret = wait(3500000);
  if (ret >0)
  {
    fcntl (m_sock, F_SETFL, fcntl (m_sock, F_GETFL) | O_NONBLOCK);

    memset ( buf, 0, MAXRECV + 1 );

    int status = ::recv ( m_sock, buf, MAXRECV, 0);

    fcntl (m_sock, F_SETFL, fcntl (m_sock, F_GETFL) & ~O_NONBLOCK);

    if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Sockets::recv\n";
      return 0;
    }
    else if ( status == 0 )
    {
      return 0;
    }
    else
    {
      s = buf;
      return status;
    }
  } 
  else return 0;
}

//! Overloaded recv: receives a vector of ushort numbers from a connected socket.
int Sockets::recv ( std::vector<ushort> & data ) const
{
  data.clear();
  data.resize(MAXRECVUS/2);
  ushort buf [ MAXRECVUS + 1 ];
  
  int ret = wait(3500000);
  if (ret >0)
  {
    fcntl (m_sock, F_SETFL, fcntl (m_sock, F_GETFL) | O_NONBLOCK);
  
    memset ( buf, 0, MAXRECVUS + 1 );

    int status = ::recv ( m_sock, buf, MAXRECVUS, 0 );

    fcntl (m_sock, F_SETFL, fcntl (m_sock, F_GETFL) & ~O_NONBLOCK);
  
    if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Sockets::recv\n";
      return 0;
    }
    else if ( status == 0 )
    {
      return 0;
    }
    else
    {
      for ( int i=0; i< status/2; i++)
         data[i]=buf[i];
      
      return status;
    }
  }
  else return 0;
}

//! Overloaded recv: receives a vector of integer numbers from a connected socket.
int Sockets::recv ( std::vector<int> & data ) const
{
  data.clear();
  data.resize(MAXRECVUS/2); //int = 2bytes. 
  int buf [ MAXRECVUS + 1 ];
  
  memset ( buf, 0, MAXRECVUS + 1 );

  int status = ::recv ( m_sock, buf, MAXRECVUS, 0 );

  if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Sockets::recv\n";
      return 0;
    }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      for (int i=0; i< status/2; i++)
      {   data[i]=buf[i];
      } 
      return status;
    }
}

//! connect shall attempt to make a connection on a socket.
bool Sockets::connect ( const std::string host, const int port )
{
    if ( ! is_valid() ) return false;

    int initialFlags;  // flags initially set on this socket

    // for select()
    fd_set writefds;
    int ret;  // for return value of select()
   
    // configure this socket's address
    m_addr.sin_family = AF_INET;      // for internet addresses
    m_addr.sin_port = htons(port);    // choose a port
    // load the address field with the translated hostIp.
    m_addr.sin_addr.s_addr = inet_addr(host.c_str());
    
    // get current socket flags
    initialFlags = fcntl(m_sock, F_GETFL);
    set_non_blocking(true);

    // try to connect 
    if (::connect(m_sock, (struct sockaddr*) &m_addr, sizeof(m_addr)) !=0 )
    {
	// check for errors (we want to block for a bit if the socket
        // connection is in progress)
        if (errno==EINPROGRESS)
        {
            // set up writefds to see when we're able to write

            // empty this fd_set
            FD_ZERO(&writefds);

            // add this socket to the set to wait on.  this way we'll
            // only get a notification about this socket
            FD_SET(m_sock, &writefds);

            // wait for a socket to become available for writing
	    if (writeTimeout.tv_sec!=-1)
	    {
		ret = select(m_sock+1,
			     (fd_set*)NULL,
			     &writefds,
			     (fd_set*)NULL,
			     &writeTimeout);
	    }
	    else
	    {
		// we don't want to ever time out
		// need to pass a null pointer instead of time struct

		ret = select(m_sock+1,
			     (fd_set*)NULL,
			     &writefds,
			     (fd_set*)NULL,
			     (struct timeval*)NULL);
	    }


            if (-1==ret)
            {
		// select() failed

                // return this socket to initial settings
                fcntl(m_sock, F_SETFL, initialFlags);
		
		return false;
            }
            else if (0==ret)
            {
                // return this socket to initial settings
                fcntl(m_sock, F_SETFL, initialFlags);
		cerr << "[E] Socket::connect -> timeout" <<endl;
		return false;
            }

	    // if we get here, connection was established successfully
        }
        else
        {
	    // return this socket to initial settings
	    fcntl(m_sock, F_SETFL, initialFlags);
		cerr << "[E] Socket::connect -> connection refused" <<endl;
		return false;
        }
    }

    // make it so that this socket won't be kept open across calls to
    // the exec() family of functions
    fcntl(m_sock, F_SETFD, FD_CLOEXEC);

    // return this socket to initial settings
    fcntl(m_sock, F_SETFL, initialFlags);

    return true;
}

//! Activates/deactivates the blocking of the file descriptor. 
void Sockets::set_non_blocking ( const bool b )
{

  int opts;

  opts = fcntl ( m_sock, F_GETFL );

  if ( opts < 0 )
    {
      return;
    }

  if ( b )
    opts = ( opts | O_NONBLOCK );
  else
    opts = ( opts & ~O_NONBLOCK );

  fcntl ( m_sock, F_SETFL,opts );
}


int Sockets::wait(int time) const
{
  int ret = 0;
  fd_set rset;
  struct timeval timeout;
  FD_ZERO (&rset);
  FD_SET (m_sock, &rset);
  if (time < 0)
  {
      ret = ::select (m_sock + 1, &rset, NULL, NULL, NULL);
  }
  else
  {
     timeout.tv_sec = time / 1000000;
     timeout.tv_usec = time % 1000000;
     ret =::select (m_sock + 1, &rset, NULL, NULL, &timeout);
  }
//   if (ret < 0)
//	std::cout << "status == -1   errno == " << errno << "  in Sockets::wait::select\n";

  return ret;
}

}
