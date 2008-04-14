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
 * @file   MatrixCheck.h
 * @author Chuck Sugnet
 * @date   Tue Apr 25 16:12:22 2006
 * 
 * @brief Class for doing a check of two matrix files after regression run.
 */
#ifndef MATRIXCHECK_H
#define MATRIXCHECK_H

#include <assert.h>
#include <string>
#include <stdlib.h>
#include "Util.h"
#include "Verbose.h"
#include "RegressionCheck.h"

/**
 * Class for testing that two files are the same +/- some
 * epsilon. Currently only supports tab separated matrixes of
 * numerical data.
 */
class MatrixCheck : public RegressionCheck {

public:

  /** 
   * Constructor.
   * 
   * @param generated - File created by application being tested.
   * @param gold - File believed to be "truth"
   * @param eps - Maximum abosolute difference from truth acceptable.
   * @param rowSkip - Number of rows to skip before comparing.
   * @param colSkip - Number of columns to skip before comparing.
   * @param matchNames - Should we try to match the names using
   *                     first column? Useful when the files aren't
   *                     necessarily in same order.
   * @param allowedMisMatch - How many can we get wrong before failing?
   */
  MatrixCheck(const std::string &generated, const std::string &gold, double eps, 
            int rowSkip, int colSkip, bool matchNames, unsigned int allowedMisMatch) :
    m_Generated(generated), m_Gold(gold), m_Epsilon(eps),
    m_RowSkip(rowSkip), m_ColSkip(colSkip), m_MatchNames(matchNames),
    m_AllowedMisMatch(allowedMisMatch) {}

  /** 
   * Check to make sure that two files are the same +/- some epsilon. 
   * @param msg - Fills in an error message if test fails, empty string otherwise.
   * @return - Returns true if files are close enough, false otherwise.
   */
  bool check(std::string &msg) {
    std::string generated(m_Generated), gold(m_Gold);
    msg = "";
    bool success = true;
    // Fix pathnames to work for this platform.
    generated = Util::getPathName(generated.c_str());
    gold = Util::getPathName(gold.c_str());
    /* Santiy checks. */
    if(!Util::fileReadable(generated.c_str())) {
      msg += "Can't open file: " + ToStr(generated.c_str()) + " to read.";
      return false;
    }
    if(!Util::fileReadable(gold.c_str())) {
      msg += "Can't open file: " + ToStr(gold.c_str()) + " to read.";
      return false;
    }
    if(!(m_ColSkip >= 0 && m_RowSkip >= 0 && m_Epsilon >= 0)) {
      msg += "invalid ColSkip, RowSkip, and/or Epsilon";
      return false;
    }
    /* Count up differences. */
    // push handler to throw exception
    Err::setThrowStatus(true); 
    int diffCount;
    try {
        diffCount = Util::matrixDifferences(generated.c_str(), gold.c_str() , 
                                            m_ColSkip, m_RowSkip, m_Epsilon, false, m_MatchNames);
    } 
    catch(Except &e) {
        msg += "Caught exception: " + ToStr(e.what());
        return false;
    }
    Err::popHandler();

    if(diffCount > (int)m_AllowedMisMatch) {
      success = false;
      msg += "File: " + generated + " vs " + gold + ": ";
      msg += ToStr("Expecting no more than ") + ToStr(m_AllowedMisMatch) + ToStr(" found: ") + ToStr(diffCount);
    }
    return success;
  }


  std::string m_Generated;   ///< File created by application being tested.
  std::string m_Gold;        ///< File believed to be "truth"
  double m_Epsilon;      ///< Maximum abosolute difference from truth acceptable.
  int m_RowSkip;        ///< Number of rows to skip before comparing.
  int m_ColSkip;        ///< Number of columns to skip before comparing.
  bool m_MatchNames;    ///< Should we try to match the names using
                        ///  first column? Useful when the files aren't
                        ///  necessarily in same order.
  unsigned int m_AllowedMisMatch;  ///< How many can we get wrong before failing?
};

#endif /* MATRIXCHECK_H */
