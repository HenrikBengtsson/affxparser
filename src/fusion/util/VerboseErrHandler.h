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
 * @file   VerboseErrHandler.h
 * @author Chuck Sugnet
 * @date   Mon Jun 26 12:26:18 PDT 2006
 * 
 * @brief Basic error handler that uses Verbose::warn() and dies by either throwing an
 * exception or calling abort()
 */

#ifndef VERBOSEERRHANDLER_H
#define VERBOSEERRHANDLER_H

#include "util/ErrHandler.h"
#include "util/Except.h"
#include "util/Verbose.h"
//
#include <cstring>
#include <string>
//

/**
 * @brief Basic error handler that prints to cerr and dies by either throwing an
 * exception or calling abort()
 */
class VerboseErrHandler : public ErrHandler {

public:
  VerboseErrHandler(bool doThrow=false, 
					bool verbose=true,
					bool exitOnError=true,
					int  exitOnErrorValue=-1) {
    m_Throw = doThrow;
    m_Verbose = verbose;
    m_ExitOnError = exitOnError;
    m_ExitOnErrorValue = exitOnErrorValue;
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
  if(m_Verbose){
        Verbose::out(1, msg);
	}
	
	if (m_ExitOnError) {	
		#ifdef _WIN32
		// windows needs time for the other process to run, yeild to it.
		Sleep(500);
		#endif
		exit(m_ExitOnErrorValue);
	}

  if (m_Throw) {
    throw Except(msg);
  }

  // We shouldnt hit here; We should be exiting or rethrowing the error.
  // If we do, then something is wrong.
  // abort(1);
  }

  /** Will this error handler be throwing exceptions? */
  virtual bool getThrows() { return m_Throw; }

  /** Will this error handler be throwing exceptions? */
  virtual void setThrows(bool doThrow) { m_Throw= doThrow; }

private:
  bool m_Throw;          ///< Determines if we throw an exception or just call abort()
  bool m_Verbose;        ///< Determines if we call Verbose::out
  bool m_ExitOnError;    ///< Determines if overriding default value to return when exiting on error
  int  m_ExitOnErrorValue;  ///< Value to set if setting the value to return when exiting on errors
};


#endif /* VERBOSEERRHANDLER_H */
