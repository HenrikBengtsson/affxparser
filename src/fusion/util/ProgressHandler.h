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
 * @file   ProgressHandler.h
 * @author Chuck Sugnet
 * @date   Fri Jan 27 14:31:52 2006
 * 
 * @brief Abtract interface for a class which interprets progress. Register your
 * ProgressHandler with the Verbose class to handle progress calls.
 * 
 */


#ifndef PROGRESSHANDLER_H
#define PROGRESSHANDLER_H

#include <cstring>
#include <string>
//

/**
 * @brief An abstract class for handling Progress updates.
 */
class ProgressHandler {

public:

  /** Virtual destructor for a virtual class. */
  virtual ~ProgressHandler() {}

  /** Does this object handle all the calls or just want signals every
      dotMod */
  virtual bool handleAll() { return false; }

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
  virtual void progressBegin(int verbosity, const std::string &msg, int total) = 0;

  /** 
   * This function is called when one unit of work has been done. In
   * general it is expected that the units of work should be roughly
   * equal in size.
   * 
   * @param verbosity - At what verbosity level should this step be
   * displayed.
   */
  virtual void progressStep(int verbosity) = 0;

  /** 
   * Signals the end of progress report. 
   * 
   * @param verbosity - Level of verbosity associated with this progress report.
   * @param msg - Closing message from calling function.
   */
  virtual void progressEnd(int verbosity, const std::string &msg) = 0;

  /** 
   * What level of verbosity is requested. This is a hint to the class and can
   * be ignored if desired.
   * 
   * @param level - Level below which progress messages are printed.
   */
  virtual void setBaseVerbosity(int level) { m_Verbosity = level; }

protected:
  
  int m_Verbosity;  ///< What level of verbosity is processed, higher == more messages.

};

#endif /* PROGRESSHANDLER_H */
