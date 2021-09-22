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
 * @file   MsgHandler.h
 * @author Chuck Sugnet
 * @date   Mon Jan 30 16:46:01 2006
 * 
 * @brief  File for handling messages from Verbose handler.
 */

#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include <cstring>
#include <string>
//

/**
 * @brief An abstract handler for messages.
 */
class MsgHandler {

public:
  
  /** Virtual destructor for a virtual class. */
  virtual ~MsgHandler() {}

  /** 
   * A message to be processed by the handler.
   * 
   * @param level - What level of verbosity is associated with this message, higher number == more verbosity.
   * @param msg - Message that is to be processed.
   * @param delimiter - Should a delimiter be emitted as well?
   */
  virtual void message(int level, const std::string &msg, bool delimiter = true) = 0;

  /** 
   * What level of verbosity is requested. This is a hint to the class and can
   * be ignored if desired.
   * 
   * @param level - Level below which progress messages are printed.
   */
  void setBaseVerbosity(int level) { m_Verbosity = level; }
  
protected:
  
  int m_Verbosity;  ///< What level of verbosity is processed, higher == more messages.

};

#endif /* MSGHANDLER_H */
