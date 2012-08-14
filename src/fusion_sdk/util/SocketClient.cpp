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


#ifdef WIN32 /* Windows includes are different for sockets than unix. */
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif /* WIN32 */

#include "util/SocketBase.h"
#include "util/SocketServer.h"
#include "util/SocketClient.h"
#include "util/Err.h"
#include "util/Verbose.h"

using namespace std;

SocketClient::SocketClient() {
  m_Socket = -1;
#ifdef WIN32

    Err::configureErrHandler(true, true);
	Err::setNewLineOnError(true);
	Err::setThrowStatus(false);
	//
	Err::setExitOnError(true);
	Err::setExitOnErrorValue(0);
	////
  if (WSAStartup(MAKEWORD(2,0), &m_WsaData) != 0) {
    APT_ERR_ABORT("WSAStartup failed.\n");
  }
#endif
}

SocketClient::~SocketClient() {
    cleanUp();
}

void SocketClient::cleanUp() {
  if (m_Socket >= 0) {
     // Verbose::out(1, "Cleaning up client socket.");
    CLOSE_SOCKET(m_Socket);
  }
#ifdef WIN32
  WSACleanup();
#endif
}

void SocketClient::socketConnect(const std::string &host, const std::string &port) {
  struct addrinfo hints, *servinfo, *p;
  int rv;
  m_Host = host;
  m_Port = port;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
    APT_ERR_ABORT("getaddrinfo: " + ToStr(gai_strerror(rv)));
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((m_Socket = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1) {
      continue;
    }

    if (connect(m_Socket, p->ai_addr, p->ai_addrlen) == -1) {
      CLOSE_SOCKET(m_Socket);
      continue;
    }

    break;
  }
  freeaddrinfo(servinfo);

  if (p == NULL) {
    APT_ERR_ABORT("Failed to connect - host: " + host + " port: " + port);
  }
  
#ifdef _WIN32
  int flag=1;
  rv=setsockopt(m_Socket,IPPROTO_TCP,TCP_NODELAY,(char*)&flag,sizeof(int));
	APT_ERR_ASSERT(rv==0, "return from setsockopt");
#endif

  Verbose::out(3, "SocketClient::socketConnect() -  connected");
}

void SocketClient::checkForMsgs(std::vector<std::string> &messages) {
	if(m_Socket < 0) {
	  APT_ERR_ABORT("Error: Must call socketConnect() before checkForMsgs().");
	}
	SocketBase::checkForMsgs(messages, m_Socket);
}

void SocketClient::sendMsg(const std::string &msg, int type, int channel) {
  if(m_Socket < 0) {
    APT_ERR_ABORT("Error: Must call socketConnect() before sendMsg().");
  }
  SocketBase::sendMsg(msg, type, channel, m_Socket);
}
