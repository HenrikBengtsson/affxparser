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

#include "util/SocketBase.h"
#include "util/Err.h"
#include "util/Convert.h"

using namespace std;
#define HEADER_SIZE 20 // 5 * 4 byte integers

void SocketBase::parseMsgFromBuffer(std::vector<char> &buffer, std::vector<std::string> *messages) {
  int expected = -1;
  int channel = -1;
  int type = -1;
  int protocol = -1;
  int reserved = -1;
  while (buffer.size() >= HEADER_SIZE) { // header has at least HEADER_SIZE bytes...
    const char *cBuf = &m_Buffer[0];
    unPackInt32NetOrder(cBuf, expected);
    unPackInt32NetOrder(cBuf+4, type);
    unPackInt32NetOrder(cBuf+8, channel);
    unPackInt32NetOrder(cBuf+12, protocol);
    APT_ERR_ASSERT(protocol == SOCKET_PROTOCOL_VER, "Expecting protocol version: " + ToStr(SOCKET_PROTOCOL_VER) + " but got: " + ToStr(protocol));
    unPackInt32NetOrder(cBuf+16, reserved);
    if (buffer.size() >= expected+HEADER_SIZE) {
      string current(expected, 0);
      for (int i = 0; i < expected; i++) {
        current[i] = buffer[i + HEADER_SIZE];
      }
      if (messages != NULL) {
        messages->push_back(current);
      }
      else {
        for (int i = 0; i < m_Handlers.size(); i++) {
          if (m_Handlers[i]->handlesType(type) && m_Handlers[i]->handlesChannel(channel)) {
            m_Handlers[i]->handleMessage(type, channel, current);
          }
        }
      }
    }
    else {
      break; // incomplete message, leave it for later
    }
    buffer.erase(buffer.begin(), buffer.begin() + expected + HEADER_SIZE);
  }
}

bool  SocketBase::fillBufferFromSocket(int &conn) {
  bool connected = true;
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
          Verbose::out(3, "SocketBase::checkForMsgss() -  socket " + ToStr(conn) + " hung up");
          CLOSE_SOCKET(conn); // bye!
          conn = -1;
          connected = false;
        } 
        else {
          APT_ERR_ABORT("checkForMsgs() error.");
        }
    }
    else {
      for (int i = 0; i < nBytes; i++) {
        m_Buffer.push_back(buf[i]);
      }
    }
  }
  return connected;
}

bool SocketBase::checkForMsgs(std::vector<std::string> &messages, int &socket) {
  bool connected = fillBufferFromSocket(socket);
  messages.clear();
  parseMsgFromBuffer(m_Buffer, &messages);
  return connected;
}

bool SocketBase::checkForMsgs(int &socket) {
  bool connected = fillBufferFromSocket(socket);
  parseMsgFromBuffer(m_Buffer, NULL);
  return connected;
}

int SocketBase::sendAll(int socket, const char *buf, int toSend, int &sent) {
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

void SocketBase::unPackInt32NetOrder(const char *buf, int32_t &x) {
  int32_t z = (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
  x = ntohl(z);
}

void SocketBase::packInt32NetOrder(char *buf, int32_t x) {
  int32_t z = htonl(x);
  *buf++ = z>>24; 
  *buf++ = z>>16;
  *buf++ = z>>8;  
  *buf++ = z;
}

void SocketBase::addHandler(SocketHandler *handler) {
  m_Handlers.push_back(handler);
}

void SocketBase::sendMsg(const std::string &msg, int type, int channel, int &conn) {
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
    int bufSize = msg.size() + HEADER_SIZE; // 12 bytes in header
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    int32_t size = msg.size();
    packInt32NetOrder(buf, size);
    packInt32NetOrder(buf+4, type);
    packInt32NetOrder(buf+8, channel);
    packInt32NetOrder(buf+12, SOCKET_PROTOCOL_VER);
    packInt32NetOrder(buf+16, 0);
    for (int i = 0; i < size; i++) {
      buf[i+HEADER_SIZE] = msg.at(i);
    }
    int sent = 0;
    if (sendAll(conn, buf, bufSize, sent) == -1) {
      APT_ERR_ABORT("Error sending data to fd: " + ToStr(conn));
    }
    if (sent != bufSize) {
      APT_ERR_ABORT("Error expecting to send: " + ToStr(bufSize) + " but only got: " + ToStr(sent));
    }
	FreezArray(buf);
  }
}
