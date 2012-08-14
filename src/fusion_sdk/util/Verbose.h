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
 * @file   Verbose.h
 * @author Chuck Sugnet
 * @date   Fri Oct 21 16:54:00 2005
 * 
 * @brief  Class for doing logging and some command line ui.
 */

#ifndef _VERBOSE_H
#define _VERBOSE_H

//
#include "portability/apt-win-dll.h"
#include "util/MsgHandler.h"
#include "util/MsgStream.h"
#include "util/ProgressDot.h"
#include "util/ProgressHandler.h"
//
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


void em_out(const std::string& msg);


/**
 *  Verbose
 * @brief Class for doing logging and some command line ui.
 */
class APTLIB_API Verbose {

public:
  /**
   *  Utility class for remembering our parameters of interest.
   */
  class Param {

  public: 
    Param(ProgressHandler *progHandler, MsgHandler *msgHandler, MsgHandler *warnHandler) {
      m_Verbosity = 1;
      m_NewLine = true;
      m_Output = true;
      m_ProHandler.push_back(progHandler);
      m_MsgHandler.push_back(msgHandler);
      m_WarnHandler.push_back(warnHandler);
      m_ProgDefault = progHandler;
      m_MsgDefault = msgHandler;
      m_WarnDefault = warnHandler;
    }

    ProgressHandler *m_ProgDefault;
    MsgHandler *m_MsgDefault;
    MsgHandler *m_WarnDefault;
    std::vector<ProgressHandler *> m_ProHandler; ///< Vector of handlers to be called with progress reports.
    std::vector<MsgHandler *> m_MsgHandler; ///< Vector of handlers for messages (i.e. log files, consoles, dialogs).
    std::vector<MsgHandler *> m_WarnHandler; ///< Vector of handlers for warnings
    bool m_NewLine;   ///< Was a new line outputted after last message?
    bool m_Output;    ///< Do we report messages
    int m_Verbosity;  ///< What level of messages is wanted, larger num == more msgs
    std::vector<int> m_DotMod;     ///< How often do we print a dot when dot() is called?
    std::vector<int> m_DotCount;   ///< How many times has dot() been called?
  };
  
  /** 
   * @brief This function gets around the problem of static variable
   * initialization as local static variables work more consistently.
   * @return Param - Our static parameters for this class.
   */
  static Param &getParam();
  
  /// @brief Functions to add and remove handlers for communcation functions.
  static void pushProgressHandler(ProgressHandler *handler);
  static void popProgressHandler();
  static void removeProgressHandler(ProgressHandler *h);
  static void removeProgressHandler(std::vector<ProgressHandler *> &vec, ProgressHandler *h);

  static void pushMsgHandler(MsgHandler *handler);
  static void popMsgHandler();
  static void removeMsgHandler(MsgHandler *handler);
  static void removeMsgHandler(std::vector<MsgHandler *> &vec, MsgHandler *h);

  static void pushWarnHandler(MsgHandler *handler);
  static void popWarnHandler();

  static void progressBegin(int verbosity, const std::string &msg, int total, int dotMod, int maxCalls);
  static void progressStep(int verbosity);
  static void progressEnd(int verbosity, const std::string &msg);

  static void removeDefault();

  /** 
   * @brief Set whether or not output messages are logged
   *        useful to turn off output when catching expected errors
   *
   * @param output - true or false
   */  
  static void setOutput(bool output);

  /** 
   * @brief Set the level of verbosity desired. 0 == no messages
   * 1 == normal messages, 2,3,4, etc. == more verbose.
   * @param level - level of verbosity desired.
   */  
  static void setLevel(int level);

  /** 
   * @brief Print a message to the stream.
   * @param level - What level of verbosity this message should be printed at.
   * @param s - Message to be printed.
   * @param nl - Should a newline be appended to message?
   */
  static void out(int level, const std::string &s, bool nl = true);

  /** 
   * @brief Print a warning message.
   * @param level - What level of verbosity this message should be printed at.
   * @param s - Message to be printed.
   * @param nl - Should a newline be appended to message?
   */
  static void warn(int level, const std::string &s, bool nl = true, const std::string prefix = "\nWARNING: ");

};

#endif /* _VERBOSE_H */
