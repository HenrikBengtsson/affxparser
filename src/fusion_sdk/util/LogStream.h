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
 * @file   LogStream.h
 * @author Chuck Sugnet
 * @date   Tue Mar 21 10:35:22 PST 2006
 * 
 * @brief  Output verbose messages to log file as we go along.
 */

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

//
#include "portability/apt-win-dll.h"
#include "util/AffxTime.h"
#include "util/MsgHandler.h"
#include "util/Util.h"
//
#include <cstring>
#include <iostream>
#include <string>

/**
 * @brief Log messages with timestamps.
 */
class APTLIB_API LogStream : public MsgHandler, public ProgressHandler {

public:
  /** Constructor. */
  LogStream(int verbosity=3, std::ostream *out=NULL, bool profile=true);
  ~LogStream();
  void setStream(std::ostream *out);
  void unsetStream(std::ostream *out);
  virtual void message(int verbosity, const std::string &log, bool delimiter = true);
  virtual void progressBegin(int verbosity, const std::string &msg, int total);
  virtual void progressStep(int verbosity);
  virtual void progressEnd(int verbosity, const std::string &msg);
  void setBaseVerbosity(int verbosity);

protected:
  std::string profileString();

protected:
  int m_Verbosity;                          ///< What level of messages is wanted, larger num == more msgs
  std::ostream *m_Out;                      ///< Stream to print messages to
  bool m_Profile;                           ///< Should we report profile info
  std::vector<time_t> m_ProgressTimeStart;  ///< Start times for progress segments
  std::vector<uint64_t>  m_ProgressTotal;   ///< Number of times step is expected to be called

};

#endif /* LOGSTREAM_H */
