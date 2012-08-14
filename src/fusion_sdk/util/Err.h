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
 * @file   Err.h
 * @author Chuck Sugnet
 * @date   Wed May  4 14:57:32 2005
 *
 * @brief Some utility error functions, currently prints message and aborts()
 *
 */

#ifndef _UTIL_ERR_H_
#define _UTIL_ERR_H_

//
#include "portability/apt-win-dll.h"
#include "util/Convert.h"
#include "util/ErrHandler.h"
#include "util/Except.h"
#include "util/Verbose.h"
#include "util/VerboseErrHandler.h"
//
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <string>
#include <vector>
//

/// Calls Err::apt_err_abort with the filename and linenumber set.
#define APT_ERR_ABORT(_Why) { Err::apt_err_abort(__FILE__,__LINE__,_Why); }
/// Calls Err::apt_err_assert with the filename and linenumber set.
/// We want to avoid evaluating the message unless the condition is false.
// this could be cleaned up to avoid the "0".
#define APT_ERR_ASSERT(_Cond,_Msg) { if (!(_Cond)) { Err::apt_err_assert(__FILE__,__LINE__,#_Cond,0,_Msg); } }

/**
 *  Err
 * @brief Common abort with message function for programs to call.
 */
class APTLIB_API Err {

 public:

  static std::string m_errorPrefix;
  static void setErrorPrefix(const std::string& prefix);

  /** Utility class for having static parameters across all instances. */
  class Param {
    public:
    /** By default add an error handler that calls verbose. */
    Param() {
      VerboseErrHandler *handler = new VerboseErrHandler();
      m_ErrHandlers.push_back(handler);
      m_NewLineOnError = true;
    }

    /** Cleanup any error handlers that are still around. */
    ~Param() {
      for(unsigned int i = 0; i < m_ErrHandlers.size(); i++) {
        delete m_ErrHandlers[i];
      }
    }

    std::vector<ErrHandler *> m_ErrHandlers; ///< vector of handlers
    bool m_NewLineOnError; ///< Should we print a newline before error message?
  };

  /**
   * @brief This function gets around the problem of static variable
   * initialization as local static variables work more consistently.
   * @return Param - Our static parameters for this class.
   */
  static Param &getParam();

  /**
   * Print the message supplied and abort program.
   * @param msg - Message for user about what went wrong.
   * @param prefix - Prefix to add to the error message.
   */
  static void errAbort(const std::string &msg, const std::string& prefix);
  static void errAbort(const std::string &msg);

  /**
   * Add a new handler for errors.
   * @param handler - Pointer to handler to call when things go wrong.
   */
  static void pushHandler(ErrHandler *handler);

  /**
   * Pop off the current handler and return it for cleanup, etc.
   * @return - last handler pushed onto the stack.
   */
  static ErrHandler *popHandler();

  /**
   * errAbort based version of assert with an error message. Try to
   * use this method in conjunction with a const char * message
   * (rather than constructing a string every time) when writing a
   * check that will be called lots of times.
   *
   * @param passed - Condition to be checked.
   * @param msg - Error message if failed.
   */
  inline static void check(bool passed, const char *msg) {
    if(!passed) {
      errAbort(msg);
    }
  }

  /**
   * errAbort based version of assert with an error message.
   *
   * @param passed - Condition to be checked.
   * @param msg - Error message if failed.
   */
  inline static void check(bool passed, const std::string &msg) {
    if(!passed) {
      errAbort(msg);
    }
  }

  /**
   * Give the current error handler a hint about whether or not to
   * throw exceptions.  Note that this is just a hint and that if a
   * specifical behavior is really required it is best to push a
   * custom error handler onto the stack.
   *
   * @param doThrow - if true requesting exceptions to be thrown, if false not to be thrown
   */
  static void setThrowStatus(bool doThrow);

  /**
   * Return true if curerntly configured to throw exceptions on error, false otherwise.
   */
  static bool getThrowStatus();

  /**
   * Configure new error handler
   *
   * @param doThrow - should an exception be thrown, or call exit
   * @param verbose - should the error handler call verbose methods
   */
  static void configureErrHandler(bool doThrow, bool verbose, bool exitOnError=false,
					int  exitOnErrorValue=-1);

  /**
   * Toggle whether we are throwing exceptions or exiting on errors.
   *
   * @param doThrow - if true exceptions will be thrown, if false exit(1) called.
   */
  static void setNewLineOnError(bool newline);

  /// @brief     The function called by the APT_ERR_ASSERT macro
  /// @param     file      Filled in from __FILE__
  /// @param     line      Filled in from __LINE__
  /// @param     condstr   The string form of the conditional
  /// @param     cond      The evaluated conditional
  /// @param     msg       The message to report on an error
  static void apt_err_assert(const std::string& file,int line,
                             const std::string& condstr,
                             bool cond,
                             const std::string& msg);

  /// @brief     The function called by the APT_ERR_ABORT macro.
  /// @param     file      Filled in from __FILE__
  /// @param     line      Filled in from __LINE__
  /// @param     msg       The message to abort with
  static void apt_err_abort(const std::string& file,int line,
                            const std::string& msg);
/**
  * Toggle whether to set the value returned when exiting on errors
  * This is primarily used to suppress Windows OS handling of
  * return values indicating an exe did not terminate successfully

  * if val is true, the value in setExitOnErrorValue will be
  * returned.
  */
static void setExitOnError(bool val);
 
/**
  * Value to return when the flag setExitOnError is true and
  * exiting on errors
 */
static void setExitOnErrorValue(int val);

};

#endif /* _UTIL_ERR_H_ */
