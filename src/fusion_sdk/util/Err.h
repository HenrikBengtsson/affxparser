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
 * @file   Err.h
 * @author Chuck Sugnet
 * @date   Wed May  4 14:57:32 2005
 * 
 * @brief Some utility error functions, currently prints message and aborts()
 * 
 */

#ifndef ERR_H
#define ERR_H

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <new>
#include "stdlib.h"
#include "Except.h"
#include "Verbose.h"
#include "ErrHandler.h"
#include "VerboseErrHandler.h"
/**
 *  Err
 * @brief Common abort with message function for programs to call.
 */
class Err {

 public: 
  /** Utility class for having static parameters across all instances. */
  class Param {
    public:
    /** By default add an error handler that calls verbose. */
    Param() {
      VerboseErrHandler *handler = new VerboseErrHandler(false);
      m_ErrHandlers.push_back(handler);
      m_Throw = false;
    }
    
    /** Cleanup any error handlers that are still around. */
    ~Param() { 
      for(unsigned int i = 0; i < m_ErrHandlers.size(); i++) {
        delete m_ErrHandlers[i];
      }
    }

    std::vector<ErrHandler *> m_ErrHandlers; ///< vector of handlers
    bool m_Throw; ///< Should we throw an exception rather than exit(1)?
  };
    
  /** 
   * @brief This function gets around the problem of static variable
   * initialization as local static variables work more consistently.
   * @return Param - Our static parameters for this class.
   */
  static Param &getParam() {
    static Param m_Param;
    return m_Param;
  }

  /** 
   * Print the message supplied and abort program.
   * @param msg - Message for user about what went wrong.
   */
  static void errAbort(const std::string &msg, const std::string prefix = "\nFATAL ERROR: "){ // throw (Except) {
    unsigned int size = getParam().m_ErrHandlers.size();
    assert(size > 0);
    getParam().m_ErrHandlers[size - 1]->handleError(prefix + msg);
  }

  /** 
   * Add a new handler for errors.
   * @param handler - Pointer to handler to call when things go wrong.
   */
  static void pushHandler(ErrHandler *handler) {
    getParam().m_ErrHandlers.push_back(handler);
  }

  /** 
   * Pop off the current handler and return it for cleanup, etc.
   * @return - last handler pushed onto the stack.
   */
  static ErrHandler *popHandler() {
    unsigned int count = getParam().m_ErrHandlers.size();
    if(count < 1) {
      Err::errAbort("Err::popHandler() - can't pop error handler when there aren't any left.");
    }
    ErrHandler *handler = getParam().m_ErrHandlers[count - 1];
    getParam().m_ErrHandlers.pop_back();
    return handler;
  }

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
   * Toggle whether we are throwing exceptions or exiting on errors.
   * 
   * @param doThrow - if true exceptions will be thrown, if false exit(1) called.
   */
  static void setThrowStatus(bool doThrow) {
    VerboseErrHandler *handler = new VerboseErrHandler(doThrow);
    pushHandler(handler);
  }

};

#endif /* ERR_H */
