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
 * @file   Verbose.h
 * @author Chuck Sugnet
 * @date   Fri Oct 21 16:54:00 2005
 * 
 * @brief  Class for doing logging and some command line ui.
 */

#ifndef _VERBOSE_H
#define _VERBOSE_H

#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "ProgressHandler.h"
#include "ProgressDot.h"
#include "MsgHandler.h"
#include "MsgStream.h"

/**
 *  Verbose
 * @brief Class for doing logging and some command line ui.
 */
class Verbose {

public:
  /**
   *  Utility class for remembering our parameters of interest.
   */
  class Param {

  public: 
    Param(ProgressHandler *progHandler, MsgHandler *msgHandler, MsgHandler *warnHandler) {
      m_Verbosity = 1;
      m_DotMod = 10;
      m_DotCount = 0;
      m_NewLine = true;
      m_ProHandler.push_back(progHandler);
      m_MsgHandler.push_back(msgHandler);
      m_WarnHandler.push_back(warnHandler);
    }

    std::vector<ProgressHandler *> m_ProHandler; ///< Vector of handlers to be called with progress reports.
    std::vector<MsgHandler *> m_MsgHandler; ///< Vector of handlers for messages (i.e. log files, consoles, dialogs).
    std::vector<MsgHandler *> m_WarnHandler; ///< Vector of handlers for warnings
    bool m_NewLine;   ///< Was a new line outputted after last message?
    int m_Verbosity;  ///< What level of messages is wanted, larger num == more msgs
    int m_DotMod;     ///< How often do we print a dot when dot() is called?
    int m_DotCount;   ///< How many times has dot() been called?
  };
  
  /** 
   * @brief This function gets around the problem of static variable
   * initialization as local static variables work more consistently.
   * @return Param - Our static parameters for this class.
   */
  static Param &getParam() {
    // Avoid weird windows .NET/Forms bug where linking to cout/cerr can cause problems...
  #ifdef _WINDOWS 
    static ProgressDot progHandler(0, NULL);
    static MsgStream  msgHandler(0, NULL);
  #else
    // Normal console mode is ok.
    static ProgressDot progHandler(1, &std::cerr);
    static MsgStream msgHandler(1, &std::cerr);
  #endif
    // By default we just use a normal message handler as the warning handler.
    static Param m_Param(&progHandler, &msgHandler, &msgHandler);
    return m_Param;
  }

  /// @brief Functions to add and remove handlers for communcation functions.
  static void pushProgressHandler(ProgressHandler *handler) {
    getParam().m_ProHandler.push_back(handler);
  }

  static void popProgressHandler() {
    getParam().m_ProHandler.pop_back();
  }

  static void pushMsgHandler(MsgHandler *handler) {
    getParam().m_MsgHandler.push_back(handler);
  }

  static void popMsgHandler() {
    getParam().m_MsgHandler.pop_back();
  }

  static void pushWarnHandler(MsgHandler *handler) {
    getParam().m_WarnHandler.push_back(handler);
  }

  static void popWarnHandler() {
    getParam().m_WarnHandler.pop_back();
  }

  static void progressBegin(int verbosity, const std::string &msg, int total, int dotMod, int maxCalls) {
    std::vector<ProgressHandler *> &m_Handle = getParam().m_ProHandler;
    getParam().m_DotMod = dotMod;
    getParam().m_DotCount = 0;
    // if we have handlers let them know we're beginning.
    for(unsigned int i = 0; i < m_Handle.size(); i++) {
      ProgressHandler *handle = m_Handle[i];
      if(handle->handleAll()) 
        handle->progressBegin(verbosity, msg, maxCalls);
      else
        handle->progressBegin(verbosity, msg, total);
    }
  }
  
  static void progressStep(int verbosity);

  static void progressEnd(int verbosity, const std::string &msg) {
    Param &p = getParam();
    if(verbosity <= p.m_Verbosity) {
      for(unsigned int i = 0; i < p.m_ProHandler.size(); i++) {
        p.m_ProHandler[i]->progressEnd(verbosity, msg);
      }
    }
  }

  /** 
   * @brief Set the level of verbosity desired. 0 == no messages
   * 1 == normal messages, 2,3,4, etc. == more verbose.
   * @param level - level of verbosity desired.
   */  
  static void setLevel(int level) {
    Param &p = getParam();
    for(unsigned int i = 0; i < p.m_ProHandler.size(); i++) {
      p.m_ProHandler[i]->setBaseVerbosity(level);
    }
    for(unsigned int i = 0; i < p.m_MsgHandler.size(); i++) {
      p.m_MsgHandler[i]->setBaseVerbosity(level);
    }
  }

  /** 
   * @brief Print a message to the stream.
   * @param level - What level of verbosity this message should be printed at.
   * @param s - Message to be printed.
   * @param nl - Should a newline be appended to message?
   */
  static void out(int level, const std::string &s, bool nl = true) {
    Param &p = getParam();
    for(unsigned int i = 0; i < p.m_MsgHandler.size(); i++) {
      p.m_MsgHandler[i]->message(level, s, nl);
    }
  }

  /** 
   * @brief Print a warning message.
   * @param level - What level of verbosity this message should be printed at.
   * @param s - Message to be printed.
   * @param nl - Should a newline be appended to message?
   */
  static void warn(int level, const std::string &s, bool nl = true, const std::string prefix = "\nWARNING: ") {
    Param &p = getParam();
    for(unsigned int i = 0; i < p.m_WarnHandler.size(); i++) {
      p.m_WarnHandler[i]->message(level, prefix + s, nl);
    }
  }

};

#endif /* _VERBOSE_H */
