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
 * @file   CalvinChpCheck.h
 * @author Chuck Sugnet
 * @date   Tue Apr 25 18:03:02 2006
 * 
 * @brief  Class for doing a check of two CHP files after regression run.
 */

#ifndef CALVINCHPCHECK_H
#define CALVINCHPCHECK_H

//
#include "calvin_files/data/src/CHPData.h"
#include "calvin_files/data/src/CHPMultiDataData.h"
#include "calvin_files/parsers/src/CHPFileReader.h"
#include "calvin_files/parsers/src/CHPMultiDataFileReader.h"
#include "calvin_files/parsers/src/CHPQuantificationDetectionFileReader.h"
#include "calvin_files/parsers/src/CHPQuantificationFileReader.h"
#include "calvin_files/utils/src/StringUtils.h"
#include "portability/affy-base-types.h"
#include "util/AffxByteArray.h"
#include "util/Fs.h"
#include "util/RegressionCheck.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_data;

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
   * @param diffAllowed - number of differences allowed
   * @param prefix - argument name prefix in header (default: "apt-")
   * @param eps - epsilon: Maximum accepted absolute difference in numeric values.
   *            i.e. if |generated-gold| >= eps then there is a difference.
   * @param bCheckHeaders Check headers? (boolean, default: true).
   * @param frac - Maximum accepted fractional difference in numeric values (not used by default).
   *            i.e. if |generated-gold| >= frac*max(|generated|,|gold|) then there is a difference.
   */
  CalvinChpCheck(
                 std::vector<std::string> &generated, 
                 std::vector<std::string> &gold, 
                 int diffAllowed=0,
                 const std::wstring &prefix=L"apt-", 
                 double eps=0.0001,
                 bool bCheckHeaders = true,
				 double fraction = 0.0 ) {
    m_Name = "AGCC-CHP-Check";
    m_Generated = generated;
    m_Gold = gold;
    m_EpsConfidence = eps;
    m_FracConfidence = fraction;
    m_EpsQuantification = eps;
    m_FracQuantification = fraction;
    m_CheckHeaders = bCheckHeaders;
    /// @todo allow for a vector of eps for checking extra metrics
    m_EpsParam = eps;
	m_FracParam = fraction;
    m_DiffAllowed = diffAllowed;
    m_Prefix = prefix;

    fillInToIgnore(m_IgnoreMap, prefix); // things know to change like user, time, etc.

    setMaxError(30);
  }
  CalvinChpCheck(const std::string &generated, 
                 const std::string &gold, 
                 int diffAllowed=0,
                 const std::wstring &prefix=L"apt-", 
                 double eps=0.0001,
                 bool bCheckHeaders = true,
				 double fraction = 0.0) {
    m_Generated.push_back(generated);
    m_Gold.push_back(gold);
    m_EpsConfidence = eps;
    m_FracConfidence = fraction;
    m_EpsQuantification = eps;
    m_FracQuantification = fraction;
    m_CheckHeaders = bCheckHeaders;
    /// @todo allow for a vector of eps for checking extra metrics
    m_EpsParam = eps;
	m_FracParam = fraction;
    m_DiffAllowed = diffAllowed;
    m_Prefix = prefix;

    fillInToIgnore(m_IgnoreMap, prefix); // things know to change like user, time, etc.

    setMaxError(30);
  }


  /** 
   * Utility function to set the max number of errors to report
   * @param max - maximum number of errors to report (-1 for no limit)
   * @return - void
   */
  void setDiffReportMax(int max) {
    setMaxError(max);
  }


  /** 
   * Check to make sure that two files are the same +/- some epsilon. 
   * @param msg - Fills in an error message if test fails, empty string otherwise.
   * @return - Returns true if files are close enough, false otherwise.
   */
  bool check(std::string &msg) {
    bool success = true;
    std::string current_filename;
            
    std::map<MultiDataType, bool> validDataSets;
    validDataSets[CopyNumberMultiDataType] = true;
    validDataSets[GenotypeMultiDataType] = true;
    validDataSets[ExpressionMultiDataType] = true;
    validDataSets[CopyNumberVariationMultiDataType] = true;
    validDataSets[DmetCopyNumberMultiDataType] = true;
    validDataSets[DmetMultiAllelicMultiDataType] = true;
    validDataSets[DmetBiAllelicMultiDataType] = true;

    if(m_Generated.size() != m_Gold.size()) {
      return checkMsg(false, "CalvinChpCheck::check() - generated and gold vectors must be same size.",msg);
    }

    for (size_t i = 0; i < m_Generated.size(); i++) {
        try {
            m_Generated[i] = Fs::convertToUncPath(m_Generated[i]);
            m_Gold[i] = Fs::convertToUncPath(m_Gold[i]);

            CHPData chp1, chp2;
            CHPFileReader reader;
            reader.SetFilename(m_Generated[i]);
            current_filename=m_Generated[i];
            reader.Read(chp1);
            reader.SetFilename(m_Gold[i]);
            current_filename=m_Gold[i];
            reader.Read(chp2);

            std::string chp1Type = chp1.GetFileHeader()->GetGenericDataHdr()->GetFileTypeId();
            std::string chp2Type = chp2.GetFileHeader()->GetGenericDataHdr()->GetFileTypeId();

            success &= checkMsg(chp2Type == chp1Type, 
                                "Error: Different CHP Types: " + chp2Type + ", " + chp1Type, msg);

            if (m_CheckHeaders && !headersSame(chp2, chp1, msg))
                    success = false;

            if(chp1Type == CHP_MULTI_DATA_TYPE) {
                CHPMultiDataData generatedChp, goldChp;
                CHPMultiDataFileReader chpReader;
                chpReader.SetFilename(m_Generated[i]);
                current_filename=m_Generated[i];
                chpReader.Read(generatedChp);
                chpReader.SetFilename(m_Gold[i]);
                current_filename=m_Gold[i];
                chpReader.Read(goldChp);

	            std::map<MultiDataType, DataSetInfo> dataSets;
	            std::map<MultiDataType, DataSetInfo>::iterator dataIter;
                dataSets = generatedChp.GetDataSetInfo();

                for(dataIter=dataSets.begin(); dataIter != dataSets.end(); dataIter++) {
                    if(validDataSets.find(dataIter->first) == validDataSets.end() || !(validDataSets.find(dataIter->first)->second)){
                        success &= checkMsg(false, "Error: Unable to check multi data type '" + 
                                    StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataIter->first]) + "'", msg);
                    }
                }

                if(!multiDataCopyNumberSame(goldChp, generatedChp, msg, CopyNumberMultiDataType)) {
                    success = false;
                }
                if(!multiDataGenotypeSame(goldChp, generatedChp, msg, GenotypeMultiDataType)) {
                    success = false;
                }
                if(!multiDataExpressionSame(goldChp, generatedChp, msg, ExpressionMultiDataType)) {
                    success = false;
                }
                if(!multiDataCopyNumberVariationSame(goldChp, generatedChp, msg, CopyNumberVariationMultiDataType)) {
                    success = false;
                }
                if(!multiDataDmetCopyNumberSame(goldChp, generatedChp, msg, DmetCopyNumberMultiDataType)) {
                    success = false;
                }
                if(!multiDataDmetMultiAllelicSame(goldChp, generatedChp, msg, DmetMultiAllelicMultiDataType)) {
                    success = false;
                }
                if(!multiDataDmetBiAllelicSame(goldChp, generatedChp, msg, DmetBiAllelicMultiDataType)) {
                    success = false;
                }
            } else if(chp1Type == "affymetrix-quantification-analysis") {
                CHPQuantificationData generatedChp, goldChp;
                CHPQuantificationFileReader chpReader;
                chpReader.SetFilename(m_Generated[i]);
                current_filename=m_Generated[i];
                chpReader.Read(generatedChp);
                chpReader.SetFilename(m_Gold[i]);
                current_filename=m_Gold[i];
                chpReader.Read(goldChp);
                if(!quantificationDataSame(goldChp, generatedChp, msg)) {
                    success = false;
                }
            } else if(chp1Type == "affymetrix-expression-probeset-analysis") {
                CHPQuantificationDetectionData generatedChp, goldChp;
                CHPQuantificationDetectionFileReader chpReader;
                chpReader.SetFilename(m_Generated[i]);
                current_filename=m_Generated[i];
                chpReader.Read(generatedChp);
                chpReader.SetFilename(m_Gold[i]);
                current_filename=m_Gold[i];
                chpReader.Read(goldChp);
                if(!quantificationDetectionDataSame(goldChp, generatedChp, msg)) {
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
                                FS_QUOTE_PATH(current_filename) +
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
    ignoreMap.insert(L"program-version");
    ignoreMap.insert(L"apt-opt-program-version");
    ignoreMap.insert(prefix + L"exec-guid");
    ignoreMap.insert(prefix + L"analysis-guid");
    ignoreMap.insert(prefix + L"time-str"); 
    ignoreMap.insert(prefix + L"free-mem");    
    ignoreMap.insert(prefix + L"cvs-id");
    ignoreMap.insert(prefix + L"version");
    ignoreMap.insert(prefix + L"opt-out-dir");
    ignoreMap.insert(prefix + L"opt-temp-dir");
    ignoreMap.insert(prefix + L"opt-exec-guid");
    ignoreMap.insert(prefix + L"state-exec-guid");
    ignoreMap.insert(prefix + L"opt-out-dir");
	
    ignoreMap.insert(L"option-program-cvs-id");
    ignoreMap.insert(L"option-version-to-report");
    ignoreMap.insert(L"option-command-line");
    ignoreMap.insert(L"option-exec-guid");
    ignoreMap.insert(L"option-run-probeset-genotype");
    ignoreMap.insert(L"option-cels");
    ignoreMap.insert(L"option-out-dir");
    ignoreMap.insert(L"option-temp-dir");
    ignoreMap.insert(prefix + L"state-time-start");
    ignoreMap.insert(prefix + L"state-free-mem-at-start");
    ignoreMap.insert(prefix + L"state-analysis-guid");

    ignoreMap.insert(prefix + L"opt-program-cvs-id");
    ignoreMap.insert(prefix + L"opt-version-to-report");
    ignoreMap.insert(prefix + L"opt-command-line");

    ignoreMap.insert(prefix + L"state-program-cvs-id");
    ignoreMap.insert(prefix + L"state-program-version");
    ignoreMap.insert(prefix + L"state-version-to-report");
    ignoreMap.insert(prefix + L"state-command-line");

    ignoreMap.insert(prefix + L"command-line");
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
    std::string goldName, genName, goldValue, genValue;
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
        checkFloat(goldFloat, genFloat, m_EpsParam, match, maxDiff, false, m_FracParam);
    }
    // othewise compare as string.
    else {
        // ignore path separators.
        AffxByteArray baGold(StringUtils::ConvertWCSToMBS(gold.ToString()));
        AffxByteArray baGenerated(StringUtils::ConvertWCSToMBS(generated.ToString()));
        baGold.replace("\\", "/");
        baGenerated.replace("\\", "/");
        match &= baGold.toString() == baGenerated.toString();
    }
    return match;
  }

  /** 
   * Utility function for checking if two vectors of generic types are the same.
   * @param gold - Vector with the "correct" values.
   * @param generated - Vector to be tested against the gold values.
   * @return - true if same, false othewise.
   */
  bool paramVectorsSame(std::vector<ParameterNameValueType> &gold, std::vector<ParameterNameValueType> &generated) {
    bool match = true;
    if(gold.size() != generated.size())
      return false;
    for(size_t i = 0; i < gold.size(); i++) {
      match &= paramNamValTypeSame(gold[i], generated[i]);
    }
    return match;
  }
  
  
  bool ParameterNameValueTypeMostlySame(ParameterNameValueTypeVector &gold, ParameterNameValueTypeVector &test, 
                                        std::string &msg, std::wstring &prefix) {
    bool same = true;
    std::map<std::wstring,ParameterNameValueType> testMap;
    std::map<std::wstring,ParameterNameValueType>::iterator testMapIter;
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
          std::string goldName, genName, goldValue, genValue;
          /* For ease in debugger... */
          goldName = StringUtils::ConvertWCSToMBS(goldIter->GetName());
          genName = StringUtils::ConvertWCSToMBS(testMapIter->second.GetName());
          goldValue = StringUtils::ConvertWCSToMBS(goldIter->ToString());
          genValue = StringUtils::ConvertWCSToMBS(testMapIter->second.ToString());
          if(!paramNamValTypeSame(testMapIter->second, *goldIter) &&
            Fs::basename(genValue) != Fs::basename(goldValue)) {
            msg += " Error: for field '" + goldName + "' expecting: '" + goldValue + "' got: '" + genValue + "'";
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
                        "Error: Different algorithm version.", msgs);
    
    
    ParameterNameValueTypeVector goldList = goldChp.GetChipSums();
    ParameterNameValueTypeVector genList = generatedChp.GetChipSums();
    success &= ParameterNameValueTypeMostlySame(goldList, genList, msgs, m_Prefix);

    goldList = goldChp.GetAlgParams();
    genList = generatedChp.GetAlgParams();
    success &= ParameterNameValueTypeMostlySame(goldList, genList, msgs, m_Prefix);
    return success;
  }

  void checkMultiDataDmetCopyNumberEntry(DmetCopyNumberData &goldEntry, DmetCopyNumberData &generatedEntry, 
                         bool &localSuccess, double &maxConfDiff, double &maxSignalDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name);
    }

    if(goldEntry.call != generatedEntry.call) {
      localSuccess = false;
      reportError("Different calls for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.call) + "' test: '" + ToStr(generatedEntry.call) + "'");
    }

    if(goldEntry.force != generatedEntry.force) {
      localSuccess = false;
      reportError("Different forced calls for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.force) + "' test: '" + ToStr(generatedEntry.force) + "'");
    }

    if(goldEntry.lower != generatedEntry.lower) {
      localSuccess = false;
      reportError("Different lower for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.lower) + "' test: '" + ToStr(generatedEntry.lower) + "'");
    }

    if(goldEntry.upper != generatedEntry.upper) {
      localSuccess = false;
      reportError("Different upper for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.upper) + "' test: '" + ToStr(generatedEntry.upper) + "'");
    }

    checkFloat(goldEntry.confidence, generatedEntry.confidence, m_EpsConfidence, localSuccess, maxConfDiff, false, m_FracConfidence);
    checkFloat(goldEntry.estimate, generatedEntry.estimate, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);

    if(!paramVectorsSame(goldEntry.metrics, generatedEntry.metrics)) {
      localSuccess = false;
      reportError("Param vectors different for snp: '" + goldEntry.name + "'.");
    }

    if(!localSuccess) {
      numDiff++;
    }
  }

  void checkMultiDataDmetMultiAllelicEntry(DmetMultiAllelicData &goldEntry, DmetMultiAllelicData &generatedEntry, 
                         bool &localSuccess, double &maxConfDiff, double &maxSignalDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name);
    }

    if(goldEntry.call != generatedEntry.call) {
      localSuccess = false;
      reportError("Different calls for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.call) + "' test: '" + ToStr(generatedEntry.call) + "'");
    }

    if(goldEntry.force != generatedEntry.force) {
      localSuccess = false;
      reportError("Different forced calls for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.force) + "' test: '" + ToStr(generatedEntry.force) + "'");
    }

    if(goldEntry.contextA != generatedEntry.contextA) {
      localSuccess = false;
      reportError("Different contextA for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextA) + "' test: '" + ToStr(generatedEntry.contextA) + "'");
    }

    if(goldEntry.contextB != generatedEntry.contextB) {
      localSuccess = false;
      reportError("Different contextB for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextB) + "' test: '" + ToStr(generatedEntry.contextB) + "'");
    }

    if(goldEntry.contextC != generatedEntry.contextC) {
      localSuccess = false;
      reportError("Different contextC for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextC) + "' test: '" + ToStr(generatedEntry.contextC) + "'");
    }

    if(goldEntry.contextD != generatedEntry.contextD) {
      localSuccess = false;
      reportError("Different contextD for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextD) + "' test: '" + ToStr(generatedEntry.contextD) + "'");
    }

    if(goldEntry.contextE != generatedEntry.contextE) {
      localSuccess = false;
      reportError("Different contextE for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextE) + "' test: '" + ToStr(generatedEntry.contextE) + "'");
    }

    if(goldEntry.contextF != generatedEntry.contextF) {
      localSuccess = false;
      reportError("Different contextF for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextF) + "' test: '" + ToStr(generatedEntry.contextF) + "'");
    }

    if(goldEntry.alleleCount != generatedEntry.alleleCount) {
      localSuccess = false;
      reportError("Different alleleCount for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.alleleCount) + "' test: '" + ToStr(generatedEntry.alleleCount) + "'");
    }

    checkFloat(goldEntry.confidence, generatedEntry.confidence, m_EpsConfidence, localSuccess, maxConfDiff, false, m_FracConfidence);

    checkFloat(goldEntry.signalA, generatedEntry.signalA, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);
    checkFloat(goldEntry.signalB, generatedEntry.signalB, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);
    checkFloat(goldEntry.signalC, generatedEntry.signalC, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);
    checkFloat(goldEntry.signalD, generatedEntry.signalD, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);
    checkFloat(goldEntry.signalE, generatedEntry.signalE, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);
    checkFloat(goldEntry.signalF, generatedEntry.signalF, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);

    if(!paramVectorsSame(goldEntry.metrics, generatedEntry.metrics)) {
      localSuccess = false;
      reportError("Param vectors different for snp: '" + goldEntry.name + "'.");
    }

    if(!localSuccess) {
      numDiff++;
    }
  }

  void checkMultiDataDmetBiAllelicEntry(DmetBiAllelicData &goldEntry, DmetBiAllelicData &generatedEntry, 
                         bool &localSuccess, double &maxConfDiff, double &maxSignalDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name);
    }

    if(goldEntry.call != generatedEntry.call) {
      localSuccess = false;
      reportError("Different calls for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.call) + "' test: '" + ToStr(generatedEntry.call) + "'");
    }

    if(goldEntry.force != generatedEntry.force) {
      localSuccess = false;
      reportError("Different forced calls for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.force) + "' test: '" + ToStr(generatedEntry.force) + "'");
    }

    if(goldEntry.contextA != generatedEntry.contextA) {
      localSuccess = false;
      reportError("Different contextA for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextA) + "' test: '" + ToStr(generatedEntry.contextA) + "'");
    }

    if(goldEntry.contextB != generatedEntry.contextB) {
      localSuccess = false;
      reportError("Different contextB for cnv: '" + goldEntry.name + "'. gold: '" + 
                  ToStr(goldEntry.contextB) + "' test: '" + ToStr(generatedEntry.contextB) + "'");
    }

    checkFloat(goldEntry.confidence, generatedEntry.confidence, m_EpsConfidence, localSuccess, maxConfDiff, false, m_FracConfidence);

    checkFloat(goldEntry.signalA, generatedEntry.signalA, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);
    checkFloat(goldEntry.signalB, generatedEntry.signalB, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);

    if(!paramVectorsSame(goldEntry.metrics, generatedEntry.metrics)) {
      localSuccess = false;
      reportError("Param vectors different for snp: '" + goldEntry.name + "'.");
    }

    if(!localSuccess) {
      numDiff++;
    }
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
    if(!checkFloat(goldEntry.confidence, generatedEntry.confidence, m_EpsConfidence, localSuccess, maxDiff, false, m_FracConfidence)) {
      reportError("Confidence is different for snp: '" + goldEntry.name + "'.");
    }
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
    checkFloat(goldEntry.quantification, generatedEntry.quantification, m_EpsQuantification, localSuccess, maxDiff, false, m_FracQuantification);
    if(!paramVectorsSame(goldEntry.metrics, generatedEntry.metrics)) {
      localSuccess = false;
      reportError("Param vectors different for probeset: '" + goldEntry.name + "'.");
    }
    if(!localSuccess) {
      numDiff++;
    }
  }

  /** 
   * Determine if two copnumber chp file entries are equivalent.
   * @param goldEntry - "correct" entry.
   * @param generatedEntry - entry to be tested against gold standard.
   * @param localSuccess - set to to false if entries not equivalent.
   * @param maxLog2RatioDiff - maximum difference seen so far.
   * @param maxSmoothSignalDiff - maximum difference seen so far.
   * @param maxAllelicDifferenceDiff - maximum difference seen so far.
   * @param numDiff - number of differences seen so far, incremented if these entries not equivalent.
   */
  void checkMultiDataCopyNumberEntry(ProbeSetMultiDataCopyNumberData &goldEntry, ProbeSetMultiDataCopyNumberData &generatedEntry, 
                         bool &localSuccess, double &maxLog2RatioDiff, double &maxSmoothSignalDiff, double &maxAllelicDifferenceDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name);
    }
    if(goldEntry.chr != generatedEntry.chr) {
      localSuccess = false;
      reportError("Different Chromosome for snp: '" + goldEntry.name + "'. gold: '" + 
                  goldEntry.name + "' test: '" + generatedEntry.name + "'");
    }
    if(goldEntry.position != generatedEntry.position) {
      localSuccess = false;
      reportError("Different Position for snp: '" + goldEntry.name + "'. gold: '" + 
                  goldEntry.name + "' test: '" + generatedEntry.name + "'");
    }
    if (goldEntry.metrics.at(0).GetValueFloat() != generatedEntry.metrics.at(0).GetValueFloat()) {
      localSuccess = false;
      reportError("Different CNState for snp: '" + goldEntry.name + "'. gold: '" + 
                  goldEntry.name + "' test: '" + generatedEntry.name + "'");
    }
    if (localSuccess)
        checkFloat(goldEntry.metrics.at(1).GetValueFloat(), 
                   generatedEntry.metrics.at(1).GetValueFloat(), 
                   m_EpsConfidence, localSuccess, maxLog2RatioDiff, false, m_FracConfidence);
    if (localSuccess) 
        checkFloat(goldEntry.metrics.at(2).GetValueFloat(), 
                   generatedEntry.metrics.at(2).GetValueFloat(), 
                   m_EpsConfidence, localSuccess, maxSmoothSignalDiff, false, m_FracConfidence);
    if (localSuccess) 
        checkFloat(goldEntry.metrics.at(4).GetValueFloat(), 
                   generatedEntry.metrics.at(4).GetValueFloat(), 
                   m_EpsConfidence, localSuccess, maxAllelicDifferenceDiff, false, m_FracConfidence);
    if ((goldEntry.metrics.at(3).GetValueFloat() == 0) || (goldEntry.metrics.at(3).GetValueFloat() == 1) ||
        (generatedEntry.metrics.at(3).GetValueFloat() == 0) || (generatedEntry.metrics.at(3).GetValueFloat() == 1)) {
        if (goldEntry.metrics.at(3).GetValueFloat() != generatedEntry.metrics.at(3).GetValueFloat()) {
          localSuccess = false;
          reportError("Different LOH for snp: '" + goldEntry.name + "'. gold: '" + 
          goldEntry.name + "' test: '" + generatedEntry.name + "'");
        }
    }

    if (!localSuccess) 
        numDiff++;
  }

  /** 
   * Determine if two cnv chp file entries are equivalent.
   * @param goldEntry - "correct" entry.
   * @param generatedEntry - entry to be tested against gold standard.
   * @param localSuccess - set to to false if entries not equivalent.
   * @param maxSignalDiff - maximum difference seen so far.
   * @param maxConfidenceDiff - maximum difference seen so far.
   * @param numDiff - number of differences seen so far, incremented if these entries not equivalent.
   */
  void checkMultiDataCopyNumberVariationEntry(ProbeSetMultiDataCopyNumberVariationRegionData &goldEntry, ProbeSetMultiDataCopyNumberVariationRegionData &generatedEntry, 
                         bool &localSuccess, double &maxSignalDiff, double &maxConfidenceDiff, int &numDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name);
    }
    if(goldEntry.call != generatedEntry.call) {
      localSuccess = false;
      reportError("Different calls. gold: '" + ToStr(goldEntry.call) + "' test: '" + ToStr(generatedEntry.call));
    }
    if (localSuccess) {
        checkFloat(goldEntry.signal, generatedEntry.signal, m_EpsConfidence, localSuccess, maxSignalDiff, false, m_FracConfidence);
        if(!localSuccess)
            reportError("Different signals. gold: '" + ToStr(goldEntry.signal) + "' test: '" + ToStr(generatedEntry.signal));
    }
    if (localSuccess){
        checkFloat(goldEntry.confidenceScore, generatedEntry.confidenceScore, m_EpsConfidence, localSuccess, maxConfidenceDiff, false, m_FracConfidence);
        if(!localSuccess)
            reportError("Different confidences. gold: '" + ToStr(goldEntry.confidenceScore) + "' test: '" + ToStr(generatedEntry.confidenceScore));
    }
    if (!localSuccess) 
        numDiff++;
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
    checkFloat(goldEntry.quantification, generatedEntry.quantification, m_EpsQuantification, localSuccess, maxDiff, false, m_FracQuantification);
    if(!localSuccess) {
      numDiff++;
    }
  }
  void checkQuantificationDetectionEntry(ProbeSetQuantificationDetectionData &goldEntry, ProbeSetQuantificationDetectionData &generatedEntry, 
                         bool &localSuccess, double &maxDiff, int &numDiff, double &pvalMaxDiff) {
    if(goldEntry.name != generatedEntry.name) {
      localSuccess = false;
      reportError("Different names. gold: '" + goldEntry.name + "' test: '" + generatedEntry.name + "'");
    }
    if(goldEntry.id != generatedEntry.id) {
      localSuccess = false;
      reportError("Different ids. gold: '" + ToStr(goldEntry.id) + "' test: '" + ToStr(generatedEntry.id) + "'");
    }
    checkFloat(goldEntry.quantification, generatedEntry.quantification, m_EpsQuantification, localSuccess, maxDiff, false, m_FracQuantification);
    checkFloat(goldEntry.pvalue, generatedEntry.pvalue, m_EpsConfidence, localSuccess, pvalMaxDiff, false, m_FracConfidence);
    if(!localSuccess) {
      numDiff++;
    }
  }

  bool multiDataDmetCopyNumberSame(CHPMultiDataData &goldChp, CHPMultiDataData &generatedChp, std::string &msgs, MultiDataType dataType) {
    bool success = true;
    double maxDiff = -1;
    double maxSignalDiff = -1;
    int numDiff = 0;
    int goldNum = goldChp.GetEntryCount(dataType);
    int genNum = generatedChp.GetEntryCount(dataType);
    assert(dataType == DmetCopyNumberMultiDataType);
    Verbose::out(4, ToStr(goldNum) + " " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + " probesets.");
    if(!checkMsg(goldNum == genNum, "Wrong number of genotyping probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(goldNum == 0)
      return true;
    for(int i = 0; i < goldNum; i++) {
      bool localSuccess = true;
      DmetCopyNumberData goldEntry, generatedEntry;
      goldChp.GetEntry(dataType, i, goldEntry);
      generatedChp.GetEntry(dataType, i, generatedEntry);
      checkMultiDataDmetCopyNumberEntry(goldEntry, generatedEntry, localSuccess, maxDiff, maxSignalDiff, numDiff);
      success &= localSuccess;
    }
    if(maxDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [confidence]");
    if(maxSignalDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxSignalDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [signal]");
    if(numDiff > 0)
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(goldNum) + " (" + 
                   ToStr(100.0 * numDiff/goldNum) + "%) were different.");
    if(!success && m_DiffAllowed >= numDiff) 
        success = true;
    if(!success) {
      msgs += "Error: " + generatedChp.GetFilename() + " is different from " + goldChp.GetFilename() + ". ";
    }
    std::string res = "different";
    if(success) 
        res = "equivalent";
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": checked " + ToStr(goldNum) + " genotype entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max confidence diff is: " + ToStr(maxDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max signal diff is: " + ToStr(maxSignalDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": chip is " + res + ".");
    return success;
  }

  bool multiDataDmetMultiAllelicSame(CHPMultiDataData &goldChp, CHPMultiDataData &generatedChp, std::string &msgs, MultiDataType dataType) {
    bool success = true;
    double maxDiff = -1;
    double maxSignalDiff = -1;
    int numDiff = 0;
    int goldNum = goldChp.GetEntryCount(dataType);
    int genNum = generatedChp.GetEntryCount(dataType);
    assert(dataType == DmetMultiAllelicMultiDataType);
    Verbose::out(4, ToStr(goldNum) + " " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + " probesets.");
    if(!checkMsg(goldNum == genNum, "Wrong number of genotyping probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(goldNum == 0)
      return true;
    for(int i = 0; i < goldNum; i++) {
      bool localSuccess = true;
      DmetMultiAllelicData goldEntry, generatedEntry;
      goldChp.GetEntry(dataType, i, goldEntry);
      generatedChp.GetEntry(dataType, i, generatedEntry);
      checkMultiDataDmetMultiAllelicEntry(goldEntry, generatedEntry, localSuccess, maxDiff, maxSignalDiff, numDiff);
      success &= localSuccess;
    }
    if(maxDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [confidence]");
    if(maxSignalDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxSignalDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [signal]");
    if(numDiff > 0)
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(goldNum) + " (" + 
                   ToStr(100.0 * numDiff/goldNum) + "%) were different.");
    if(!success && m_DiffAllowed >= numDiff) 
        success = true;
    if(!success) {
      msgs += "Error: " + generatedChp.GetFilename() + " is different from " + goldChp.GetFilename() + ". ";
    }
    std::string res = "different";
    if(success) 
        res = "equivalent";
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": checked " + ToStr(goldNum) + " genotype entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max confidence diff is: " + ToStr(maxDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max signal diff is: " + ToStr(maxSignalDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": chip is " + res + ".");
    return success;
  }

  bool multiDataDmetBiAllelicSame(CHPMultiDataData &goldChp, CHPMultiDataData &generatedChp, std::string &msgs, MultiDataType dataType) {
    bool success = true;
    double maxDiff = -1;
    double maxSignalDiff = -1;
    int numDiff = 0;
    int goldNum = goldChp.GetEntryCount(dataType);
    int genNum = generatedChp.GetEntryCount(dataType);
    assert(dataType == DmetBiAllelicMultiDataType);
    Verbose::out(4, ToStr(goldNum) + " " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + " probesets.");
    if(!checkMsg(goldNum == genNum, "Wrong number of genotyping probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(goldNum == 0)
      return true;
    for(int i = 0; i < goldNum; i++) {
      bool localSuccess = true;
      DmetBiAllelicData goldEntry, generatedEntry;
      goldChp.GetEntry(dataType, i, goldEntry);
      generatedChp.GetEntry(dataType, i, generatedEntry);
      checkMultiDataDmetBiAllelicEntry(goldEntry, generatedEntry, localSuccess, maxDiff, maxSignalDiff, numDiff);
      success &= localSuccess;
    }
    if(maxDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [confidence]");
    if(maxSignalDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxSignalDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [signal]");
    if(numDiff > 0)
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(goldNum) + " (" + 
                   ToStr(100.0 * numDiff/goldNum) + "%) were different.");
    if(!success && m_DiffAllowed >= numDiff) 
        success = true;
    if(!success) {
      msgs += "Error: " + generatedChp.GetFilename() + " is different from " + goldChp.GetFilename() + ". ";
    }
    std::string res = "different";
    if(success) 
        res = "equivalent";
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": checked " + ToStr(goldNum) + " genotype entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max confidence diff is: " + ToStr(maxDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max signal diff is: " + ToStr(maxSignalDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": chip is " + res + ".");
    return success;
  }

  /** 
   * Check to see if the copynumber entries for MultiDataType are the same.
   * @param goldChp - "correct" data.
   * @param generatedChp - chp file data to test against gold standard.
   * @param msgs - any ongoing messages for user.
   * @param dataType - Either genotype or control genotype, they are conceptually  separate in chp file.
   * @return - true if genotype entries are equivalent, false otherwise.
   */
  bool multiDataCopyNumberSame(CHPMultiDataData &goldChp, CHPMultiDataData &generatedChp, std::string &msgs, MultiDataType dataType) {
    bool success = true;
    double maxLog2RatioDiff = -1;
    double maxSmoothSignalDiff = -1;
    double maxAllelicDifferenceDiff = -1;
    int numDiff = 0;
    int numGenotyping = goldChp.GetEntryCount(dataType);
    int genNum = generatedChp.GetEntryCount(dataType);
    assert(dataType == CopyNumberMultiDataType);
    Verbose::out(4, ToStr(numGenotyping) + " " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + " copynumber probesets.");
    if(!checkMsg(numGenotyping == genNum, "Wrong number of copynumber probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(numGenotyping == 0)
      return true;
    for(int i = 0; i < numGenotyping; i++) {
      bool localSuccess = true;
      ProbeSetMultiDataCopyNumberData goldEntry, generatedEntry;
      goldChp.GetCopyNumberEntry(dataType, i, goldEntry);
      generatedChp.GetCopyNumberEntry(dataType, i, generatedEntry);
      checkMultiDataCopyNumberEntry(goldEntry, generatedEntry, localSuccess, maxLog2RatioDiff, maxSmoothSignalDiff, maxAllelicDifferenceDiff, numDiff);
      success &= localSuccess;
    }
    if(maxLog2RatioDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxLog2RatioDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [Log2Ratio]");
    if(maxSmoothSignalDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxSmoothSignalDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [SmoothSignal]");
    if(maxLog2RatioDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxAllelicDifferenceDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [AllelicDifference]");
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
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": checked " + ToStr(numGenotyping) + " copynumber entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max Log2Ratio diff is: " + ToStr(maxLog2RatioDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max SmoothSignal diff is: " + ToStr(maxSmoothSignalDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max AlleleicDifference diff is: " + ToStr(maxAllelicDifferenceDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": chip is " + res + ".");
    return success;
  }

  /** 
   * Check to see if the copynumber entries for MultiDataType are the same.
   * @param goldChp - "correct" data.
   * @param generatedChp - chp file data to test against gold standard.
   * @param msgs - any ongoing messages for user.
   * @param dataType - Either genotype or control genotype, they are conceptually  separate in chp file.
   * @return - true if genotype entries are equivalent, false otherwise.
   */
  bool multiDataCopyNumberVariationSame(CHPMultiDataData &goldChp, CHPMultiDataData &generatedChp, std::string &msgs, MultiDataType dataType) {
    bool success = true;
    int numDiff = 0;
    double maxConfidenceDiff = -1;
    double maxSignalDiff = -1;
    int goldNum = goldChp.GetEntryCount(dataType);
    int genNum = generatedChp.GetEntryCount(dataType);
    assert(dataType == CopyNumberVariationMultiDataType);
    Verbose::out(4, ToStr(goldNum) + " " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + " CNV probesets.");
    if(!checkMsg(goldNum == genNum, "Wrong number of CNV probesets.", msgs))
      return false;
    // if nothing to check, skip it.
    if(goldNum == 0)
      return true;
    for(int i = 0; i < goldNum; i++) {
      bool localSuccess = true;
      ProbeSetMultiDataCopyNumberVariationRegionData goldEntry, generatedEntry;
      goldChp.GetCopyNumberVariationEntry(dataType, i, goldEntry);
      generatedChp.GetCopyNumberVariationEntry(dataType, i, generatedEntry);
      checkMultiDataCopyNumberVariationEntry(goldEntry, generatedEntry, localSuccess, maxSignalDiff, maxConfidenceDiff, numDiff);
      success &= localSuccess;
    }
    if(maxSignalDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxSignalDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [signal]");
    if(maxConfidenceDiff > m_EpsConfidence)
      Verbose::out(1, "Max diff: " + ToStr(maxConfidenceDiff) + " is greater than expected (" + ToStr(m_EpsConfidence) + ") [confidence]");
    if(numDiff > 0)
      Verbose::out(1, ToStr(numDiff) + " of " + ToStr(goldNum) + " (" + 
                   ToStr(100.0 * numDiff/goldNum) + "%) were different.");
    if(!success && m_DiffAllowed >= numDiff) 
        success = true;
    if(!success) {
      msgs += "Error: " + generatedChp.GetFilename() + " is different from " + goldChp.GetFilename() + ". ";
    }
    std::string res = "different";
    if(success) 
        res = "equivalent";
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": checked " + ToStr(goldNum) + " CNV entries.");
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max Signal diff is: " + ToStr(maxSignalDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": max Confidence diff is: " + ToStr(maxConfidenceDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": " + StringUtils::ConvertWCSToMBS(MultiDataDataSetNames[dataType]) + ": chip is " + res + ".");
    return success;
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

  /** 
   * Check to see if the expression entries for Quantification  are the same.
   * @param goldChp - "correct" data.
   * @param generatedChp - chp file data to test against gold standard.
   * @param msgs - any ongoing messages for user.
   * @return - true if expression entries are equivalent, false otherwise.
   */
  bool quantificationDetectionDataSame(CHPQuantificationDetectionData &goldChp, CHPQuantificationDetectionData &generatedChp, std::string &msgs) {
    bool success = true;
    double maxDiff = -1;
    double pvalMaxDiff = -1;
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
      ProbeSetQuantificationDetectionData goldEntry, generatedEntry;
      goldChp.GetQuantificationDetectionEntry(i, goldEntry);
      generatedChp.GetQuantificationDetectionEntry(i, generatedEntry);
      checkQuantificationDetectionEntry(goldEntry, generatedEntry, localSuccess, maxDiff, numDiff, pvalMaxDiff);
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
    Verbose::out(1, generatedChp.GetFilename() + ": max pval diff is: " + ToStr(pvalMaxDiff));
    Verbose::out(1, generatedChp.GetFilename() + ": chip is " + res + ".");
    return success;
  }

  /// Filenames for the gold standard or correct data chp files.
  std::vector<std::string> m_Gold;
  /// Matching filenames for the chp files to be tested.
  std::vector<std::string> m_Generated;
  /// Epsilon, small value that two floats can differ by but still be considered equivalent.
  double m_EpsConfidence;
  double m_FracConfidence;
  double m_EpsQuantification;
  double m_FracQuantification;
  double m_EpsParam;
  double m_FracParam;
  bool m_CheckHeaders;
  /// How many differences will we tolerate?
  int m_DiffAllowed;
  /// What is the expected prefix for parameter names?
  std::wstring m_Prefix;
  /// Header entries to ignore
  std::set<std::wstring> m_IgnoreMap;
  
};

#endif /* CALVINCHPCHECK_H */
