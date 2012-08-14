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
 * @file   SocketBase.h
 * @author Chuck Sugnet
 * @date   Mon Jan 25 11:29:11 2010
 * 
 * @brief  Some utilities for dealing with reading and writing to sockets.
 */

#ifndef _UTIL_SOCKETBASE_H
#define _UTIL_SOCKETBASE_H

//
#include "portability/affy-base-types.h"
#include "util/SocketHandler.h"
//
#include <string>
#include <vector>

#ifdef _WIN32
// Windows includes are different for sockets than unix.
#include <winsock2.h>  //QZ: windows.h will load winsock.h automatically. It seems we do not use any winsock2.h specific functions. Ellen has tested out the changes.
#else
#include <sys/socket.h>
#endif

#ifdef _WIN32
#define CLOSE_SOCKET(_socket) { shutdown(_socket,SD_BOTH); closesocket(_socket); }
#else
#define CLOSE_SOCKET(_socket) shutdown(_socket,SHUT_RDWR)
#endif


/**
 * Class with static utility functions for working with Sockets.
 * 
 * The messages passsed back and forth have a standard message header which consists of
 * three 4 byte integers consisting of the message size, message type and message channel
 * This information is used to know when the full message has been recieved and what to do
 * with the resulting following the header. The type of the message would be something like the
 * format and the channel would be which handler would be interested. So for exampe a basic type
 * would be "text" and a basic channel would be "stdout", or progress. This is analagous to the 
 * type being the format of the signal coming in (eg high definition or regular tv) and the
 * channel being the address of where to send the data (eg hbo television channel).
 *
 * Msg format:
 * @todo - supported types, versions, etc.?
 <pre>
 int32_t|int32_t|int32_t|int32_t|int32_t | bytes
  size  | type  |channel|version|reserved| msg bytes
 </pre>
 * 
 * 
*/
class SocketBase {

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
  virtual void parseMsgFromBuffer(std::vector<char> &buffer, std::vector<std::string> *messages);

  /** 
   * Try to read new messages from socket.
   * 
   * @param messages - Vector to fill with char * messages from socket.
   * @param socket - socket to read messages from. Set to -1 if disconnected.
   */
  virtual bool checkForMsgs(std::vector<std::string> &messages, int &socket);

  /** 
   * Try to read new messages from socket.
   * 
   * @param socket - socket to read messages from. Set to -1 if disconnected.
   */
  virtual bool checkForMsgs(int &socket);

  /** 
   * Write char * messages into a socket.
   * 
   * @param msg - Message to write to the socket.
   * @param type - What format of message? e.g. SocketHandler::TEXT
   * @param channel - What channel or sort of message? e.g. SocketHandler::VERBOSE
   * @param socket - Socket that could be written to.
   */
  void sendMsg(const std::string &msg, int type, int channel, int &conn);

  /**
   * Add a handler for incoming messages. This handler will have a chance
   * see the message type and channel and decide if it should do something.
   * Like a java listener or a regular callback
   *
   * @param - 
   *
   */
  virtual void addHandler(SocketHandler *handler);

  /** 
   * Take a 4 byte 32bit int from buff and parse it into x.
   * 
   * @param buf - Buffer with at least 4 bytes in it.
   * @param x - Integer to be filled in.
   */
  static void unPackInt32NetOrder(const char *buf, int32_t &x);

  /** 
   * Pack int x into first 4 bytes pointed at by buf.
   * 
   * @param buf - Buffer containing at least 4 bytes to write int into
   * @param x - integer to be written.
   */
  static void packInt32NetOrder(char *buf, int32_t x);

  /** 
   * Fill in our buffer from the socket connection
   * 
   * @param conn - Socket fd desc
   * 
   * @return 
   */
  bool fillBufferFromSocket(int &conn);

protected:

  /// Data coming over socket, including any partial messages
  std::vector<char> m_Buffer;

  std::vector<SocketHandler *> m_Handlers;
#ifdef WIN32
  WSADATA m_WsaData;   ///< Windows data socket structure
#endif
};

#endif /* SOCKETBASE_H */
