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
 * @file   SocketClient.h
 * @author Chuck Sugnet
 * @date   Mon Jan 25 11:07:19 2010
 * 
 * @brief  Basic version of a client that will open a socket to a server and
 *         read/write messages from that socket.
 * 
 */

#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include <string>

#include "util/SocketBase.h"

/**
 * Class for reading and writing to a server over a socket.
 */
class SocketClient : public SocketBase {

public:
  /// Constructor
  SocketClient();
  /// Destructor closes sockets
  ~SocketClient();

  void cleanUp();

  /** 
   * Open a connection to a listening server.
   * 
   * @param host - ip address or name of computer to connect to, often just 127.0.0.1
   * @param port - which port to try and connect on.
   */
  void socketConnect(const std::string &host, const std::string &port);

  /** 
   * Try reading from the socket to see if there is anything sent.
   * 
   * @param messages - data from the socket as delivered by recv()
   */
  void checkForMsgs(std::vector<std::string> &messages);

  /** 
   * Write string to open socket.
   * 
   * @param msg - Message to write into the socket
   */
  void sendMsg(const std::string &msg, int type, int channel);

private:
  int m_Socket;         ///< listening socket descriptor
  std::string m_Host;   ///< Host connected to
  std::string m_Port;   ///< Port that client is connected to
  std::vector<int> m_Sockets; ///< List of open sockets/file descriptors currently connected to, sorted by file descriptor

};

#endif /* SOCKETCLIENT_H */
