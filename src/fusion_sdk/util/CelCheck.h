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
 * @file   CelCheck.h
 * @author Alan Williams
 * @date   Tue July 11 18:03:02 2007
 * 
 * @brief  Class for doing a check of two CEL files after regression run.
 */

#ifndef CELCHECK_H
#define CELCHECK_H

#include "calvin_files/fusion/src/FusionCELData.h"
#include "calvin_files/utils/src/StringUtils.h"
#include "util/Fs.h"
#include "util/RegressionCheck.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <sys/stat.h>
#include <vector>
//
using namespace affymetrix_fusion_io;

/**
 * Class for testing that CEL files are the same +/- some epsilon. Also checks
 * to make sure that at least some of the headers are the same
 * (times/dates/versions change so not checked).
 */
class CelCheck : public RegressionCheck {

public:

  /**
   * @brief     Constructor
   * @param     generated 
   * @param     gold      the reference data to compare with
   * @param     eps       epsilon
   * @param     prefix
   * @param     diffAllowed
   * @return    
   */
  CelCheck(
           std::vector<std::string> &generated,
           std::vector<std::string> &gold, 
           double eps, 
           const std::string &prefix="affymetrix-", 
           int diffAllowed=0) {
    m_Name = "CEL-Check";
    m_Generated = generated;
    m_Gold = gold;
    m_Eps = eps;
    m_Prefix = prefix;
    m_DiffAllowed = diffAllowed;
  }
  CelCheck(std::string &generated,
	   std::string &gold,
	   double eps,
	   const std::string &prefix ="affymetrix-",
	   int diffAllowed = 0)
    {
      m_Generated.push_back(generated);
      m_Gold.push_back(gold);
      m_Eps = eps;
      m_Prefix = prefix;
      m_DiffAllowed = diffAllowed;
    }

  /** 
   * Check to make sure that two files are the same +/- some epsilon. 
   * @param msg - Fills in an error message if test fails, empty string otherwise.
   * @return - Returns true if files are close enough, false otherwise.
   */

  bool checkFilePair(int genIdx, int goldIdx, std::string &msg)
  {
    bool success = true;
    try {
           // currently the cel file readers do not appear to like the "\\?\" , use normalizePath and not convertToUncPath
            m_Generated[genIdx] = Fs::normalizePath(m_Generated[genIdx]);
            m_Gold[goldIdx] = Fs::normalizePath(m_Gold[goldIdx]);
            if(!headersSame(m_Generated[genIdx], m_Gold[goldIdx], msg))
                success = false;
            if(!dataSame(m_Generated[genIdx], m_Gold[goldIdx], msg)) {
                success = false;
            }
        } // end try
        catch(Except &e) {
            success &= checkMsg(false, "Error: " + ToStr(e.what()),msg);
        }
        catch(affymetrix_calvin_exceptions::CalvinException &ce) {
            success &= checkMsg(false, "Error: AGCC library exception: " + 
                            StringUtils::ConvertWCSToMBS(ce.Description()),msg);
        }
        catch(const std::exception &e) {
            success &= checkMsg(false, "Error: standard exception: " + ToStr(e.what()),msg);
        }
        catch(...) {
            success &= checkMsg(false, "Error: Uncaught Exception.",msg);
        }

	return success;
  }

  bool check(std::string &msg) {
    bool success = true;
    if(m_Generated.size() != m_Gold.size()) {
      return checkMsg(false, "CelCheck::check() - generated and gold vectors must be same size.",msg);
    }

    if(m_Generated.size() == 1 && m_Gold.size() == 1)
      {
	success = checkFilePair(0, 0, msg);
      }
    else
      {
	for(unsigned int i = 0; i < m_Generated.size(); i++) {
	  success = checkFilePair(i, i, msg);
	}
      }
    return success;
  }

private:
  
  static void fillInToIgnore(std::set<std::string> &ignoreMap, const std::string &prefix) {
    ignoreMap.clear();
    ignoreMap.insert("analysis-guid");
  }

  bool headersSame(const std::string &generated, const std::string &gold, std::string &msgs) {
    ///@todo implement cel header check once we have new regression data
    return true;
  }

  bool dataSame(const std::string &generated, const std::string &gold, std::string &msgs) {
    bool success = true;
    double maxDiff = -1;
    int numDiff = 0;
    FusionCELData generatedCel, goldCel;

    goldCel.SetFileName (gold.c_str());
    if (! goldCel.Read()) {
      msgs += "Can't read cel file: " + ToStr(gold);
      return false;
    }
    generatedCel.SetFileName (generated.c_str());
    if (! generatedCel.Read()) {
      msgs += "Can't read cel file: " + ToStr(generated);
      return false;
    }
    const int numCells = goldCel.GetNumCells();
    for (int celIx = 0; celIx < numCells; celIx++) {
      bool localSuccess = true;
      checkFloat(goldCel.GetIntensity(celIx), generatedCel.GetIntensity(celIx), m_Eps, localSuccess, maxDiff);
      if(!localSuccess) {
        numDiff++;
      }
      success &= localSuccess;
    }

    if(maxDiff > m_Eps) {
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_Eps) + ")");
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(numCells) + " (" + 
                   ToStr(100.0 * numDiff/numCells) + "%) were different.");
    }
    if(!success && m_DiffAllowed >= numDiff) 
        success = true;
    if(!success) {
      msgs += "Error: " + generated + " is different from " + gold + ". ";
      
    }
    std::string res = "different";
    if(success) 
        res = "same";
    Verbose::out(1, generated + ToStr(" chip is ") + res + " max diff is: " + ToStr(maxDiff));
    return success;
  }
      
  std::vector<std::string> m_Generated;
  std::vector<std::string> m_Gold;
  double m_Eps;
  std::string m_Prefix;
  int m_DiffAllowed;
};

#endif /* CELCHECK_H */
