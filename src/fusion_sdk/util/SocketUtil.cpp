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
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif /* WIN32 */

#include "util/SocketUtil.h"
#include "util/Err.h"
#include "util/Convert.h"

using namespace std;

void SocketUtil::parseMsgFromBuffer(std::string &buffer, std::vector<std::string> &messages) {
  messages.clear();
  int expected = -1;
  string current = buffer;
  int pos = current.find(':');
  
  while (pos != string::npos) {
    string eStr = current.substr(0, pos);
    expected = Convert::toInt(eStr);
    APT_ERR_ASSERT(expected >= 0, "Can't expect messages of negative length.");
    current = current.substr(pos + 1); 
    if (current.size() >= expected) {
      messages.push_back(current.substr(0, expected));
    }
    else {
      break; // incomplete message, leave it for later
    }
    current = current.substr(expected);
    pos = current.find(':');
  }
  buffer = current;
}

void SocketUtil::checkForMsgs(std::vector<std::string> &messages, int &conn) {
  messages.clear();
  fd_set readFds;  
  FD_ZERO(&readFds);
  FD_SET(conn, &readFds);
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  char buf[2048];
  int nBytes = 0;
  if (select(conn + 1, &readFds, NULL, NULL, &tv) == -1) {
    APT_ERR_ABORT("select() failed.");
  }
  if (FD_ISSET(conn, &readFds)) { // we got one!!
    memset(buf, 0, 2048);
    if ((nBytes = recv(conn, buf, sizeof(buf), 0)) <= 0) {
      // got error or connection closed by client
        if (nBytes == 0) {
          // connection closed
          Verbose::out(1, "SocketUtil::checkForMsgss() -  socket " + ToStr(conn) + " hung up");
          CLOSE_SOCKET(conn); // bye!
          conn = -1;
        } 
        else {
          APT_ERR_ABORT("checkForMsgs() error.");
        }
    }
    else {
      m_Buffer.append(buf);
    }
  }
  parseMsgFromBuffer(m_Buffer, messages);
}

int SocketUtil::sendAll(int socket, const char *buf, int toSend, int &sent) {
  int total = 0;        // how many bytes we've sent
  int bytesLeft = toSend; // how many we have left to send
  int curSend = 0;

  while(total < toSend) {
    curSend = send(socket, buf+total, bytesLeft, 0);
    if (curSend == -1) { 
      break; 
    }
    total += curSend;
    bytesLeft -= curSend;
  }

  sent = total; // return number actually sent here
  int rv = -1;
  if (curSend != 0) 
    rv = 0;
  return rv;
} 


void SocketUtil::sendMsg(const std::string &msg, int &conn) {
  fd_set readFds;  
  fd_set writeFds;  
  FD_ZERO(&readFds);
  FD_SET(conn, &readFds);
  writeFds = readFds;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  if (select(conn + 1, &readFds, &writeFds, NULL, &tv) == -1) {
    APT_ERR_ABORT("select() failed.");
  }
  if (FD_ISSET(conn, &writeFds)) { // we got one!!  
    string msgSend = ToStr(msg.size()) + ":" + msg;
    int sent = 0;
    if (sendAll(conn, msgSend.c_str(), msgSend.size(), sent) == -1) {
      APT_ERR_ABORT("Error sending data to fd: " + ToStr(conn));
    }
  }
}
