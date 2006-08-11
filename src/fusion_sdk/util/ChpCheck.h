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
 * @file   ChpCheck.h
 * @author Chuck Sugnet
 * @date   Tue Apr 25 18:03:02 2006
 * 
 * @brief  Class for doing a check of two CHP files after regression run.
 */

#ifndef CHPCHECK_H
#define CHPCHECK_H

#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include "Util.h"
#include "Verbose.h"
#include "RegressionCheck.h"
#include "file/CHPFileData.h"

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

/**
 * Class for testing that CHP files are the same +/- some epsilon. Also checks
 * to make sure that at least some of the headers are the same
 * (times/dates/versions change so not checked).
 */
class ChpCheck : public RegressionCheck {

public:

  ChpCheck(std::vector<std::string> &generated, std::vector<std::string> &gold,
           double eps) {
    if(generated.size() != gold.size()) {
      Err::errAbort("ChpCheck::ChpCheck() - generated and gold vectors must be same size.");
    }
    m_Generated = generated;
    m_Gold = gold;
    m_Eps = eps;

  }

  /** 
   * Check to make sure that two files are the same +/- some epsilon. 
   * @param msg - Fills in an error message if test fails, empty string otherwise.
   * @return - Returns true if files are close enough, false otherwise.
   */
  bool check(std::string &msg) {
    bool success = true;
    for(unsigned int i = 0; i < m_Generated.size(); i++) {
      m_Generated[i] = Util::getPathName(m_Generated[i].c_str());
      m_Gold[i] = Util::getPathName(m_Gold[i].c_str());
      if(headersSame(m_Generated[i], m_Gold[i], msg) &&
         dataSame(m_Generated[i], m_Gold[i], msg)) {
      }
      else {
        return false;
      }
    }
    return success;
  }

private:
  
  bool checkMsg(bool condition, const std::string &msg, std::string &summary) {
    if(!condition) {
      summary += msg;
    }
    return condition;
  }
  
  bool headersSame(const std::string &generated, const std::string &gold, std::string &msgs) {
    bool success = true;
    affxchp::CCHPFileData generatedChp, goldChp;
    affxchp::CCHPFileHeader genHdr, goldHdr;
    generatedChp.SetFileName(generated.c_str());
    goldChp.SetFileName(gold.c_str());
    if(!generatedChp.ReadHeader()) {
      success = false;
      msgs += "Can't read CHP" + ToStr(generated) + " error is: " + generatedChp.GetError();
    }
    if(!goldChp.ReadHeader()) {
      success = false;
      msgs += "Can't read CHP" + ToStr(gold) + " error is: " + goldChp.GetError();
    }
    if(success) {
      genHdr = generatedChp.GetHeader();
      goldHdr = goldChp.GetHeader();
      success &= checkMsg(genHdr.GetCols() == goldHdr.GetCols(), "Error: cols not the same. ", msgs);
      success &= checkMsg(genHdr.GetRows() == goldHdr.GetRows(), "Error: rows not the same. ", msgs);
      success &= checkMsg(genHdr.GetNumProbeSets() == goldHdr.GetNumProbeSets(), "Error: NumProbeSets not the same. ", msgs);
      success &= checkMsg(genHdr.GetChipType() == goldHdr.GetChipType(), "Error: ChipType not the same. ", msgs);
      success &= checkMsg(genHdr.GetAlgVersion() == goldHdr.GetAlgVersion(), "Error: AlgVersion not the same. ", msgs);
      success &= checkMsg(genHdr.GetProgID() == goldHdr.GetProgID(), "Error: ProgID not the same. ", msgs);
      TagValuePairTypeList &goldList = goldHdr.AlgorithmParameters();
      TagValuePairTypeList &genList = genHdr.AlgorithmParameters();
      TagValuePairTypeList::iterator genIter, goldIter;
      for(genIter = genList.begin(), goldIter = goldList.begin();
          genIter != genList.end() && goldIter != goldList.end();
          ++genIter, ++goldIter) {
        // skip the parameters that should be changing
        if(goldIter->Tag == "analysis-guid" ||
           goldIter->Tag == "exec-guid" ||
           goldIter->Tag == "cvs-id" || goldIter->Tag == "program-id" || goldIter->Tag == "chrX-file" ||
           goldIter->Tag == "time-str" || goldIter->Tag == "cel-files") {
          continue;
        }
        if(!checkMsg(goldIter->Value == genIter->Value, 
                     "Error: values not same for: " + goldIter->Tag + " " + 
                     goldIter->Value + " vs. " + genIter->Value, msgs)) {
          success = false;
        }
      }
    }
    return success;
  }

  bool checkFloat(float gold, float gen, double eps, bool &success, double &maxDiff) {
    float diff = fabs(gold - gen);
    maxDiff = MAX(diff, maxDiff);
    if(diff > eps) 
      success = false;
    return success;
  }

  bool dataSame(const std::string &generated, const std::string &gold, std::string &msgs) {
    bool success = true;
    double maxDiff = -1;
    int numDiff = 0;
    affxchp::CCHPFileData generatedChp, goldChp;
    affxchp::CCHPFileHeader genHdr, goldHdr;
    generatedChp.SetFileName(generated.c_str());
    goldChp.SetFileName(gold.c_str());
    if(!generatedChp.Read()) {
      success = false;
      msgs += "Can't read CHP" + ToStr(generated) + " error is: " + generatedChp.GetError();
    }
    if(!goldChp.Read()) {
      success = false;
      msgs += "Can't read CHP" + ToStr(gold) + " error is: " + goldChp.GetError();
    }    
    genHdr = generatedChp.GetHeader();    
    for(int i = 0; i < genHdr.GetNumProbeSets(); i++) {
      bool localSuccess = true;
      affxchp::CGenotypeProbeSetResults *genResults = generatedChp.GetGenotypingResults(i);
      affxchp::CGenotypeProbeSetResults *goldResults = goldChp.GetGenotypingResults(i);
      if(goldResults->AlleleCall != genResults->AlleleCall) 
        localSuccess = false;
      checkFloat(goldResults->Confidence, genResults->Confidence, m_Eps, localSuccess, maxDiff);
      checkFloat(goldResults->pvalue_AA, genResults->pvalue_AA, m_Eps, localSuccess, maxDiff);
      checkFloat(goldResults->pvalue_AB, genResults->pvalue_AB, m_Eps, localSuccess, maxDiff);
      checkFloat(goldResults->pvalue_BB, genResults->pvalue_BB, m_Eps, localSuccess, maxDiff);
      checkFloat(goldResults->pvalue_NoCall, genResults->pvalue_NoCall, m_Eps, localSuccess, maxDiff);
      if(!localSuccess) {
        numDiff++;
      }
      success &= localSuccess;
    }
    if(maxDiff > m_Eps) {
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_Eps) + ")");
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(genHdr.GetNumProbeSets()) + " (" + 
                   ToStr(100.0 * numDiff/genHdr.GetNumProbeSets()) + "%) were different.");
    }
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

};

#endif /* CHPCHECK_H */
