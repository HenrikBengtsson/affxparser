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
 * @file   CalvinChpCheck.h
 * @author Chuck Sugnet
 * @date   Tue Apr 25 18:03:02 2006
 * 
 * @brief  Class for doing a check of two CHP files after regression run.
 */

#ifndef CALVINCHPCHECK_H
#define CALVINCHPCHECK_H

#include <string>
#include <set>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include "Util.h"
#include "Verbose.h"
#include "RegressionCheck.h"
#include "CHPData.h"
#include "CHPFileReader.h"
#include "CHPMultiDataData.h"
#include "CHPMultiDataFileReader.h"
#include "CHPQuantificationFileReader.h"
#include "StringUtils.h"
#include "portability/affy-base-types.h"

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_data;
using namespace std;

/**
 * Class for testing that CHP files are the same +/- some epsilon. Also checks
 * to make sure that at least some of the headers are the same
 * (times/dates/versions change so not checked). 
 */
class CalvinChpCheck : public RegressionCheck {

public:

  /** 
   * Constructor.
   * @param generated - vector of filenames to be tested.
   * @param gold - matching vector of filenames for gold standard data to test against.
   * @param diffAllowed - 
   * @param prefix -
   * @param eps - epsilon: small amount that two floats can differ by,
   * but still be considered equivalent.
   */
  CalvinChpCheck(std::vector<std::string> &generated, 
                 std::vector<std::string> &gold, 
                 int diffAllowed=0,
                 const std::wstring &prefix=L"apt-", 
                 double eps=0.0001) {
    m_Generated = generated;
    m_Gold = gold;
    m_EpsConfidence = eps;
    m_EpsQuantification = eps;
    /// @todo allow for a vector of eps for checking extra metrics
    m_EpsParam = eps;
    m_DiffAllowed = diffAllowed;
    m_Prefix = prefix;

    fillInToIgnore(m_IgnoreMap, prefix); // things know to change like user, time, etc.

    setMaxError(30);
  }

  /** 
   * Check to make sure that two files are the same +/- some epsilon. 
   * @param msg - Fills in an error message if test fails, empty string otherwise.
   * @return - Returns true if files are close enough, false otherwise.
   */
  bool check(std::string &msg) {
    bool success = true;
    if(m_Generated.size() != m_Gold.size()) {
      return checkMsg(false, "CalvinChpCheck::check() - generated and gold vectors must be same size.",msg);
    }
    for(unsigned int i = 0; i < m_Generated.size(); i++) {
        try {
            m_Generated[i] = Util::getPathName(m_Generated[i].c_str());
            m_Gold[i] = Util::getPathName(m_Gold[i].c_str());
            CHPData chp1, chp2;
            CHPFileReader reader;
            reader.SetFilename(m_Generated[i]);
            reader.Read(chp1);
            reader.SetFilename(m_Gold[i]);
            reader.Read(chp2);


            string chp1Type = chp1.GetFileHeader()->GetGenericDataHdr()->GetFileTypeId();
            string chp2Type = chp2.GetFileHeader()->GetGenericDataHdr()->GetFileTypeId();

            success &= checkMsg(chp2Type == chp1Type, 
                                "Error: Different CHP Types: " + chp2Type + ", " + chp1Type, msg);

            if(!headersSame(chp2, chp1, msg)) {
                success = false;
            }

            if(chp1Type == CHP_MULTI_DATA_TYPE) {
                CHPMultiDataData generatedChp, goldChp;
                CHPMultiDataFileReader chpReader;
                chpReader.SetFilename(m_Generated[i]);
                chpReader.Read(generatedChp);
                chpReader.SetFilename(m_Gold[i]);
                chpReader.Read(goldChp);
                if(!multiDataGenotypeSame(goldChp, generatedChp, msg, GenotypeMultiDataType)) {
                    success = false;
                }
                if(!multiDataExpressionSame(goldChp, generatedChp, msg, ExpressionMultiDataType)) {
                    success = false;
                }
            } else if(chp1Type == "affymetrix-quantification-analysis") {
                CHPQuantificationData generatedChp, goldChp;
                CHPQuantificationFileReader chpReader;
                chpReader.SetFilename(m_Generated[i]);
                chpReader.Read(generatedChp);
                chpReader.SetFilename(m_Gold[i]);
                chpReader.Read(goldChp);
                if(!quantificationDataSame(goldChp, generatedChp, msg)) {
                    success = false;
                }
            } else if(chp1Type == CHP_GENOTYPING_ASSAY_TYPE) {
                success &= checkMsg(false, "Error: Unable to compare CHPs of type " + chp1Type, msg);
            } else if(chp1Type == CHP_UNIVERSAL_ASSAY_TYPE) {
                success &= checkMsg(false, "Error: Unable to compare CHPs of type " + chp1Type, msg);
            } else if(chp1Type == CHP_RESEQUENCING_ASSAY_TYPE) {
                success &= checkMsg(false, "Error: Unable to compare CHPs of type " + chp1Type, msg);
            } else {
                success &= checkMsg(false, "Error: Unable to compare CHPs of type " + chp1Type, msg);
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
    } // end for
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
  void addHeaderIgnore(std::wstring &key) {
      m_IgnoreMap.insert(key);
  }

private:

  // Header entries to ignore
  static void fillInToIgnore(std::set<std::wstring> &ignoreMap, const std::wstring &prefix) {
    ignoreMap.clear();
    ignoreMap.insert(prefix + L"exec-guid");
    ignoreMap.insert(prefix + L"analysis-guid");
    ignoreMap.insert(prefix + L"time-str"); 
    ignoreMap.insert(prefix + L"free-mem");    
    ignoreMap.insert(prefix + L"cvs-id");
    ignoreMap.insert(prefix + L"version");
    ignoreMap.insert(prefix + L"opt-block-size");
    ignoreMap.insert(prefix + L"opt-out-dir");
  }

  /** 
   * Check to see if two generic types are equivalent. If floats then
   * makes sure they are withing epsilon (small number). Otherwise
   * tested as strings which should work for other types.
   * @param gold - Type with "correct" values.
   * @param generated - Type with values to be tested.
   * @return - true if same, false otherwise.
   */
  bool paramNamValTypeSame(ParameterNameValueType &gold, ParameterNameValueType &generated) {
    string goldName, genName, goldValue, genValue;
    /* For ease in debugger... */
    goldName = StringUtils::ConvertWCSToMBS(gold.GetName());
    genName = StringUtils::ConvertWCSToMBS(generated.GetName());
    goldValue = StringUtils::ConvertWCSToMBS(gold.ToString());
    genValue = StringUtils::ConvertWCSToMBS(generated.ToString());
    //    bool match = gold.GetName() == generated.GetName();
    bool match = true;
    if(gold.GetParameterType() == ParameterNameValueType::FloatType) {
      float goldFloat = gold.GetValueFloat();
      float genFloat = generated.GetValueFloat();
      double maxDiff = 0;
      checkFloat(goldFloat, genFloat, m_EpsParam, match, maxDiff);
    }
    // othewise compare as string.
    else {
      match &= gold.ToString() == generated.ToString();
    }
    return match;
  }

  /** 
   * Utility function for checking if two vectors of generic types are the same.
   * @param gold - Vector with the "correct" values.
   * @param generated - Vector to be tested against the gold values.
   * @return - true if same, false othewise.
   */
  bool paramVectorsSame(vector<ParameterNameValueType> &gold, vector<ParameterNameValueType> &generated) {
    bool match = true;
    if(gold.size() != generated.size())
      return false;
    for(int i = 0; i < gold.size(); i++) {
      match &= paramNamValTypeSame(gold[i], generated[i]);
    }
    return match;
  }
  
  
  bool ParameterNameValueTypeMostlySame(ParameterNameValueTypeVector &gold, ParameterNameValueTypeVector &test, 
                                        std::string &msg, std::wstring &prefix) {
    bool same = true;
    map<wstring,ParameterNameValueType> testMap;
    map<wstring,ParameterNameValueType>::iterator testMapIter;
    ParameterNameValueTypeVector::iterator goldIter; 
    ParameterNameValueTypeVector::iterator testIter;
    // Load up test as a map which will be queried by items in gold.
    for(testIter = test.begin(); testIter != test.end(); ++testIter) {
      testMap[testIter->GetName()] = *testIter;
    }
    for(goldIter = gold.begin(); goldIter != gold.end(); ++goldIter) {
      // ignore items that are in the ignoreMap
      if(m_IgnoreMap.find(goldIter->GetName()) == m_IgnoreMap.end()) {
        testMapIter = testMap.find(goldIter->GetName());
        if(testMapIter == testMap.end()) {
          msg += " Error: Test missing field: '" + StringUtils::ConvertWCSToMBS(goldIter->GetName()) + "'";
          same = false;
        }
        else {
          string goldName, genName, goldValue, genValue;
          /* For ease in debugger... */
          goldName = StringUtils::ConvertWCSToMBS(goldIter->GetName());
          genName = StringUtils::ConvertWCSToMBS(testMapIter->second.GetName());
          goldValue = StringUtils::ConvertWCSToMBS(goldIter->ToString());
          genValue = StringUtils::ConvertWCSToMBS(testMapIter->second.ToString());
          if(!paramNamValTypeSame(testMapIter->second, *goldIter) &&
             Util::fileRoot(genValue) != Util::fileRoot(goldValue)) {
            msg += " Error: for field '" + goldName + "' expecting: '" + goldValue + 
              "' got: '" + genValue + "'";
            same = false;
          }
        }
      }
    }
    return same;
  }

  /** 
   * Are the headers for to CHP files the same?
   * Ignore some fields known to change like time, run id, etc.
   * @param goldChp - "correct" chp file.
   * @param generatedChp - chp file to be tested against gold standard.
   * @param msgs - ongoing list of messages about what when wrong if anything.
   * @return - true if headers are equivalent, false otherwise.
   */
  bool headersSame(CHPData &goldChp, CHPData &generatedChp, std::string &msgs) {
    bool success = true;
    FileHeader *goldHeader = NULL, *genHeader = NULL;
    goldHeader = goldChp.GetFileHeader();
    genHeader = generatedChp.GetFileHeader();
    /* Check the version. */
    success &= checkMsg(goldHeader->GetVersion() == genHeader->GetVersion(), "Error: different chp versions.", msgs);

    /* Check the group numbers. */
    success &= checkMsg(goldHeader->GetNumDataGroups() == genHeader->GetNumDataGroups(), 
                        "Error: Different group counts.", msgs);

    /* What type of array chp is from. */
    success &= checkMsg(goldChp.GetArrayType() == generatedChp.GetArrayType(), 
                        "Error: Different array types. [" + StringUtils::ConvertWCSToMBS(goldChp.GetArrayType()) + " != " + 
                        StringUtils::ConvertWCSToMBS(generatedChp.GetArrayType()) + "]", msgs);
    
    /* Algorithm used. */
    success &= checkMsg(goldChp.GetAlgName() == generatedChp.GetAlgName(), 
                        "Error: Different algorithm names.", msgs);
    
    /* Algorithm version. */
    success &= checkMsg(goldChp.GetAlgVersion() == generatedChp.GetAlgVersion(), 
                        "Error: Different algorithm names.", msgs);
    
    
    ParameterNameValueTypeVector goldList = goldChp.GetChipSums();
    ParameterNameValueTypeVector genList = generatedChp.GetChipSums();
    success &= ParameterNameValueTypeMostlySame(goldList, genList, msgs, m_Prefix);

    goldList = goldChp.GetAlgParams();
    genList = generatedChp.GetAlgParams();
    success &= ParameterNameValueTypeMostlySame(goldList, genList, msgs, m_Prefix);
    return success;
  }


  /** 
   * Determine if two gentoyping chp file entries are equivalent.
   * @param goldEntry - "correct" entry.
   * @param generatedEntry - entry to be tested against gold standard.
   * @param localSuccess - set to to false if entries not equivalent.
   * @param maxDiff - maximum difference seen so far.
   * @param numDiff - number of differences seen so far, incremented if these entries not equivalent.
   */
  void checkMultiDataGenotypeEntry(ProbeSetMultiDataGenotypeData &goldEntry, ProbeSetMultiDataGenotypeData &generatedEntry, 
                         bool &localSuccess, double &maxDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name);
    }
    if(goldEntry.call != generatedEntry.call) {
      localSuccess = false;
      reportError("Different calls for snp: '" + goldEntry.name + "'. gold: '" + 
                  goldEntry.name + "' test: '" + generatedEntry.name + "'");
    }
    checkFloat(goldEntry.confidence, generatedEntry.confidence, m_EpsConfidence, localSuccess, maxDiff);
    if(!paramVectorsSame(goldEntry.metrics, generatedEntry.metrics)) {
      localSuccess = false;
      reportError("Param vectors different for snp: '" + goldEntry.name + "'.");
    }
    if(!localSuccess) {
      numDiff++;
    }
  }

  /** 
   * Determine if two expression chp file entries are equivalent.
   * @param goldEntry - "correct" entry.
   * @param generatedEntry - entry to be tested against gold standard.
   * @param localSuccess - set to to false if entries not equivalent.
   * @param maxDiff - maximum difference seen so far.
   * @param numDiff - number of differences seen so far, incremented if these entries not equivalent.
   */
  void checkMultiDataExpressionEntry(ProbeSetMultiDataExpressionData &goldEntry, ProbeSetMultiDataExpressionData &generatedEntry, 
                         bool &localSuccess, double &maxDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name + "'");
    }
    checkFloat(goldEntry.quantification, generatedEntry.quantification, m_EpsQuantification, localSuccess, maxDiff);
    if(!paramVectorsSame(goldEntry.metrics, generatedEntry.metrics)) {
      localSuccess = false;
      reportError("Param vectors different for probeset: '" + goldEntry.name + "'.");
    }
    if(!localSuccess) {
      numDiff++;
    }
  }

  /** 
   * Determine if two expression chp file entries are equivalent.
   * @param goldEntry - "correct" entry.
   * @param generatedEntry - entry to be tested against gold standard.
   * @param localSuccess - set to to false if entries not equivalent.
   * @param maxDiff - maximum difference seen so far.
   * @param numDiff - number of differences seen so far, incremented if these entries not equivalent.
   */
  void checkQuantificationEntry(ProbeSetQuantificationData &goldEntry, ProbeSetQuantificationData &generatedEntry, 
                         bool &localSuccess, double &maxDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name + "'");
    }
    if(goldEntry.id != generatedEntry.id) {
      localSuccess = false;
      reportError("Different ids. gold: '" + ToStr(goldEntry.id) + "' test: '" + ToStr(generatedEntry.id) + "'");
    }
    checkFloat(goldEntry.quantification, generatedEntry.quantification, m_EpsQuantification, localSuccess, maxDiff);
    if(!localSuccess) {
      numDiff++;
    }
  }

  /** 
   * Check to see if the genotype entries for MultiDataType are the same.
   * @param goldChp - "correct" data.
   * @param generatedChp - chp file data to test against gold standard.
   * @param msgs - any ongoing messages for user.
   * @param dataType - Either genotype or control genotype, they are conceptually  separate in chp file.
   * @return - true if genotype entries are equivalent, false otherwise.
   */
  bool multiDataGenotypeSame(CHPMultiDataData &goldChp, CHPMultiDataData &generatedChp, std::string &msgs, MultiDataType dataType) {
    bool success = true;
    double maxDiff = -1;
    int numDiff = 0;
    int numGenotyping = goldChp.GetEntryCount(dataType);
    int genNum = generatedChp.GetEntryCount(dataType);
    assert(dataType == GenotypeMultiDataType);
    Verbose::out(4, ToStr(numGenotyping) + " " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + " genotyping probesets.");
    if(!checkMsg(numGenotyping == genNum, "Wrong number of genotyping probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(numGenotyping == 0)
      return true;
    for(int i = 0; i < numGenotyping; i++) {
      bool localSuccess = true;
      ProbeSetMultiDataGenotypeData goldEntry, generatedEntry;
      goldChp.GetGenotypeEntry(dataType, i, goldEntry);
      generatedChp.GetGenotypeEntry(dataType, i, generatedEntry);
      checkMultiDataGenotypeEntry(goldEntry, generatedEntry, localSuccess, maxDiff, numDiff);
      success &= localSuccess;
    }
    if(maxDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [confidence]");
    if(numDiff > 0)
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(numGenotyping) + " (" + 
                   ToStr(100.0 * numDiff/numGenotyping) + "%) were different.");
    if(!success && m_DiffAllowed >= numDiff) 
        success = true;
    if(!success) {
      msgs += "Error: " + generatedChp.GetFilename() + " is different from " + goldChp.GetFilename() + ". ";
    }
    std::string res = "different";
    if(success) 
        res = "equivalent";
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": checked " + ToStr(numGenotyping) + " genotype entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max confidence diff is: " + ToStr(maxDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": chip is " + res + ".");
    return success;
  }
  
  /** 
   * Check to see if the expression entries for MultiDataType are the same.
   * @param goldChp - "correct" data.
   * @param generatedChp - chp file data to test against gold standard.
   * @param msgs - any ongoing messages for user.
   * @param dataType - Either expression or control expression, they are conceptually separate in chp file.
   * @return - true if expression entries are equivalent, false otherwise.
   */
  bool multiDataExpressionSame(CHPMultiDataData &goldChp, CHPMultiDataData &generatedChp, std::string &msgs, MultiDataType dataType) {
    bool success = true;
    double maxDiff = -1;
    int numDiff = 0;
    assert(dataType == ExpressionMultiDataType);
    int numExpr = goldChp.GetEntryCount(dataType);
    int genNum = generatedChp.GetEntryCount(dataType);
    Verbose::out(4, ToStr(numExpr) + " " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + " expression probesets.");
    if(!checkMsg(numExpr == genNum, "Wrong number of expression probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(numExpr == 0)
      return true;
    for(int i = 0; i < numExpr; i++) {
      bool localSuccess = true;
      ProbeSetMultiDataExpressionData goldEntry, generatedEntry;
      goldChp.GetExpressionEntry(dataType, i, goldEntry);
      generatedChp.GetExpressionEntry(dataType, i, generatedEntry);
      checkMultiDataExpressionEntry(goldEntry, generatedEntry, localSuccess, maxDiff, numDiff);
      success &= localSuccess;
    }
    if(maxDiff > m_EpsQuantification)
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_EpsQuantification) + ") [quantification]");
    if(numDiff > 0 )
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(numExpr) + " (" + 
                   ToStr(100.0 * numDiff/numExpr) + "%) were different.");
    if(!success) {
      msgs += "Error: " + generatedChp.GetFilename() + " is different from " + goldChp.GetFilename() + ". ";
      
    }
    std::string res = "different";
    if(success) 
        res = "equivalent";
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": checked " + ToStr(numExpr) + " expression entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max signal diff is: " + ToStr(maxDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": chip is " + res + ".");
    return success;
  }

  /** 
   * Check to see if the expression entries for Quantification  are the same.
   * @param goldChp - "correct" data.
   * @param generatedChp - chp file data to test against gold standard.
   * @param msgs - any ongoing messages for user.
   * @return - true if expression entries are equivalent, false otherwise.
   */
  bool quantificationDataSame(CHPQuantificationData &goldChp, CHPQuantificationData &generatedChp, std::string &msgs) {
    bool success = true;
    double maxDiff = -1;
    int numDiff = 0;
    int numExpr = goldChp.GetEntryCount();
    int genNum = generatedChp.GetEntryCount();
    Verbose::out(4, ToStr(numExpr) + " expression probesets.");
    if(!checkMsg(numExpr == genNum, "Wrong number of expression probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(numExpr == 0)
      return true;
    for(int i = 0; i < numExpr; i++) {
      bool localSuccess = true;
      ProbeSetQuantificationData goldEntry, generatedEntry;
      goldChp.GetQuantificationEntry(i, goldEntry);
      generatedChp.GetQuantificationEntry(i, generatedEntry);
      checkQuantificationEntry(goldEntry, generatedEntry, localSuccess, maxDiff, numDiff);
      success &= localSuccess;
    }
    if(maxDiff > m_EpsQuantification)
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_EpsQuantification) + ") [quantification]");
    if(numDiff > 0 )
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(numExpr) + " (" + 
                   ToStr(100.0 * numDiff/numExpr) + "%) were different.");
    if(!success) {
      msgs += "Error: " + generatedChp.GetFilename() + " is different from " + goldChp.GetFilename() + ". ";
      
    }
    std::string res = "different";
    if(success) 
        res = "equivalent";
    Verbose::out(1, generatedChp.GetFilename() + ": checked " + ToStr(numExpr) + " quantification entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": max signal diff is: " + ToStr(maxDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": chip is " + res + ".");
    return success;
  }

  /// Filenames for the gold standard or correct data chp files.
  std::vector<std::string> m_Gold;
  /// Matching filenames for the chp files to be tested.
  std::vector<std::string> m_Generated;
  /// Epsilon, small value that two floats can differ by but still be considered equivalent.
  double m_EpsConfidence;
  double m_EpsQuantification;
  double m_EpsParam;
  /// How many differences will we tolerate?
  int m_DiffAllowed;
  /// What is the expected prefix for parameter names?
  std::wstring m_Prefix;
  /// Header entries to ignore
  std::set<std::wstring> m_IgnoreMap;
  
};

#endif /* CALVINCHPCHECK_H */
