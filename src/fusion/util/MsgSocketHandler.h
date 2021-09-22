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
 * @file   MsgSocketHandler.h
 * @author Chuck Sugnet
 * @date   Thu Jan 21 16:26:33 PST 2010
 * 
 * @brief  File for handling messages from Verbose handler to a socket
 */

#ifndef MSGSOCKETHANDLER_H
#define MSGSOCKETHANDLER_H

#include <cstring>
#include <string>
#include <vector>
#include "util/MsgHandler.h"
#include "util/ProgressHandler.h"
//
class SocketClient;

#define SOCKETSTOP "_SOCKET_STOP_"
#define FATALERROR "FATAL ERROR:"


/**
 * @brief An abstract handler for messages.
 */
class MsgSocketHandler : public MsgHandler, public ProgressHandler {

public:

  /** 
   * Constructor
   * @param verbosity - Verbosity level to print at higher -> more messages
   */
  MsgSocketHandler(int verbosity);

  /**
   * Destructor
   */
  ~MsgSocketHandler();

  /** 
   * Check to see if handler has been sent the message to abort by server.
   */
  void checkForKill();

  /** 
   * Open a socket for reading/writing on the specified host and port
   * 
   * @param host - Ip address, often 127.0.0.1 or localhost
   * @param port - Which port to open
   */
  void openSocket(const std::string &host, const std::string &port);

  /** 
   * Begin a progress reporting. After this initial call the
   * progressStep call will be called N times where N = the parameter
   * total passed in below.
   * 
   * @param verbosity - What level of verbosity is the progress
   * associated with. Higher levels of verbosity produce more
   * messages.
   * @param msg - String message associated with the beginning of this task.
   * @param total - Expected number of times that the progressStep()
   * will be called after this.
   */
  virtual void progressBegin(int verbosity, const std::string &msg, int total);

  /** 
   * This function is called when one unit of work has been done. In
   * general it is expected that the units of work should be roughly
   * equal in size.
   * 
   * @param verbosity - At what verbosity level should this step be
   * displayed.
   */
  void progressStep(int verbosity); 

  /** 
   * Signals the end of progress report. 
   * 
   * @param verbosity - Level of verbosity associated with this progress report.
   * @param msg - Closing message from calling function.
   */
  void progressEnd(int verbosity, const std::string &msg);
 
  /** 
   * A message to be processed by the handler.
   * 
   * @param level - What level of verbosity is associated with this message, higher number == more verbosity.
   * @param msg - Message that is to be processed.
   * @param delimiter - Should a delimiter be emitted as well?
   */
  void message(int level, const std::string &msg, bool delimiter = true);
  
  /** 
   * What level of verbosity is requested. This is a hint to the class and can
   * be ignored if desired.
   * 
   * @param level - Level below which progress messages are printed.
   */
  void setBaseVerbosity(int level);

  /**
   * Send message indication termination of computation.
   */
  void finishedMsg();

  /**
   * Set end of line suffix
   */
  void setEndOfLine(const std::string &s) {
      m_EndLine = s;
  }  
protected:
  
  SocketClient *m_Client; ///< Class for reading from and writing to socket.
  int m_LocVerbosity;     ///< Our verbosity level
  int m_Total;         ///< How many steps total are we expecting?  
  std::vector<std::string> m_Messages; ///< Buffer for messages from the socket.
  std::string m_EndLine; ///< Any end to tack on messages (like "\n");
};

#endif /* MSGHANDLER_H */
