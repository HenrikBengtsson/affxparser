////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
////////////////////////////////////////////////////////////////

/**
 * @file   VerboseErrHandler.h
 * @author Chuck Sugnet
 * @date   Mon Jun 26 12:26:18 PDT 2006
 * 
 * @brief Basic error handler that uses Verbose::warn() and dies by either throwing an
 * exception or calling abort()
 */

#ifndef VERBOSEERRHANDLER_H
#define VERBOSEERRHANDLER_H

#include <string>
#include "ErrHandler.h"
#include "Verbose.h"
#include "Except.h"

/**
 * @brief Basic error handler that prints to cerr and dies by either throwing an
 * exception or calling abort()
 */
class VerboseErrHandler : public ErrHandler {

public:
  VerboseErrHandler(bool doThrow) { 
    m_Throw = doThrow;
  }

  /** Virtual destructor for a virtual class. */
  virtual ~VerboseErrHandler() {}

  /** 
   * When something terminal happens this function is called. This is
   * the place to do any last minute cleanup, log writing, and
   * communication with the user about what went wrong. The
   * implementation of this function should either exit the program or
   * throw an exception.
   * 
   * @param msg - Message about what went wrong.
   */
  virtual void handleError(const std::string &msg) {
    Verbose::warn(0, msg, true, "");
    if(m_Throw) {
      throw Except(msg);
    }
    else {
      exit(1);
    }
  }

private:
  bool m_Throw;          ///< Determines if we throw an exception or just call abort()
};


#endif /* VERBOSEERRHANDLER_H */
