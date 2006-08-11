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
 * @file   RegressionCheck.h
 * @author Chuck Sugnet
 * @date   Tue Apr 25 16:02:08 2006
 * 
 * @brief  Virtual class for checks to be done after a regression test.
 */

#ifndef REGRESSIONCHECK_H
#define REGRESSIONCHECK_H

#include <string>

/**
 * Abstract base class (i.e. interface) for checks to be run during a regression
 * test.
 */
class RegressionCheck {

public:
  
  /** Virtual destructor for virtual class. */
  virtual ~RegressionCheck() {}

  /** 
   * Check the condition expected.
   * @param msg - Message to be optionally set while checking.
   * @return - true if passed, false otherwise.
   */
  virtual bool check(std::string &msg) = 0;
};


#endif /* REGRESSIONCHECK_H */
