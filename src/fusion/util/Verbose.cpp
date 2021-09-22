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
 * @file   Verbose.cpp
 * @author Chuck Sugnet
 * @date   Fri Oct 21 17:06:17 2005
 *
 * @brief  Class for doing logging and some command line ui.
 */

//
#include "calvin_files/utils/src/StringUtils.h"
#include "portability/affy-system-api.h"
#include "util/Fs.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <cstdlib>
#include <fstream>

using namespace std;
using namespace affymetrix_calvin_utilities;

/**
 * @brief This function will create log file for test 
 * purposes.  
 * The path and name of the log file needs to be specified
 * prior to use (see fname).
 */

/// a static pointer to the fstream which will be used for "emergency debugging output".
std::fstream* em_out_fstream;

void em_out(const std::string& msg)
{
  // if the stream isnt setup, then open it.
	if (em_out_fstream==NULL) {
		em_out_fstream=new std::fstream();
		std::string fname="EM-OUT-"+ToStr((int)getpid())+"-"+ToStr((int)rand())+".log";
// PATCH: open() may not exist for wstrings, but does for strings.
//   To avoid compilation errors in Windows, we simply skip the
//   StringUtils::ConvertMBSToWCS() call. Hopefully ok.  If not,
//   this "emergency debugging output" function shouldn't be called
//   anyways. /HB 2012-08-29
#ifdef _MSC_VER_HIDE //HB
		std::wstring wfname=StringUtils::ConvertMBSToWCS(fname);
		em_out_fstream->open(wfname.c_str(),ios::out);
#else
    em_out_fstream->open(fname.c_str(),ios::out);
#endif
	}
  
  // now send the message...
	*em_out_fstream << msg << "\n";
  // ...and be sure the output goes to the file right away, as
  // this function is used when we have a hard to debug crash.
	em_out_fstream->flush();
}


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
  static std::ostream *out = NULL;
  if (std::cerr.good()) {
    out = &std::cerr;
  }
#ifdef _NO_STD_OUT
  static ProgressDot progHandler(0, NULL);
  static MsgStream  msgHandler(0, NULL);
#else
  // Normal console mode is ok.
  static ProgressDot progHandler(1, out);
  static MsgStream msgHandler(1, out);
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
  //  pushWarnHandler(handler); // @todo - Make this simpler. Too many push/pop calls currently
}

void Verbose::popMsgHandler() {
  getParam().m_MsgHandler.pop_back();
  //  popWarnHandler(); // @todo - Make this simpler. Too many push/pop calls currently
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

void Verbose::removeMsgHandler(MsgHandler *h) {
  Verbose::Param &p = getParam();
  removeMsgHandler(p.m_MsgHandler, h);
  removeMsgHandler(p.m_WarnHandler, h);
}

void Verbose::removeProgressHandler(ProgressHandler *h) {
  Verbose::Param &p = getParam();
  removeProgressHandler(p.m_ProHandler, h);
}

void Verbose::removeMsgHandler(std::vector<MsgHandler *> &vec, MsgHandler *h) {
  vector<MsgHandler *>::iterator i;
  for (i = vec.begin(); i < vec.end(); i++) {
    if (*i == h) {
      vec.erase(i);
      break;
    }
  }
} 

void Verbose::removeProgressHandler(std::vector<ProgressHandler *> &vec, ProgressHandler *h) {
  vector<ProgressHandler *>::iterator i;
  for (i = vec.begin(); i < vec.end(); i++) {
    if (*i == h) {
      vec.erase(i);
      break;
    }
  }
} 

void Verbose::removeDefault() {
  Verbose::Param &p = getParam();
  if (!p.m_ProHandler.empty() && p.m_ProHandler[0] == p.m_ProgDefault) {
    p.m_ProHandler.erase(p.m_ProHandler.begin());
    p.m_ProgDefault = NULL;
  }
  if (!p.m_MsgHandler.empty() && p.m_MsgHandler[0] == p.m_MsgDefault) {
    p.m_MsgHandler.erase(p.m_MsgHandler.begin());
    p.m_MsgDefault = NULL;
  }
  if (!p.m_WarnHandler.empty() && p.m_WarnHandler[0] == p.m_WarnDefault) {
    p.m_WarnHandler.erase(p.m_WarnHandler.begin());
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
  p.m_Verbosity = level;
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
  // this forces our messages out to the OS, so we know what is going on.
  // If someone (errabort) calls exit, the messages might be left in our buffers and not written out.  
  fflush(NULL); 
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
