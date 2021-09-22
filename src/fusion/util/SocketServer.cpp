////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License 
// (version 2.1) as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif /* WIN32 */
#include "util/Err.h"

#include "util/SocketServer.h"
#include "util/SocketBase.h"

using namespace std;

SocketServer::SocketServer() {
  m_ListenSocket = -1;
  m_RWSocket = -1;
#ifdef WIN32
  if (WSAStartup(MAKEWORD(2,0), &m_WsaData) != 0) {
	  Err::errAbort("Unable to initialize socket communication.");
  }
#endif
}

SocketServer::~SocketServer() {
    cleanUp();
}

void SocketServer::cleanUp() {
    if (m_ListenSocket >= 0) {
        CLOSE_SOCKET(m_ListenSocket);
    }
    if (m_RWSocket >= 0) {
        CLOSE_SOCKET(m_RWSocket);
    }
    
#ifdef WIN32
    WSACleanup();
#endif
}

unsigned int SocketServer::socketOpen(const std::string &host, unsigned int &startPort)
{
	unsigned int lastPort = startPort + 5;
	while (startPort <= lastPort)
	{
		try
		{
			if (socketOpen(host, ToStr(startPort)) == true)
				return (startPort);			
			startPort++;
		}
        catch(...)
		{
			startPort++;
		}
	}
	return 0;
}


bool SocketServer::socketOpen(const std::string &host, const std::string &port) {
  int rv = 0;
  struct addrinfo hints, *ai = NULL, *p = NULL; 
  // get us a socket and bind it
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(host.c_str(), port.c_str(), &hints, &ai)) != 0) {
	  return false;
    //APT_ERR_ABORT("getaddrinfo() error: " + ToStr(gai_strerror(rv)));
  }
  
  for(p = ai; p != NULL; p = p->ai_next) {
    m_ListenSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (m_ListenSocket < 0) { 
      continue;
    }

// Dont think we actually want to do this, as it may prevent 
// a relaunch of this app to reuse the socket.
// http://msdn.microsoft.com/en-us/library/ms740621(VS.85).aspx
//
// #ifdef _WIN32
//     char yes[256];
//     setsockopt(m_ListenSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, yes, sizeof(yes));
// #endif

   if (bind(m_ListenSocket, p->ai_addr, p->ai_addrlen) < 0) {
      CLOSE_SOCKET(m_ListenSocket);
      continue;
    }

    break;
  }

  freeaddrinfo(ai); // all done with this

  // if we got here, it means we didn't get bound
  if (p == NULL) {
	  return false;
    //APT_ERR_ABORT("Failed to bind host " + host + "  port: " + port);
  }
 
  // listen
  if (listen(m_ListenSocket, 10) == -1) {
	  return false;
    //APT_ERR_ABORT("Listening failed.");
  }
    
  return true;
}

bool SocketServer::isConnected() {
  return m_RWSocket >= 0;
}

bool SocketServer::checkForMsgs(std::vector<std::string> &messages) {
  if(m_RWSocket < 0) {
    APT_ERR_ABORT("Error: Must call socketOpen() before checking for messages.");
  }
  return SocketBase::checkForMsgs(messages, m_RWSocket);
}

bool SocketServer::processNewMessages() {
  return SocketBase::checkForMsgs(m_RWSocket);
}

void SocketServer::sendMsg(const std::string &msg, int type, int channel) {
  if(m_RWSocket < 0) {
    APT_ERR_ABORT("Error: Must call socketOpen() before sending messages.");
  }
  SocketBase::sendMsg(msg, type, channel, m_RWSocket);
}

bool SocketServer::acceptNewConnection() {
  if(m_ListenSocket < 0) {
    APT_ERR_ABORT("Error: Must call socketOpen() before checking for connections.");
  }
  fd_set m_ReadFds;
  FD_ZERO(&m_ReadFds);
  FD_SET(m_ListenSocket, &m_ReadFds);
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  if (select(m_ListenSocket + 1, &m_ReadFds, NULL, NULL, &tv) == -1) {
    APT_ERR_ABORT("select");
  }
  struct sockaddr_storage remoteaddr; // client address
  int newfd = -1;
  socklen_t addrlen;

  vector<int> toAdd;
  if (FD_ISSET(m_ListenSocket, &m_ReadFds)) { // we got one!!
    // handle new connections
    addrlen = sizeof(remoteaddr);
    newfd = accept(m_ListenSocket,
                   (struct sockaddr *)&remoteaddr,
                   &addrlen);
    
    if (newfd == -1) {
      APT_ERR_ABORT("accept");
    } 
    else {
      m_RWSocket = newfd;
      Verbose::out(2, "SocketServer: new connection found:"  + ToStr(m_RWSocket) );
      return true;
    }
  }
  return false;
}

// get sockaddr, IPv4 or IPv6:
void *SocketServer::getInAddr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
