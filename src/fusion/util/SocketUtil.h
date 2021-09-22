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

/**
 * @file   SocketUtil.h
 * @author Chuck Sugnet
 * @date   Mon Jan 25 11:29:11 2010
 * 
 * @brief  Some utilities for dealing with reading and writing to sockets.
 */

#ifndef SOCKETUTIL_H
#define SOCKETUTIL_H
#include <string>
#include <vector>

#ifdef WIN32
#define CLOSE_SOCKET closesocket
#else
#define CLOSE_SOCKET close
#endif

/**
 * Class with static utility functions for working with Sockets.
 * 
 */
class SocketUtil {

public: 

  /** 
   * Send a message over multiple socket send calls as necessary
   * 
   * @param socket - Socket to write to
   * @param buf - Buffer to send over socket
   * @param toSend - How long is buffer
   * @param sent - Number characters actually sent
   * 
   * @return 
   */
  virtual int sendAll(int socket, const char *buf, int toSend, int &sent);

  /** 
   * Tokenize the buffer into distinct messages
   * 
   * @param buffer - String to break into individual messages
   * @param messages - Vector of messages to fill in 
   */
  virtual void parseMsgFromBuffer(std::string &buffer, std::vector<std::string> &messages);

  /** 
   * Try to read new messages from socket.
   * 
   * @param messages - Vector to fill with char * messages from socket.
   * @param socket - socket to read messages from. Set to -1 if disconnected.
   */
  virtual void checkForMsgs(std::vector<std::string> &messages, int &socket);

  /** 
   * Write char * messages into a socket.
   * 
   * @param msg - Message to write to the socket.
   * @param socket - Socket that could be written to.
   */
  virtual void sendMsg(const std::string &msg, int &socket);

protected:

  /// Data coming over socket, including any partial messages
  std::string m_Buffer;

};

#endif /* SOCKETUTIL_H */
