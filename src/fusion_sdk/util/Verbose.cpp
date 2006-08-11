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

#include "Verbose.h"

using namespace std;

/**
 * @brief Print a dot out to let the user know we are still alive
 * and making progress.
 * @param verbosity - What level of verbosity this message should be printed at.
 */
void Verbose::progressStep(int verbosity) {
  Param &p = getParam();
  
  if(verbosity <= p.m_Verbosity) {
    p.m_DotCount--;
    /* Check to see if it is time to call the step report function yet. */
    for(unsigned int i = 0; i < p.m_ProHandler.size(); i++) {
      if(p.m_DotCount <= 0 || p.m_ProHandler[i]->handleAll()) {
        p.m_ProHandler[i]->progressStep(verbosity);
      }
    }
    if(p.m_DotCount <= 0) {
      p.m_DotCount = p.m_DotMod; // reset the counter
    }
  }
}


