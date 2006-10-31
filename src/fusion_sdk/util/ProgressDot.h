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
 * @file   ProgressDot.h
 * @author Chuck Sugnet
 * @date   Fri Jan 27 14:57:49 2006
 * 
 * @brief ProgressHandler for the console that prints dots to the command line
 * to signify progress.
 */

#ifndef PROGRESSDOT_H
#define PROGRESSDOT_H

//
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
//
#include "ProgressHandler.h"

/**
 * @brief Prints dots as progress is made.
 */
class ProgressDot : public ProgressHandler {

public:

  /** 
   * Constructor that takes an stream for doing outputs and level at
   * which to print messages.
   * 
   * @param out - 
   * @param verbosity 
   * 
   * @return 
   */
  ProgressDot(int verbosity, std::ostream *out) {
    m_Verbosity = verbosity;
    m_Out = out;
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
    // flush any C io before doing our IO
    fflush(NULL); 
    if(verbosity <= m_Verbosity && m_Out != NULL) {
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
  std::ostream *m_Out; ///< Stream to print messages to. Can be NULL to indicate no printing.
  int m_Total;         ///< How many steps total are we expecting?
};

#endif /* PROGRESSDOT_H */
