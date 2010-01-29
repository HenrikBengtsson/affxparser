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
 * @file   Verbose.cpp
 * @author Chuck Sugnet
 * @date   Fri Oct 21 17:06:17 2005
 *
 * @brief  Class for doing logging and some command line ui.
 */

#include "util/Verbose.h"

using namespace std;

/**
 * @brief Print a dot out to let the user know we are still alive
 * and making progress.
 * @param verbosity - What level of verbosity this message should be printed at.
 */
void Verbose::progressStep(int verbosity) {
  Param &p = getParam();
 
  assert(p.m_DotCount.size() > 0);
  assert(p.m_DotMod.size() > 0);

  if(verbosity <= p.m_Verbosity) {
    p.m_DotCount.back()--;
    /* Check to see if it is time to call the step report function yet. */
    for(unsigned int i = 0; i < p.m_ProHandler.size(); i++) {
      if(p.m_DotCount.back() <= 0 || p.m_ProHandler[i]->handleAll()) {
        p.m_ProHandler[i]->progressStep(verbosity);
      }
    }
    if(p.m_DotCount.back() <= 0) {
      p.m_DotCount.back() = p.m_DotMod.back(); // reset the counter
    }
  }
}

/** 
 * @brief This function gets around the problem of static variable
 * initialization as local static variables work more consistently.
 * @return Param - Our static parameters for this class.
 */
Verbose::Param &Verbose::getParam() {

  // Avoid weird windows .NET/Forms bug where linking to cout/cerr can cause problems...
#ifdef _NO_STD_OUT
  static ProgressDot progHandler(0, NULL);
  static MsgStream  msgHandler(0, NULL);
#else
  // Normal console mode is ok.
  static ProgressDot progHandler(1, &std::cerr);
  static MsgStream msgHandler(1, &std::cerr);
#endif

  // By default we just use a normal message handler as the warning handler.
  static Verbose::Param m_Param(&progHandler, &msgHandler, &msgHandler);
  return m_Param;
}

/// @brief Functions to add and remove handlers for communcation functions.
void Verbose::pushProgressHandler(ProgressHandler *handler) {
  getParam().m_ProHandler.push_back(handler);
}

void Verbose::popProgressHandler() {
  getParam().m_ProHandler.pop_back();
}

void Verbose::pushMsgHandler(MsgHandler *handler) {
  getParam().m_MsgHandler.push_back(handler);
}

void Verbose::popMsgHandler() {
  getParam().m_MsgHandler.pop_back();
}

void Verbose::pushWarnHandler(MsgHandler *handler) {
  getParam().m_WarnHandler.push_back(handler);
}

void Verbose::popWarnHandler() {
  getParam().m_WarnHandler.pop_back();
}

void Verbose::progressBegin(int verbosity, const std::string &msg, int total, int dotMod, int maxCalls) {
  std::vector<ProgressHandler *> &m_Handle = getParam().m_ProHandler;
  getParam().m_DotMod.push_back(dotMod);
  getParam().m_DotCount.push_back(0);
  // if we have handlers let them know we're beginning.
  for(unsigned int i = 0; i < m_Handle.size(); i++) {
    ProgressHandler *handle = m_Handle[i];
    if(handle->handleAll()) 
      handle->progressBegin(verbosity, msg, maxCalls);
    else
      handle->progressBegin(verbosity, msg, total);
  }
}


void Verbose::progressEnd(int verbosity, const std::string &msg) {
  Verbose::Param &p = getParam();

  assert(p.m_DotCount.size() > 0);
  assert(p.m_DotMod.size() > 0);

  p.m_DotMod.pop_back();
  p.m_DotCount.pop_back();
  if(verbosity <= p.m_Verbosity) {
    for(unsigned int i = 0; i < p.m_ProHandler.size(); i++) {
      p.m_ProHandler[i]->progressEnd(verbosity, msg);
    }
  }
}

/** 
 * @brief Set whether or not output messages are logged
 *        useful to turn off output when catching expected errors
 *
 * @param output - true or false
 */  
void Verbose::setOutput(bool output) {
  Verbose::Param &p = getParam();
  p.m_Output = output;
}

/** 
 * @brief Set the level of verbosity desired. 0 == no messages
 * 1 == normal messages, 2,3,4, etc. == more verbose.
 * @param level - level of verbosity desired.
 */  
void Verbose::setLevel(int level) {
  Verbose::Param &p = getParam();
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
void Verbose::out(int level, const std::string &s, bool nl) {
  Verbose::Param &p = getParam();
  if(p.m_Output) {
      for(unsigned int i = 0; i < p.m_MsgHandler.size(); i++) {
          p.m_MsgHandler[i]->message(level, s, nl);
      }
  }
}

/** 
 * @brief Print a warning message.
 * @param level - What level of verbosity this message should be printed at.
 * @param s - Message to be printed.
 * @param nl - Should a newline be appended to message?
 */
void Verbose::warn(int level, const std::string &s, bool nl, const std::string prefix) {
  Verbose::Param &p = getParam();
  if(p.m_Output) {
      for(unsigned int i = 0; i < p.m_WarnHandler.size(); i++) {
          p.m_WarnHandler[i]->message(level, prefix + s, nl);
      }
  }
}
