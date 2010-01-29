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
 * @file   Err.cpp
 * @author Chuck Sugnet
 * @date   Wed May  4 14:57:32 2005
 * 
 * @brief Some utility error functions, currently prints message and aborts()
 * 
 */

#include "util/Err.h"
//
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <string>
#include <vector>
//

  /** 
   * @brief This function gets around the problem of static variable
   * initialization as local static variables work more consistently.
   * @return Param - Our static parameters for this class.
   */
  Err::Param &Err::getParam() {
    static Param m_Param;
    return m_Param;
  }

  /** 
   * Print the message supplied and abort program.
   * @param msg - Message for user about what went wrong.
   * @param prefix - Prefix to add to the error message.
   */
  void Err::errAbort(const std::string &msg, const std::string prefix){ // throw (Except) {
    unsigned int size = getParam().m_ErrHandlers.size();
    assert(size > 0);
    std::string errMsg = prefix + msg;;
    // GUI's do not like the newline
    if(getParam().m_NewLineOnError) {
        errMsg = "\n" + errMsg;
    }
    getParam().m_ErrHandlers[size - 1]->handleError(errMsg);
  }

  /** 
   * Add a new handler for errors.
   * @param handler - Pointer to handler to call when things go wrong.
   */
  void Err::pushHandler(ErrHandler *handler) {
    getParam().m_ErrHandlers.push_back(handler);
  }

  /** 
   * Pop off the current handler and return it for cleanup, etc.
   * @return - last handler pushed onto the stack.
   */
  ErrHandler *Err::popHandler() {
    unsigned int count = getParam().m_ErrHandlers.size();
    if(count < 1) {
      Err::errAbort("Err::popHandler() - can't pop error handler when there aren't any left.");
    }
    ErrHandler *handler = getParam().m_ErrHandlers[count - 1];
    getParam().m_ErrHandlers.pop_back();
    return handler;
  }

  /** 
   * Toggle whether we are throwing exceptions or exiting on errors.
   * 
   * @param doThrow - if true exceptions will be thrown, if false exit(1) called.
   */
  void Err::setThrowStatus(bool doThrow) {
    VerboseErrHandler *handler = new VerboseErrHandler(doThrow);
    pushHandler(handler);
  }

  /** 
   * Configure new error handler
   * 
   * @param doThrow - should an exception be thrown, or call exit
   * @param verbose - should the error handler call verbose methods
   */
  void Err::configureErrHandler(bool doThrow, bool verbose) {
    VerboseErrHandler *handler = new VerboseErrHandler(doThrow, verbose);
    pushHandler(handler);
  }

  /** 
   * Toggle whether we are throwing exceptions or exiting on errors.
   * 
   * @param doThrow - if true exceptions will be thrown, if false exit(1) called.
   */
  void Err::setNewLineOnError(bool newline) {
    getParam().m_NewLineOnError = newline;
  }

void Err::apt_err_assert(const std::string& file,int line,
                         const std::string& condstr,
                         bool cond,
                         const std::string& msg)
{
  if (!cond) {
    std::string errmsg=file+":"+ToStr(line)+": '"+condstr+"'==false";
    if (msg!="") {
      errmsg+=" "+msg;
    }
    errAbort(errmsg);
  }
}

void Err::apt_err_abort(const std::string& file,int line,
                        const std::string& msg)
{
  std::string errmsg=file+":"+ToStr(line)+": "+msg;
  errAbort(errmsg);
}
