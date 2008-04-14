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
#include "Verbose.h"
#include "util/Util.h"
#include "portability/affy-base-types.h"

/**
 * Abstract base class (i.e. interface) for checks to be run during a regression
 * test.
 */
class RegressionCheck {

public:
 
  /** Constructor */
  RegressionCheck() {
    m_MaxErrorsReport = 30;
    m_CurrentErrorsReported = 0;
  }

  /** Virtual destructor for virtual class. */
  virtual ~RegressionCheck() {}

  /** 
   * Check the condition expected.
   * @param msg - Message to be optionally set while checking.
   * @return - true if passed, false otherwise.
   */
  virtual bool check(std::string &msg) = 0;

protected:
  /** 
   * Utility function to see if a condition is true and if not to add
   * the message to our summary of messages.
   * @param condition - boolean to be tested.
   * @param msg - informative message for this condition (if fails)
   * @param summary - ongoing collection of messages.
   * @return - condition test results.
   */
  bool checkMsg(bool condition, const std::string &msg, std::string &summary) {
    if(!condition) {
      summary += msg;
      reportError(msg);
    }
    return condition;
  }

  /** 
   * Utility function to accumulate error messages
   * @param err - error message
   * @return - void
   */
  void reportError(const std::string &err) {
    if(m_CurrentErrorsReported++ < m_MaxErrorsReport) {
      Verbose::out(1, "Error encountered: " + err);
      if(m_CurrentErrorsReported >= m_MaxErrorsReport) {
        Verbose::out(1, "Maximum number of errors reported.");
      }
    }
  }

  /** 
   * Utility function to set the max number of allowed errors
   * @param max - maximum number of errors to report
   * @return - void
   */
  void setMaxError(int max) {
    m_MaxErrorsReport = max;
  }

  /** 
   * Are two floats within epsilon (small value) of eachother?
   * 
   * @param gold - correct value.
   * @param gen - value to be tested.
   * @param eps - epsilon, small number they can be different but still equivalent.
   * @param success - boolean set to false if floats not equivalent.
   * @param maxDiff - Maximum difference seen so far.
   * @return - true if floats equivalent, false otherwise.
   */
  void checkFloat(float gold, float gen, double eps, bool &success, double &maxDiff, bool log = false) {

      if(!Util::isFinite(gold) || !Util::isFinite(gen)) {
        // Allow for case where both inf to not be an error
        if(gold == gen)
            return;
        success = false;
        if(log)
            reportError("Non-finite floats. gold: '" + ToStr(gold) + "' test: '" + ToStr(gen) + "'");
        return;
      }
      
      double diff = fabs(gold - gen);
      maxDiff = Max(diff, maxDiff);
      if(diff > eps) {
        success = false;
        if(log)
            reportError("Different floats. gold: '" + ToStr(gold) + "' test: '" + ToStr(gen) + "'");
        return;
      }
  }
  
private:

  int m_MaxErrorsReport;        /// The maximum number of errors to report
  int m_CurrentErrorsReported;  /// The current number of reported errors

};


#endif /* REGRESSIONCHECK_H */
