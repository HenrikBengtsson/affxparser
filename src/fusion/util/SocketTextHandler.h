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
#ifndef SOCKETTEXTHANDLER_H
#define SOCKETTEXTHANDLER_H

#include <map>
#include <string>
#include "util/SocketHandler.h"


/**
 * Basic Socket Handler (Listener) that parses out
 * verbose and progress messages from a socket stream.
 */
class SocketTextHandler : public SocketHandler {

public: 
  
  /// Constructor
  SocketTextHandler();

  /// Has the socket send the "completion" message?
  bool hasFinished();

  
  /// was the run successful?
  bool successfulRun();

  /** 
   * Take a message and do whatever functionality this handler
   * implements from simply displaying message to 
   * 
   * @param type - What type (i.e. text, xml, ) of message is this message.
   * @param channel - What channel (i.e. verbose, progress, etc.) is this message.
   * @param msg - Data to be parsed out
   */
  virtual void handleMessage(int type, int channel, const std::string &msg);

protected:
  bool m_Finished; ///< Has the socket sent the "completed" signal?
  bool m_SuccessfulRun;  // was the run succesful or aborted

};

#endif /* SOCKETTEXTHANDLER_H */
