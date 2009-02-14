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
#include "AffxTime.h"

/**
 * @brief Log messages with timestamps.
 */
class LogStream : public MsgHandler, public ProgressHandler {

public:
  /** Constructor. */
  LogStream(int level=3, std::ostream *out=NULL) {
    m_Verbosity = level;
    m_Out = out;
	m_timeStart = time(NULL);
  }

  void setStream(std::ostream *out) {
      m_Out = out;
  }
  void unsetStream(std::ostream *out) {
      m_Out = NULL;
  }

  /** 
   * A message to be processed by the stream.
   * 
   * @param level - What level of verbosity is associated with this message, higher number == more verbosity.
   * @param log - Message that is to be processed.
   * @param delimiter - Should a delimiter be emitted as well?
   */
  virtual void message(int level, const std::string &log, bool delimiter = true) {
//    std::string timeStr = Util::getTimeStamp();
	std::string timeStr = AffxTime::getCurrentTime().getTimeStampString();
    if(level <= m_Verbosity && m_Out != NULL) {
      if(!m_Out->good()) {
        Err::errAbort("AffxLogStream::message() - problem writing to stream.");
      }
      if(log != "") 
        (*m_Out) << timeStr << "\t";
      (*m_Out) << log;
      if(delimiter)
        (*m_Out) << std::endl;
    }
    m_Out->flush();
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
  virtual void progressBegin(int verbosity, const std::string &msg, int total) {
	  m_timeStart = time(NULL);
    m_Total = total;
	std::string timeStr = AffxTime::getCurrentTime().getTimeStampString();
    // flush any C io before doing our IO
    fflush(NULL); 
    if(verbosity <= m_Verbosity && m_Out != NULL) {
      (*m_Out) << timeStr << "\t";
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
  virtual void progressStep(int verbosity) {
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
  virtual void progressEnd(int verbosity, const std::string &msg) {
	time_t timeEnd = time(NULL);
	double dRunTime = (timeEnd - m_timeStart); // time span in seconds.
	std::string str;
	
	if (dRunTime < 60)
	{
        str = msg + "\t";
		str += ::getDouble(dRunTime, 2, true);
		str += " second run time";
    }
	else if (dRunTime < (60 * 60)) 
    {
		str = msg + "\t";
		str += ::getDouble((dRunTime) / 60, 2, true);
		str += " minute run time"; 
    }
	else
	{
        str = msg + "\t";
		str += ::getDouble((dRunTime) / (60 * 60), 2, true);
		str += " hour run time"; 
	}

    if(verbosity <= m_Verbosity && m_Out != NULL) {
		(*m_Out) << str << std::endl;
      m_Out->flush();
    }
  }

  /** 
   * What level of verbosity is requested. As we're a log, we are going to ignore this
   * and print out lots of stuff.
   * 
   * @param level - Level below which progress messages are printed.
   */
  void setBaseVerbosity(int level) { // do notthing
  }

protected:
  std::ostream *m_Out;   ///< Stream to print messages to
  int m_Verbosity;     ///< What level of messages is wanted, larger num == more msgs
  int m_Total;         ///< How many steps total are we expecting?  
  time_t m_timeStart;
};

#endif /* LOGSTREAM_H */
