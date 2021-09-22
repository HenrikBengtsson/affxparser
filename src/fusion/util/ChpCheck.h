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
 * @file   ChpCheck.h
 * @author Chuck Sugnet
 * @date   Tue Apr 25 18:03:02 2006
 * 
 * @brief  Class for doing a check of two CHP files after regression run.
 */

#ifndef CHPCHECK_H
#define CHPCHECK_H

//
#include "util/Fs.h"
#include "util/RegressionCheck.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include "file/CHPFileData.h"
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

/**
 * Class for testing that CHP files are the same +/- some epsilon. Also checks
 * to make sure that at least some of the headers are the same
 * (times/dates/versions change so not checked).
 */
class ChpCheck : public RegressionCheck {

public:

  /// @brief     Constructor
  /// @param     generated 
  /// @param     gold      the reference data to compare with
  /// @param     diffAllowed the number of differences allowed (default 0)
  /// @param     prefix    the prefix for the header string alg part
  /// @param     eps       epsilon [confidence, pvalue]
  ///                      i.e. if |gen-gold| >= frac*max(|gen|,|gold|) then there is a difference.
  /// @param     bCheckHeaders Check headers? (boolean, default: true).
  /// @param     frac      maximum accepted fractional difference in numeric values (not used by default).
  ///                      i.e. if |gen-gold| >= frac*max(|gen|,|gold|) then there is a difference.
  ChpCheck(std::vector<std::string> &generated, std::vector<std::string> &gold, 
           int diffAllowed=0, const std::string &prefix="apt-", double eps=0.0001,
           bool bCheckHeaders = true, double frac=0.0) {
    m_Name = "XDA-CHP-Check";
    m_Generated = generated;
    m_Gold = gold;
    m_Eps_confidence = eps;
    m_Eps_pvalue = eps;
    m_Eps_signal = eps;
    m_Frac_confidence = frac;
    m_Frac_pvalue = frac;
    m_Frac_signal = frac;
    m_DiffAllowed = diffAllowed;
    m_Prefix = prefix;
    m_CheckHeaders = bCheckHeaders;

    fillInToIgnore(m_IgnoreMap, prefix); // things know to change like user, time, etc.

    setMaxError(30);
  }

  ChpCheck(const std::string &generated, const std::string &gold, 
           int diffAllowed=0, const std::string &prefix="apt-", double eps=0.0001,
           bool bCheckHeaders = true, double frac=0.0) {
    m_Generated.push_back(generated);
    m_Gold.push_back(gold);
    m_Eps_confidence = eps;
    m_Eps_pvalue = eps;
    m_Eps_signal = eps;
    m_Frac_confidence = frac;
    m_Frac_pvalue = frac;
    m_Frac_signal = frac;
    m_DiffAllowed = diffAllowed;
    m_Prefix = prefix;
    m_CheckHeaders = bCheckHeaders;

    fillInToIgnore(m_IgnoreMap, prefix); // things know to change like user, time, etc.

    setMaxError(30);
  }


  bool checkFilePair(int genIdx, int goldIdx, std::string &msg)
  {
    bool success = true;
     try {
            m_Generated[genIdx] = Fs::convertToUncPath(m_Generated[genIdx]);
            m_Gold[goldIdx] = Fs::convertToUncPath(m_Gold[goldIdx]);
            if (m_CheckHeaders && !headersSame(m_Generated[genIdx], m_Gold[goldIdx], msg))
	      success = false;
            if(!dataSame(m_Generated[genIdx], m_Gold[goldIdx], msg)) {
	      success = false;
            }
	  } // end try
	  catch(Except &e) {
            success &= checkMsg(false, "Error: " + ToStr(e.what()),msg);
	  }
	  catch(const std::exception &e) {
            success &= checkMsg(false, "Error: standard exception: " + ToStr(e.what()),msg);
	  }
	  catch(...) {
            success &= checkMsg(false, "Error: Uncaught Exception.",msg);
	  }
	  return success;
  }
  /** 
   * Check to make sure that two files are the same +/- some epsilon. 
   * @param msg - Fills in an error message if test fails, empty string otherwise.
   * @return - Returns true if files are close enough, false otherwise.
   */
  bool check(std::string &msg) {
    bool success = true;
    if(m_Generated.size() != m_Gold.size()) {
      return checkMsg(false, "CelCheck::check() - generated and gold vectors must be same size.",msg);
    }

    if(m_Generated.size() == 1 && m_Generated.size() == 1)
      {
	success = this->checkFilePair(0,0,msg);
      }
    else
      {
	for(unsigned int i = 0; i < m_Generated.size(); i++) 
	  {
	    success = this->checkFilePair(i,i,msg);
	}
      }
    return success;
  }

  /** 
   * Clear out the default set of header entries to ignore
   */
  void clearHeaderIgnore() {
      m_IgnoreMap.clear();
  }

  /** 
   * Add a header item to ignore
   * @param key - reference to a wide string
   */
  void addHeaderIgnore(std::string &key) {
      m_IgnoreMap.insert(key);
  }

private:
  
  // Header entries to ignore
  static void fillInToIgnore(std::set<std::string> &ignoreMap, const std::string &prefix) {
    ignoreMap.clear();
    ignoreMap.insert("program-version");
    ignoreMap.insert(prefix + "exec-guid");
    ignoreMap.insert(prefix + "analysis-guid");
    ignoreMap.insert(prefix + "time-str"); 
    ignoreMap.insert(prefix + "free-mem");    
    ignoreMap.insert(prefix + "cvs-id");
    ignoreMap.insert(prefix + "version");
    ignoreMap.insert(prefix + "opt-out-dir");
  }

  /** 
   * Check the gold tag value pairs to make sure they are getting the
   * same values in the test. We only check items that are in gold,
   * which means that if a new field has been added it will not be
   * checked.
   * 
   * @param gold - List of key, value pairs in the gold set.
   * @param test - List of key, value pairs in the set to be checked.
   * @param msg - Place for adding error messages as they are encountered.
   * 
   * @return - true if same, false otherwise.
   */
  bool tagValuePairMostlySame(TagValuePairTypeList &gold, TagValuePairTypeList &test, std::string &msg) {
    bool same = true;
    std::map<std::string,std::string> testMap;
    std::map<std::string,std::string>::iterator testMapIter;
    TagValuePairTypeList::iterator testIter;
    // Load up test as a map which will be queried by items in gold.
    for(testIter = test.begin(); testIter != test.end(); ++testIter) {
      testMap[testIter->Tag] = testIter->Value;
    }
    TagValuePairTypeList::iterator goldIter;
    for(goldIter = gold.begin(); goldIter != gold.end(); ++goldIter) {
      // ignore items that are in the ignoreMap
      if(m_IgnoreMap.find(goldIter->Tag) == m_IgnoreMap.end()) {
        testMapIter = testMap.find(goldIter->Tag);
        if(testMapIter == testMap.end()) {
          msg += " Error: Test missing field: '" + ToStr(goldIter->Tag) + "'";
          same = false;
        }
        else {
          bool isTestNum = false;
          bool isGoldNum = false;
          float testNum, goldNum;
          testNum = Convert::toFloatCheck(testMapIter->second,&isTestNum);
          goldNum = Convert::toFloatCheck(goldIter->Value,&isGoldNum);
          if(isTestNum && isGoldNum) {
            bool success = true;
            double diff = 0.0;
            if(!checkFloat(goldNum, testNum, m_Eps_confidence, success, diff, false, 0.0)) {
              msg += " Error: for field '" + goldIter->Tag + "' expecting: '" + goldIter->Value + 
                "' got: '" + testMapIter->second + "'";
              same = false;
            }
          } else {
            if(testMapIter->second != goldIter->Value &&
               Fs::basename(testMapIter->second) != Fs::basename(goldIter->Value)) {
              msg += " Error: for field '" + goldIter->Tag + "' expecting: '" + goldIter->Value + 
                "' got: '" + testMapIter->second + "'";
              same = false;
            }
          }
        }
      }
    }
    return same;
  }
  
  bool headersSame(const std::string &generated, const std::string &gold, std::string &msgs) {
    bool success = true;
    affxchp::CCHPFileData generatedChp, goldChp;
    affxchp::CCHPFileHeader genHdr, goldHdr;
    generatedChp.SetFileName(generated.c_str());
    goldChp.SetFileName(gold.c_str());
    if(!generatedChp.ReadHeader()) {
      success = false;
	  success &= checkMsg(false, "Error: Can't read CHP Header in '" + ToStr(generated) + "' error is: " + generatedChp.GetError(), msgs);
	  return success;
    }
    if(!goldChp.ReadHeader()) {
      success = false;
	  success &= checkMsg(false, "Error: Can't read CHP Header in '" + ToStr(gold) + "' error is: " + goldChp.GetError(), msgs);
	  return success;
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
      success &= checkMsg(genHdr.GetAssayType() == goldHdr.GetAssayType(), "Error: Assay Type not the same. ", msgs);

      TagValuePairTypeList &goldList = goldHdr.AlgorithmParameters();
      TagValuePairTypeList &genList = genHdr.AlgorithmParameters();
      bool paramSame = tagValuePairMostlySame(goldList, genList, msgs);
      success &= paramSame;

      goldList = goldHdr.SummaryParameters();
      genList = genHdr.SummaryParameters();
      paramSame = tagValuePairMostlySame(goldList, genList, msgs);
      success &= paramSame;
    }
    return success;
  }

  bool dataSame(const std::string &generated, const std::string &gold, std::string &msgs) {
    bool success = true;
    double maxDiffConf = -1;
    double maxDiffPval = -1;
    double maxDiffSignal = -1;
    int numDiff = 0;

    affxchp::CCHPFileData generatedChp, goldChp;
    affxchp::CCHPFileHeader genHdr, goldHdr;
    generatedChp.SetFileName(generated.c_str());
    goldChp.SetFileName(gold.c_str());
    if(!generatedChp.Read()) {
      success = false;
	  success &= checkMsg(false, "Error: Can't read CHP '" + ToStr(generated) + "' error is: " + generatedChp.GetError(), msgs);
	  return success;
    }
    if(!goldChp.Read()) {
      success = false;
	  success &= checkMsg(false, "Error: Can't read CHP '" + ToStr(gold) + "' error is: " + goldChp.GetError(), msgs);	  
	  return success;
    }    

    genHdr = generatedChp.GetHeader();
    goldHdr = goldChp.GetHeader();  
	if (genHdr.GetAssayType() != goldHdr.GetAssayType()) {success &= checkMsg(false, "Error: Assay Types are not the same. Gold: " + gold + " " + ToStr(goldHdr.GetAssayType()) + " Generated: " + generated + " " + ToStr(genHdr.GetAssayType()), msgs); return success;}
	if (genHdr.GetNumProbeSets() != goldHdr.GetNumProbeSets()) {success &= checkMsg(false, "Error: Number of Probe Sets are not the same. Gold: " + gold + " " + ToStr(goldHdr.GetNumProbeSets()) + " Generated: " + generated + " " + ToStr(genHdr.GetNumProbeSets()), msgs); return success;}
	if(genHdr.GetAssayType() == affxchp::CCHPFileHeader::Genotyping) {
        for(int i = 0; i < genHdr.GetNumProbeSets(); i++) {
            bool localSuccess = true;
            affxchp::CGenotypeProbeSetResults *genResults = generatedChp.GetGenotypingResults(i);
            if(genResults == NULL)
                Err::errAbort("Failed to get entry for test (" + ToStr(i) + "). File: " + generated + " Error: " + generatedChp.GetError());
            affxchp::CGenotypeProbeSetResults *goldResults = goldChp.GetGenotypingResults(i);
            if(goldResults == NULL)
                Err::errAbort("Failed to get entry for gold (" + ToStr(i) + "). File: " + gold + " Error: " + generatedChp.GetError());
            if(goldResults->AlleleCall != genResults->AlleleCall) 
                localSuccess = false;
            checkFloat(goldResults->Confidence, genResults->Confidence, m_Eps_confidence, localSuccess, maxDiffConf, false, m_Frac_confidence);
            checkFloat(goldResults->pvalue_AA, genResults->pvalue_AA, m_Eps_pvalue, localSuccess, maxDiffPval, false, m_Frac_pvalue);
            checkFloat(goldResults->pvalue_AB, genResults->pvalue_AB, m_Eps_pvalue, localSuccess, maxDiffPval, false, m_Frac_pvalue);
            checkFloat(goldResults->pvalue_BB, genResults->pvalue_BB, m_Eps_pvalue, localSuccess, maxDiffPval, false, m_Frac_pvalue);
            checkFloat(goldResults->pvalue_NoCall, genResults->pvalue_NoCall, m_Eps_pvalue, localSuccess, maxDiffPval, false, m_Frac_pvalue);
            ///@todo check RAS fields
            //checkFloat(goldResults->RAS1, genResults->RAS1, m_Eps_ras, localSuccess, maxDiffRas, false, m_Frac_ras);
            //checkFloat(goldResults->RAS2, genResults->RAS2, m_Eps_ras, localSuccess, maxDiffRas, false, m_Frac_ras);
            if(!localSuccess)
                numDiff++;
            success &= localSuccess;
        }
        if(maxDiffConf > m_Eps_confidence)
            Verbose::out(1, "Max diff: " + ToStr(maxDiffConf) + " is greater than expected (" + ToStr(m_Eps_confidence) + ") [confidence]");
        if(maxDiffPval > m_Eps_pvalue)
            Verbose::out(1, "Max diff: " + ToStr(maxDiffPval) + " is greater than expected (" + ToStr(m_Eps_pvalue) + ") [pvalue]");
        if(numDiff > 0)
            Verbose::out(1, ToStr(numDiff) + " of " + ToStr(genHdr.GetNumProbeSets()) + " (" + 
                            ToStr(100.0 * numDiff/genHdr.GetNumProbeSets()) + "%) were different.");

        std::string res = "different";
        if(success) 
            res = "same";
        Verbose::out(1, generated + ": checked " + ToStr(genHdr.GetNumProbeSets()) + " genotype entries.");
        Verbose::out(1, generated + ": max confidence diff is: " + ToStr(maxDiffConf));
        Verbose::out(1, generated + ": max pvalue diff is: " + ToStr(maxDiffPval));
        Verbose::out(1, generated + ": chip is " + res + ".");
    } else if(genHdr.GetAssayType() == affxchp::CCHPFileHeader::Expression) {
        for(int i = 0; i < genHdr.GetNumProbeSets(); i++) {
            bool localSuccess = true;
            affxchp::CExpressionProbeSetResults *genResults = generatedChp.GetExpressionResults(i);
            affxchp::CExpressionProbeSetResults *goldResults = goldChp.GetExpressionResults(i);
            checkFloat(goldResults->Signal, genResults->Signal, m_Eps_signal, localSuccess, maxDiffSignal, false, m_Frac_signal);
            ///@todo check other fields
            if(!localSuccess)
                numDiff++;
            success &= localSuccess;
        }
        if(maxDiffSignal > m_Eps_signal)
            Verbose::out(1, "Max diff: " + ToStr(maxDiffSignal) + " is greater than expected (" + ToStr(m_Eps_signal) + ") [signal]");
        if(numDiff > 0)
            Verbose::out(1, ToStr(numDiff) + " of " + ToStr(genHdr.GetNumProbeSets()) + " (" + 
                            ToStr(100.0 * numDiff/genHdr.GetNumProbeSets()) + "%) were different.");

        std::string res = "different";
        if(success) 
            res = "same";
        Verbose::out(1, generated + ": checked " + ToStr(genHdr.GetNumProbeSets()) + " expression entries.");
        Verbose::out(1, generated + ": max signal diff is: " + ToStr(maxDiffSignal));
        Verbose::out(1, generated + ": chip is " + res + ".");
    } else {
        Err::errAbort("ChpCheck::ChpCheck() - unknown CHP type.");
    }

    if(!success && m_DiffAllowed >= numDiff) 
        success = true;
    if(!success)
      msgs += "Error: " + generated + " is different from " + gold + ". ";
    return success;
  }
      
  /// Filenames for the gold standard or correct data chp files.
  std::vector<std::string> m_Gold;
  /// Matching filenames for the chp files to be tested.
  std::vector<std::string> m_Generated;
  /// Epsilon, small value that two floats can differ by but still be considered equivalent.
  double m_Eps_confidence;
  double m_Eps_pvalue;
  double m_Eps_signal;
  double m_Frac_confidence;
  double m_Frac_pvalue;
  double m_Frac_signal;
  bool m_CheckHeaders;
  /// How many differences will we tolerate?
  int m_DiffAllowed;
  /// What is the expected prefix for parameter names?
  std::string m_Prefix;
  /// Header entries to ignore
  std::set<std::string> m_IgnoreMap;
  
};

#endif /* CHPCHECK_H */
