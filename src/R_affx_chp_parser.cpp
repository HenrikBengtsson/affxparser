#include "FusionCHPData.h"
#include "FusionCHPLegacyData.h"
#include "FusionCHPQuantificationData.h"
#include "FusionCHPQuantificationDetectionData.h"
#include "FusionCHPDataAdapterInterface.h"
#include "FusionCHPTilingData.h" 
#include "StringUtils.h"
#include "ParameterNameValueType.h"
#include <string>

using namespace std;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_data;
using namespace affymetrix_calvin_io;
using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_parameter;

#include <Rdefines.h>

#define SET_NAMED_ELT(lst, index, val, nameLst, nameVal) \
  SET_ELEMENT(lst, index, val); \
  SET_STRING_ELT(nameLst, index, mkChar(nameVal))

const char *
wcs_to_cstr(std::wstring wstr)
{
#ifdef SUPPORT_MBCS
  return StringUtils::ConvertWCSToMBS(wstr).c_str();
#else
  return "\0";
#endif
}

int
R_affx_AddCHPMeta(AffymetrixGuidType fileId, 
		  wstring algName, wstring algVersion, 
		  wstring arrayType, 
		  SEXP lst, SEXP nms, int lstIdx)
{
  SET_NAMED_ELT(lst, lstIdx, mkString(fileId.c_str()), nms, "FileId");
  SET_NAMED_ELT(lst, lstIdx+1, mkString(wcs_to_cstr(algName)),
		nms, "AlgorithmName"); 
  SET_NAMED_ELT(lst, lstIdx+2, mkString(wcs_to_cstr(algVersion)),
		nms, "AlgorithmVersion");
  SET_NAMED_ELT(lst, lstIdx+3, mkString(wcs_to_cstr(arrayType)),
		nms, "ArrayType");
  return lstIdx+4;
}

SEXP 
R_affx_GetList(FusionTagValuePairTypeList& params)
{
  SEXP pLst, pNms, pVal;
  int pIdx = 0, pNbr = params.size();
  PROTECT(pLst = NEW_LIST(pNbr));
  PROTECT(pNms = NEW_CHARACTER(pNbr));
      
  for(FusionTagValuePairTypeList::iterator param=params.begin();
      param != params.end(); ++pIdx, ++param) {
    PROTECT(pVal = mkString(wcs_to_cstr(param->Value)));
    SET_NAMED_ELT(pLst, pIdx, pVal, pNms, 
		  wcs_to_cstr(param->Tag));
    UNPROTECT(1);
  }
  SET_NAMES(pLst, pNms);
  UNPROTECT(2);
  return pLst;
}

SEXP
R_affx_GetList(ParameterNameValueTypeList params)
// R_affx_ParamaterNameValueTypeList(ParameterNameValueTypeList params)
{
  SEXP pLst, pNms, pVal;
  int pIdx=0, pNbr = params.size();
  PROTECT(pLst = NEW_LIST(pNbr));
  PROTECT(pNms = NEW_CHARACTER(pNbr));
      
  for(ParameterNameValueTypeList::iterator param=params.begin();
      param != params.end(); ++pIdx, ++param) {
    const char *pName = wcs_to_cstr(param->GetName());
    switch(param->GetParameterType()) 
      {
      case ParameterNameValueType::Int8Type:
	PROTECT(pVal = ScalarInteger(param->GetValueInt8()));
	break;
      case ParameterNameValueType::Int16Type:
	PROTECT(pVal = ScalarInteger(param->GetValueInt16()));
	break;
      case ParameterNameValueType::Int32Type:
	PROTECT(pVal = ScalarInteger(param->GetValueInt32()));
	break;
      case ParameterNameValueType::UInt8Type:
      case ParameterNameValueType::UInt16Type:
      case ParameterNameValueType::UInt32Type:
	Rf_warning("reading 'unsigned int' parameter '%s' as NA", 
		   pName);
	PROTECT(pVal = ScalarInteger(R_NaInt));
	break;
      case ParameterNameValueType::FloatType:
	PROTECT(pVal = ScalarReal(param->GetValueFloat()));
	break;
      case ParameterNameValueType::TextType:
	PROTECT(pVal = mkString(wcs_to_cstr(param->GetValueText())));
	break;
      case ParameterNameValueType::AsciiType:
	PROTECT(pVal = mkString(param->GetValueAscii().c_str()));
	break;
      default:
	Rf_warning("unhandled type for parameter '%s'", pName);
	PROTECT(pVal = ScalarString(R_NaString));
      }
    SET_NAMED_ELT(pLst, pIdx, pVal, pNms, pName);
    UNPROTECT(1);
  }
  SET_NAMES(pLst, pNms);
  UNPROTECT(2);
  return pLst;
}

int
R_affx_AddList(FusionTagValuePairTypeList& list,
	       SEXP lst, SEXP nms, int lstIdx, const char *eltName)
{
  SEXP paramList;
  PROTECT(paramList = R_affx_GetList(list));
  SET_NAMED_ELT(lst, lstIdx, paramList, nms, eltName);
  UNPROTECT(1);
  return lstIdx+1;
}

int
R_affx_AddList(ParameterNameValueTypeList list,
	       SEXP lst, SEXP nms, int lstIdx, const char *eltName)
{
  SEXP paramList;
  PROTECT(paramList = R_affx_GetList(list));
  SET_NAMED_ELT(lst, lstIdx, paramList, nms, eltName);
  UNPROTECT(1);
  return lstIdx+1;
}	       

SEXP
R_affx_GetCHPExpressionResults(FusionCHPLegacyData *chp)
{
  SEXP 
    detectionPValue, signal, numPairs, numUsedPairs, detection, 
    hasCompResults, changePValue, signalLogRatio, signalLogRatioLow, 
    signalLogRatioHigh, numCommonPairs, change;
  int qNbr=chp->GetHeader().GetNumProbeSets();

  PROTECT(detectionPValue = NEW_NUMERIC(qNbr));
  PROTECT(signal = NEW_NUMERIC(qNbr));
  PROTECT(numPairs = NEW_INTEGER(qNbr));
  PROTECT(numUsedPairs = NEW_INTEGER(qNbr));
  PROTECT(detection = NEW_INTEGER(qNbr));
  PROTECT(hasCompResults = NEW_LOGICAL(qNbr));
  PROTECT(changePValue = NEW_NUMERIC(qNbr));
  PROTECT(signalLogRatio = NEW_NUMERIC(qNbr));
  PROTECT(signalLogRatioLow = NEW_NUMERIC(qNbr));
  PROTECT(signalLogRatioHigh = NEW_NUMERIC(qNbr));
  PROTECT(numCommonPairs = NEW_INTEGER(qNbr));
  PROTECT(change = NEW_INTEGER(qNbr));

  // FIXME: probe set names need to come from PSI file
  // FIXME: very inefficient -- function calls for each assignment
  FusionExpressionProbeSetResults psResults;
  for (int qIdx=0; qIdx<qNbr; ++qIdx) {
    chp->GetExpressionResults(qIdx, psResults);
    REAL(detectionPValue)[qIdx] = psResults.GetDetectionPValue();
    REAL(signal)[qIdx] = psResults.GetSignal();
    INTEGER(numPairs)[qIdx] = psResults.GetNumPairs();
    INTEGER(numUsedPairs)[qIdx] = psResults.GetNumUsedPairs();
    INTEGER(detection)[qIdx] = psResults.GetDetection();
    LOGICAL(hasCompResults)[qIdx] =
      (psResults.HasCompResults() ? TRUE : FALSE);
    REAL(changePValue)[qIdx] = psResults.GetChangePValue();
    REAL(signalLogRatio)[qIdx] = psResults.GetSignalLogRatio();
    REAL(signalLogRatioLow)[qIdx] = psResults.GetSignalLogRatioLow();
    REAL(signalLogRatioHigh)[qIdx] = psResults.GetSignalLogRatioHigh();
    INTEGER(numCommonPairs)[qIdx] = psResults.GetNumCommonPairs();
    INTEGER(change)[qIdx] = psResults.GetChange();
  }

  SEXP result;
  PROTECT(result = NEW_LIST(12));
  SET_ELEMENT(result, 0, detectionPValue);
  SET_ELEMENT(result, 1, signal);
  SET_ELEMENT(result, 2, numPairs);
  SET_ELEMENT(result, 3, numUsedPairs);
  SET_ELEMENT(result, 4, detection);
  SET_ELEMENT(result, 5, hasCompResults);
  SET_ELEMENT(result, 6, changePValue);
  SET_ELEMENT(result, 7, signalLogRatio);
  SET_ELEMENT(result, 8, signalLogRatioLow);
  SET_ELEMENT(result, 9, signalLogRatioHigh);
  SET_ELEMENT(result, 10, numCommonPairs);
  SET_ELEMENT(result, 11, change);
  SEXP nms;
  PROTECT(nms = NEW_CHARACTER(12));
  SET_ELEMENT(nms, 0, mkChar("DetectionPValue"));
  SET_ELEMENT(nms, 1, mkChar("Signal"));
  SET_ELEMENT(nms, 2, mkChar("NumPairs"));
  SET_ELEMENT(nms, 3, mkChar("NumUsedPairs"));
  SET_ELEMENT(nms, 4, mkChar("Detection"));
  SET_ELEMENT(nms, 5, mkChar("HasCompResults"));
  SET_ELEMENT(nms, 6, mkChar("ChangePValue"));
  SET_ELEMENT(nms, 7, mkChar("SignalLogRatio"));
  SET_ELEMENT(nms, 8, mkChar("SignalLogRatioLow"));
  SET_ELEMENT(nms, 9, mkChar("SignalLogRatioHigh"));
  SET_ELEMENT(nms, 10, mkChar("NumCommonPairs"));
  SET_ELEMENT(nms, 11, mkChar("Change"));

  SET_NAMES(result, nms);
  UNPROTECT(14);
  return result;
}

SEXP
R_affx_GetCHPEntries(FusionCHPQuantificationData *qData)
{
  SEXP qVec, qNm, qId;
  int qNbr = qData->GetEntryCount();

  PROTECT(qVec = NEW_NUMERIC(qNbr));
  PROTECT(qNm = NEW_CHARACTER(qNbr));
  PROTECT(qId = NEW_INTEGER(qNbr));
    
  ProbeSetQuantificationData psData;
  for (int qIdx=0; qIdx < qNbr; ++qIdx) {
    qData->GetQuantificationEntry(qIdx, psData);
    SET_STRING_ELT(qNm, qIdx, mkChar(psData.name.c_str()));
    INTEGER(qId)[qIdx] = psData.id;
    REAL(qVec)[qIdx] = psData.quantification;
  }    
    
  SEXP result;
  PROTECT(result = NEW_LIST(2));
  SET_ELEMENT(result, 0, qNm);
  SET_ELEMENT(result, 1, qVec);
  SEXP nms;
  PROTECT(nms = NEW_CHARACTER(2));
  SET_ELEMENT(nms, 0, mkChar("ProbeSetName"));
  SET_ELEMENT(nms, 1, mkChar("QuantificationValue"));
  SET_NAMES(result, nms);

  UNPROTECT(5);
  return result;
}

SEXP
R_affx_GetCHPEntries(FusionCHPQuantificationDetectionData *qData)
{
  // FIXME: 'id' attribute?
  SEXP qVec, pValueVec, qNm;
  int qNbr = qData->GetEntryCount();

  PROTECT(qVec = NEW_NUMERIC(qNbr));
  PROTECT(pValueVec = NEW_NUMERIC(qNbr));
  PROTECT(qNm = NEW_CHARACTER(qNbr));
    
  double *q = NUMERIC_POINTER(qVec);;
  double *pValue = NUMERIC_POINTER(pValueVec);
  ProbeSetQuantificationDetectionData psData;
  for (int qIdx=0; qIdx < qNbr; ++qIdx) {
    qData->GetQuantificationDetectionEntry(qIdx, psData);
    q[qIdx] = psData.quantification;
    pValue[qIdx] = psData.pvalue;
    SET_STRING_ELT(qNm, qIdx, mkChar(psData.name.c_str()));
  }    
    
  SEXP result;
  PROTECT(result = NEW_LIST(3));
  SET_ELEMENT(result, 0, qNm);
  SET_ELEMENT(result, 1, qVec);
  SET_ELEMENT(result, 2, pValueVec);
  SEXP nms;
  PROTECT(nms = NEW_CHARACTER(3));
  SET_STRING_ELT(nms, 0, mkChar("ProbeSetName"));
  SET_STRING_ELT(nms, 1, mkChar("QuantificationValue"));
  SET_STRING_ELT(nms, 2, mkChar("PValue"));
  SET_NAMES(result, nms);

  UNPROTECT(5);
  return result;
}

int
R_affx_AddCHPEntries(FusionCHPQuantificationData *chp,
		     SEXP lst, SEXP nms, int lstIdx, bool isBrief)
{
  if (isBrief == false) {
    SEXP quantEntries = PROTECT(R_affx_GetCHPEntries(chp));
    SET_ELEMENT(lst, lstIdx, quantEntries);
    UNPROTECT(1);
  }
  SET_STRING_ELT(nms, lstIdx, mkChar("QuantificationEntries"));
  return lstIdx + 1;
}

int
R_affx_AddCHPEntries(FusionCHPQuantificationDetectionData *chp,
		     SEXP lst, SEXP nms, int lstIdx, bool isBrief)
{
  if (isBrief == false) {
    SEXP quantEntries = PROTECT(R_affx_GetCHPEntries(chp));
    SET_ELEMENT(lst, lstIdx, quantEntries);
    UNPROTECT(1);
  }
  SET_STRING_ELT(nms, lstIdx, mkChar("QuantificationEntries"));
  return lstIdx + 1;
}

SEXP
R_affx_ReadCHP(FusionCHPLegacyData *chp, bool isBrief)
{
  SEXP lst, nms;
  int lstIdx = 0, lstNbr = 7;
  PROTECT(lst = NEW_LIST(lstNbr));
  PROTECT(nms = NEW_CHARACTER(lstNbr));

  FusionCHPHeader header = chp->GetHeader();

  lstIdx = R_affx_AddCHPMeta(chp->FileId(), header.GetAlgName(), 
			     header.GetAlgVersion(), header.GetChipType(),
			     lst, nms, lstIdx);

  FusionTagValuePairTypeList params;
  header.AlgorithmParameters(params);
  lstIdx = R_affx_AddList(params, lst, nms, lstIdx, "AlgorithmParameters");
  header.SummaryParameters(params);
  lstIdx = R_affx_AddList(params, lst, nms, lstIdx, "SummaryParameters");

  if (isBrief == false) {
    SEXP quantEntries;
    switch(header.GetAssayType())
      {
      case FusionExpression:
	PROTECT(quantEntries = R_affx_GetCHPExpressionResults(chp));
	break;
      case FusionGenotyping:
      case FusionResequencing:
      case FusionUniversal:
      case FusionUnknown:
      default:
	Rf_warning("unhandled quantification entry index '%d'", 
		   header.GetAssayType());
	PROTECT(quantEntries = NEW_NUMERIC(0));
      }
    SET_NAMED_ELT(lst, lstIdx, quantEntries, nms, "QuantificationEntries");
    ++lstIdx;
    UNPROTECT(1);
  }

  SET_NAMES(lst, nms);
  UNPROTECT(2);
  return(lst);
}

SEXP
R_affx_ReadCHP(FusionCHPTilingData *chp, bool isBrief)
{
  SEXP lst, nms;
  int lstIdx = 0, lstNbr = 3;

  PROTECT(lst = NEW_LIST(lstNbr));
  PROTECT(nms = NEW_CHARACTER(lstNbr));
  
  lstIdx = R_affx_AddList(chp->GetAlgParams(), lst, nms, lstIdx,
			   "AlgorithmParameters");
  SET_NAMES(lst, nms);
  UNPROTECT(2);
  return(lst);
}
	       
SEXP
R_affx_ReadCHP(FusionCHPQuantificationData *chp, bool isBrief)
{
  SEXP lst, nms;
  int lstIdx = 0, lstNbr = 7;
  PROTECT(lst = NEW_LIST(lstNbr));
  PROTECT(nms = NEW_CHARACTER(lstNbr));

  lstIdx = R_affx_AddCHPMeta(chp->FileId(), chp->GetAlgName(),
			     chp->GetAlgVersion(), chp->GetArrayType(),
			     lst, nms, lstIdx);
  lstIdx = R_affx_AddList(chp->GetAlgParams(), lst, nms, lstIdx,
			  "AlgorithmParameters");
  lstIdx = R_affx_AddList(chp->GetSummaryParams(), lst, nms, lstIdx,
			  "SummaryParameters");
  lstIdx = R_affx_AddCHPEntries(chp, lst, nms, lstIdx, isBrief);

  SET_NAMES(lst, nms);
  UNPROTECT(2);
  return(lst);
}

SEXP
R_affx_ReadCHP(FusionCHPQuantificationDetectionData *chp, bool isBrief)
{
  SEXP lst, nms;
  int lstIdx = 0, lstNbr = 7;
  PROTECT(lst = NEW_LIST(lstNbr));
  PROTECT(nms = NEW_CHARACTER(lstNbr));

  lstIdx = R_affx_AddCHPMeta(chp->FileId(), chp->GetAlgName(),
			     chp->GetAlgVersion(), chp->GetArrayType(),
			     lst, nms, lstIdx);
  lstIdx = R_affx_AddList(chp->GetAlgParams(), lst, nms, lstIdx,
			  "AlgorithmParameters");
  lstIdx = R_affx_AddList(chp->GetSummaryParams(), lst, nms, lstIdx,
			  "SummaryParameters");
  lstIdx = R_affx_AddCHPEntries(chp, lst, nms, lstIdx, isBrief);
    
  SET_NAMES(lst, nms);
  UNPROTECT(2);
  return(lst);
}

extern "C" {

  SEXP 
  R_affx_get_chp_file(SEXP fname, SEXP withQuantifications)
  {
    if (IS_CHARACTER(fname) == FALSE || LENGTH(fname) != 1)
      Rf_error("argument '%s' should be '%s'", "fname",
	       "character(1)");
    if (IS_LOGICAL(withQuantifications) == FALSE || 
	LENGTH(withQuantifications) != 1)
      Rf_error("argument '%s' should be '%s'", 
	       "withQuantifications", "logical(1)");

    const char *chpFileName = CHAR(STRING_ELT(fname, 0));
    bool
      isBrief = (LOGICAL(withQuantifications)[0] == TRUE ? false : true),
      processed = false;
    int protectionCount = 0;
    SEXP result = R_NilValue;

    FusionCHPData *chp = FusionCHPDataReg::Read(chpFileName);
    if (chp == NULL)
      Rf_error("could not read '%s'", chpFileName);
  
    if (processed == false) {
      FusionCHPLegacyData *lChp = FusionCHPLegacyData::FromBase(chp);
      if (lChp != NULL) {
	processed = true;
	PROTECT(result = R_affx_ReadCHP(lChp, isBrief));
	++protectionCount;
	delete lChp;
      }
    }

    if (processed==false) {
      FusionCHPQuantificationData 
	*qChp = FusionCHPQuantificationData::FromBase(chp);
      if (qChp != NULL) {
	processed = true;
	PROTECT(result = R_affx_ReadCHP(qChp, isBrief));
	++protectionCount;
	delete qChp;
      }
    }

    if (processed==false) {
      FusionCHPQuantificationDetectionData 
	*qdChp = FusionCHPQuantificationDetectionData::FromBase(chp);
      if (qdChp != NULL) {
	processed = true;
	PROTECT(result = R_affx_ReadCHP(qdChp, isBrief));
	++protectionCount;
	delete qdChp;
      }
    }

    if (processed==false) {
      FusionCHPTilingData *tChp = FusionCHPTilingData::FromBase(chp);
      if(tChp != NULL) {
        processed = true;
        PROTECT(result = R_affx_ReadCHP(tChp, isBrief));
        ++protectionCount;
        delete tChp;
      }
     }

    if (processed==false) {
      Rf_warning("unable to read CHP file '%s'", chpFileName);
      delete chp;
    }

    UNPROTECT(protectionCount);
    return result;
  }
}
