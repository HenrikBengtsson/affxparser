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
 * @file   MsgStream.h
 * @author Chuck Sugnet
 * @date   Mon Jan 30 16:48:01 2006
 * 
 * @brief  File for handling messages and printing them to the stream.
 * 
 */

#ifndef MSGSTREAM_H
#define MSGSTREAM_H

#include "util/MsgHandler.h"
//
#include <cstring>
#include <iostream>
#include <string>
//

/**
 * @brief An output stream for messages.
 */
class MsgStream : public MsgHandler {

public:
  /** Constructor. */
  MsgStream(int level, std::ostream *out=NULL) {
    m_Verbosity = level;
    m_Out = out;
  }

  /** 
   * A message to be processed by the stream.
   * 
   * @param level - What level of verbosity is associated with this message, higher number == more verbosity.
   * @param msg - Message that is to be processed.
   * @param delimiter - Should a delimiter be emitted as well?
   */
  void message(int level, const std::string &msg, bool delimiter = true);

private:
  std::ostream *m_Out;   ///< Stream to print messages to (cout by default.)
  
};

#endif /* MSGSTREAM_H */
