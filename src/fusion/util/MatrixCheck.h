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
 * @file   MatrixCheck.h
 * @author Chuck Sugnet
 * @date   Tue Apr 25 16:12:22 2006
 * 
 * @brief Class for doing a check of two matrix files after regression run.
 */
#ifndef MATRIXCHECK_H
#define MATRIXCHECK_H

//
#include "util/Fs.h"
#include "util/RegressionCheck.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
//

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
   * @param eps - Maximum absolute difference from truth acceptable.
   * @param rowSkip - Number of rows to skip before comparing.
   * @param colSkip - Number of columns to skip before comparing.
   * @param matchNames - Should we try to match the names using
   *                     first column? Useful when the files aren't
   *                     necessarily in same order.
   * @param allowedMisMatch - How many can we get wrong before failing?
   * @param frac - Maximum fractional difference from truth acceptable
   */
  MatrixCheck(
                const std::string &generated, 
                const std::string &gold, 
                double eps, 
                int rowSkip, 
                int colSkip, 
                bool matchNames, 
                unsigned int allowedMisMatch, 
				double frac = 0.0 ) :
    m_Generated(generated), m_Gold(gold), m_Epsilon(eps), m_Fraction(frac), 
    m_RowSkip(rowSkip), m_ColSkip(colSkip), m_MatchNames(matchNames),
    m_AllowedMisMatch(allowedMisMatch), m_PrintMismatch(false), m_PrintMismatchMax(-1) { 
    m_Name = Fs::basename(generated);
    }

  /** 
   * Utility function to enable/disable reporting of individual differences
   * @param print - mismatches printed if true, not printed if false.
   * @return - void
   */
  void setPrintMismatch(bool print) {
    m_PrintMismatch = print;
  }

  /** 
   * Utility function to set the max number of mismatches reported
   * @param max - maximum number of mismatches to report (set to -1 for no limit default behavior)
   * @return - void
   */
  void setPrintMismatchMax(int max) {
    m_PrintMismatchMax = max;
  }

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
    generated = Fs::convertToUncPath(generated);
    gold = Fs::convertToUncPath(gold);
    /* Santiy checks. */
    if (!Fs::isReadable(generated)) {
      msg += "Can't open file: "+FS_QUOTE_PATH(generated)+" to read.";
      return false;
    }
    if (!Fs::isReadable(gold.c_str())) {
      msg += "Can't open file: "+FS_QUOTE_PATH(gold)+" to read.";
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
        diffCount = Util::matrixDifferences(generated.c_str(), gold.c_str(), 
                                            m_ColSkip, m_RowSkip, m_Epsilon, m_PrintMismatch, m_MatchNames, 
											m_Fraction, m_PrintMismatchMax );
    } 
    catch(Except &e) {
        msg += "Caught exception: " + ToStr(e.what());
        return false;
    }
    Err::setThrowStatus(true); 

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
  double m_Fraction;     ///< Maximum fractional difference from truth acceptable
  int m_RowSkip;        ///< Number of rows to skip before comparing.
  int m_ColSkip;        ///< Number of columns to skip before comparing.
  bool m_MatchNames;    ///< Should we try to match the names using
                        ///  first column? Useful when the files aren't
                        ///  necessarily in same order.
  unsigned int m_AllowedMisMatch;  ///< How many can we get wrong before failing?
  bool m_PrintMismatch;   ///< Mismatches printed if true, not printed if false.
  int m_PrintMismatchMax; ///< Maximum number of mismatches to print (default -1 for no limit)
};

#endif /* MATRIXCHECK_H */
