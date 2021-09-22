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
 * @file   SocketServer.h
 * @author Chuck Sugnet
 * @date   Mon Jan 25 11:17:58 2010
 * 
 * @brief  Basic version of a server that will listen for connections on a dedicated
 *         socket and then listen on those connections for new messages from clients.
 * 
 */
#pragma once
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <string>
#include <vector>
#include "util/SocketBase.h"

/**
 * Basic version of a server that will listen for connections on a dedicated
 * socket and then listen on those connections for new messages from clients.
 */
class SocketServer : public SocketBase {

public:

  /// Constructor
  SocketServer();
  /// Destructor, clean up open sockets.
  ~SocketServer();

  /** 
   * Open a connection to a listening server with a range of possible ports to try.
   * 
   * @param host - ip address or name of computer to open connection on, often just 127.0.0.1
   * @param port - which port  number to start the open connection request with.
   *
   * This is current coded to allow for 5 trys.  TBD: is this a good number of ports
   * Should the caller control the number of trys.
   */
  unsigned int socketOpen(const std::string &host, unsigned int &startPort);

  /** 
   * Open a connection to a listening server.
   * 
   * @param host - ip address or name of computer to open connection on, often just 127.0.0.1
   * @param port - which port to open.
   */
  bool socketOpen(const std::string &host, const std::string &port);

  /** 
   * Try reading from the socket to see if there is anything sent.
   * 
   * @param messages - data from the socket as delivered by recv()
   */
  bool  checkForMsgs(std::vector<std::string> &messages);

  /** 
   * Try reading from the socket to see if there is anything sent.
   */
  bool processNewMessages();

  /** 
   * Write string to open socket.
   * 
   * @param msg - Message to write into the socket
   */
  void sendMsg(const std::string &msg, int type, int channel);

  /** 
   * See if any clients have connected to server.
   * 
   * @return - Number of clients connected in this function
   */
  bool acceptNewConnection();

  /** 
   * Wrapper to get IPv4 or IPv6 as appropriate
   * 
   * @param sa - Pointer to socket address structure
   * 
   * @return - Pointer to correct socket address
   */
  static void *getInAddr(struct sockaddr *sa);

  /** 
   * Check to see if the server has any connections yet.
   * 
   * @return 
   */
  bool isConnected();

  void cleanUp();

private:

  int m_ListenSocket;    ///< listening socket descriptor for new connection
  int m_RWSocket;        ///< Client socket for reading and writing
  std::string m_Port;    ///< Port open for new connections. ///< Address information for socket connections


};

#endif /* SOCKETSERVER_H */
