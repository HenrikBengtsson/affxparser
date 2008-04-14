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
 * @file   LogStream.h
 * @author Chuck Sugnet
 * @date   Tue Mar 21 10:35:22 PST 2006
 * 
 * @brief  Output verbose messages to log file as we go along.
 */

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <iostream>
#include <string>
#include "MsgHandler.h"
#include "util/Util.h"

/**
 * @brief Log messages with timestamps.
 */
class LogStream : public MsgHandler, public ProgressHandler {

public:
  /** Constructor. */
  LogStream(int level, std::ostream *out=NULL) {
    m_Verbosity = level;
    m_Out = out;
  }

  /** 
   * A message to be processed by the stream.
   * 
   * @param level - What level of verbosity is associated with this message, higher number == more verbosity.
   * @param log - Message that is to be processed.
   * @param delimiter - Should a delimiter be emitted as well?
   */
  void message(int level, const std::string &log, bool delimiter = true) {
    std::string timeStr = Util::getTimeStamp();
    if(level <= m_Verbosity && m_Out != NULL) {
      if(!m_Out->good()) {
        Err::errAbort("LogStream::message() - problem writing to stream.");
      }
      if(log != "") 
        (*m_Out) << timeStr << " - ";
      (*m_Out) << log;
      if(delimiter)
        (*m_Out) << std::endl;
    }
    m_Out->flush();
  }

  /** 
   * What level of verbosity is requested. As we're a log, we are going to ignore this
   * and print out lots of stuff.
   * 
   * @param level - Level below which progress messages are printed.
   */
  void setBaseVerbosity(int level) { // do notthing
  }

  /** 
   * Begin a progress reporting. After this initial call the
   * progressStep call will be called N times where N = the parameter
   * total passed in below.
   * 
   * @param verbosity - What level of verbosity is the progress
   * associated with. Higher levels of verbosity produce more
   * messages.
   * @param msg - String message associated with the beginning of this task.
   * @param total - Expected number of times that the progressStep()
   * will be called after this.
   */
  void progressBegin(int verbosity, const std::string &msg, int total) {
    m_Total = total;
    std::string timeStr = Util::getTimeStamp();
    // flush any C io before doing our IO
    fflush(NULL); 
    if(verbosity <= m_Verbosity && m_Out != NULL) {
      (*m_Out) << timeStr << " - ";
      (*m_Out) << msg;
      m_Out->flush();
    }
  }

  /** 
   * This function is called when one unit of work has been done. In
   * general it is expected that the units of work should be roughly
   * equal in size.
   * 
   * @param verbosity - At what verbosity level should this step be
   * displayed.
   */
  void progressStep(int verbosity) {
    if(verbosity <= m_Verbosity && m_Out != NULL) {
      m_Out->put('.');
      m_Out->flush();
    }
  }

  /** 
   * Signals the end of progress report. 
   * 
   * @param verbosity - Level of verbosity associated with this progress report.
   * @param msg - Closing message from calling function.
   */
  void progressEnd(int verbosity, const std::string &msg) {
    if(verbosity <= m_Verbosity && m_Out != NULL) {
      (*m_Out) << msg << std::endl;
      m_Out->flush();
    }
  }


private:
  std::ostream *m_Out;   ///< Stream to print messages to
  int m_Verbosity;     ///< What level of messages is wanted, larger num == more msgs
  int m_Total;         ///< How many steps total are we expecting?  
};

#endif /* LOGSTREAM_H */
