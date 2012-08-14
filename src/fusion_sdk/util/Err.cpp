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
 * @file   Err.cpp
 * @author Chuck Sugnet
 * @date   Wed May  4 14:57:32 2005
 * 
 * @brief Some utility error functions, currently prints message and aborts()
 * 
 */

//
#include "util/Err.h"
//
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

std::string Err::m_errorPrefix="FATAL ERROR:";

void Err::setErrorPrefix(const std::string& prefix) {
  m_errorPrefix=prefix;
}

  /** 
   * @brief This function gets around the problem of static variable
   * initialization as local static variables work more consistently.
   * @return Param - Our static parameters for this class.
   */
  Err::Param &Err::getParam() {
    static Param m_Param;
    return m_Param;
  }

  void Err::errAbort(const std::string &msg) {
    errAbort(msg,m_errorPrefix);
  }

  /** 
   * Print the message supplied and abort program.
   * @param msg - Message for user about what went wrong.
   * @param prefix - Prefix to add to the error message.
   */
  void Err::errAbort(const std::string& msg,const std::string& prefix){ // throw (Except) {
    unsigned int size = getParam().m_ErrHandlers.size();
    if(size <= 0) {
      cout << "Can't have no error handlers." << endl;
    }
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
   * Give the current error handler a hint about whether or not to
   * throw exceptions.  Note that this is just a hint and that if a
   * specifical behavior is really required it is best to push a
   * custom error handler onto the stack.
   * 
   * @param doThrow - if true requesting exceptions to be thrown, if false not to be thrown
   */
  void Err::setThrowStatus(bool doThrow) {
    int size = getParam().m_ErrHandlers.size();
    for (int i = 0; i < size; i++) {
     ErrHandler *handler = getParam().m_ErrHandlers[i];
     handler->setThrows(doThrow);
    }
  }

  /** 
   * Return true if curerntly configured to throw exceptions on error, false otherwise.
   */ 
  bool Err::getThrowStatus() {
      ErrHandler *handler = getParam().m_ErrHandlers.back();
      return handler->getThrows();
  }

  /** 
   * Configure new error handler
   * 
   * @param doThrow - should an exception be thrown, or call exit
   * @param verbose - should the error handler call verbose methods
   */
   void Err::configureErrHandler(bool doThrow, bool verbose, bool exitOnError,
					int  exitOnErrorValue) {  
    VerboseErrHandler *handler = new VerboseErrHandler(doThrow, verbose, exitOnError, exitOnErrorValue);
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

/**
  * Toggle whether to set the value returned when exiting on errors
  * This is primarily used to suppress Windows OS handling of
  * return values indicating an exe did not terminate successfully

  * if val is true, the value in setExitOnErrorValue will be
  * returned.
  */
void Err::setExitOnError(bool val) {
	int size = getParam().m_ErrHandlers.size();

  if (size==0) {
    Verbose::warn(1,"Err::setExitOnError(): called without a handler present.");
  }

  for (int i = 0; i < size; i++) {
		ErrHandler *handler = getParam().m_ErrHandlers[i];
		handler->setExitOnError(val);
	}   
}

 /**
  * Value to return when the flag setExitOnError is true and
  * exiting on errors
  */
void Err::setExitOnErrorValue(int val) {
  int size = getParam().m_ErrHandlers.size();

  if (size==0) {
    Verbose::warn(1,"Err::setExitOnErrorValue(): called without a handler present.");
  }

  for (int i = 0; i < size; i++) {
		ErrHandler *handler = getParam().m_ErrHandlers[i];
		handler->setExitOnErrorValue(val);
	}
}
