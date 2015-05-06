#include "FusionCHPData.h"
#include "FusionCHPLegacyData.h"
#include "FusionCHPMultiDataData.h"
#include "FusionCHPQuantificationData.h"
#include "FusionCHPQuantificationDetectionData.h"
#include "FusionCHPDataAdapterInterface.h"
#include "FusionCHPTilingData.h" 
#include "CHPReseqEntry.h"
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

// const char *
// wcs_to_cstr(std::wstring wstr)
// {
// #ifdef SUPPORT_MBCS
//   return StringUtils::ConvertWCSToMBS(wstr).c_str();
// #else
//   return "\0";
// #endif
// }

char *
wcs_to_cstr(std::wstring wstr)
{
#ifdef SUPPORT_MBCS
  string str;
  int str_length; 
  char* cstr; 
  str_length = wstr.size();
  cstr = Calloc(str_length+1, char);
  wcstombs(cstr, wstr.c_str(), str_length);
  cstr[str_length] = '\0';
  return cstr;
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
  char* cstr;
  SET_NAMED_ELT(lst, lstIdx, mkString(fileId.c_str()), nms, "FileId");
  SET_NAMED_ELT(lst, lstIdx+1, mkString(cstr = wcs_to_cstr(algName)),
		nms, "AlgorithmName");
  Free(cstr);
  SET_NAMED_ELT(lst, lstIdx+2, mkString(cstr = wcs_to_cstr(algVersion)),
		nms, "AlgorithmVersion");
  Free(cstr);
  SET_NAMED_ELT(lst, lstIdx+3, mkString(cstr = wcs_to_cstr(arrayType)),
		nms, "ArrayType");
  Free(cstr);
  return lstIdx+4;
}

int
R_affx_AddCHPTileMeta(AffymetrixGuidType fileId, 
		  wstring algName, wstring algVersion, 
		  SEXP lst, SEXP nms, int lstIdx)
{
  char* cstr;
  SET_NAMED_ELT(lst, lstIdx, mkString(fileId.c_str()), nms, "FileId");
  SET_NAMED_ELT(lst, lstIdx+1, mkString(cstr = wcs_to_cstr(algName)),
		nms, "AlgorithmName"); 
  Free(cstr);
  SET_NAMED_ELT(lst, lstIdx+2, mkString(cstr = wcs_to_cstr(algVersion)),
		nms, "AlgorithmVersion");
  Free(cstr);
  return lstIdx+3;
}

SEXP 
R_affx_GetList(FusionTagValuePairTypeList& params)
{
  SEXP pLst, pNms, pVal;
  int pIdx = 0, pNbr = params.size();
  char* cstr;
  PROTECT(pLst = NEW_LIST(pNbr));
  PROTECT(pNms = NEW_CHARACTER(pNbr));
      
  for(FusionTagValuePairTypeList::iterator param=params.begin();
      param != params.end(); ++pIdx, ++param) {
    PROTECT(pVal = mkString(cstr = wcs_to_cstr(param->Value)));
    Free(cstr);
    SET_NAMED_ELT(pLst, pIdx, pVal, pNms, 
		  cstr = wcs_to_cstr(param->Tag));
    Free(cstr);
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
  SEXP pLst, pNms, pVal, pName;
  int pIdx=0, pNbr = params.size();
  char* cstr;
  PROTECT(pLst = NEW_LIST(pNbr));
  PROTECT(pNms = NEW_CHARACTER(pNbr));
  
  /* no idea why one must use mkString to hold pName and not a char*
     but the former works and the latter does not */    
  for(ParameterNameValueTypeList::iterator param=params.begin();
      param != params.end(); ++pIdx, ++param) {
    PROTECT(pName = mkString(cstr = wcs_to_cstr(param->GetName())));
    Free(cstr);
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
	warning("reading 'unsigned int' parameter '%s' as NA", 
		   pName);
	PROTECT(pVal = ScalarInteger(R_NaInt));
	break;
      case ParameterNameValueType::FloatType:
	PROTECT(pVal = ScalarReal(param->GetValueFloat()));
	break;
      case ParameterNameValueType::TextType:
	PROTECT(pVal = mkString(cstr = wcs_to_cstr(param->GetValueText())));
        Free(cstr);
	break;
      case ParameterNameValueType::AsciiType:
	PROTECT(pVal = mkString(param->GetValueAscii().c_str()));
	break;
      default:
	warning("unhandled type for parameter '%s'", CHAR(pName));
	PROTECT(pVal = ScalarString(R_NaString));
      }
    SET_NAMED_ELT(pLst, pIdx, pVal, pNms, CHAR(STRING_ELT(pName,0)));

    UNPROTECT(2);
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
R_affx_GetCHPReseqResults(FusionCHPLegacyData *chp)
{
  SEXP call, score, rval, nms, fcall, freason, fposition, fnms, force,
    orig, onames, ocall;
  int ct, i, nprotect=0;
  char *s, *sc, *sr;
  FusionResequencingResults  frResults;
  
  chp->GetReseqResults(frResults);

  ct = frResults.GetCalledBasesSize();
  s = (char *) R_alloc((long) (ct) + 1L, sizeof(char));
  for(i=0; i<ct; i++) 
    s[i] = (char) frResults.GetCalledBase(i);
  s[ct] = '\0';

  PROTECT( call = mkString(s) );
  nprotect++;

  //scores
  ct = frResults.GetScoresSize();
  PROTECT(score = NEW_NUMERIC(ct));
  nprotect++;

  for(i=0; i<ct; i++) 
    REAL(score)[i] = frResults.GetScore(i);

  //force calls: call, position, reason
  ct = frResults.GetForceCallsSize();
  if( ct > 0 ) {
    PROTECT(force = NEW_LIST(3));
    PROTECT(fposition = NEW_INTEGER(ct));
    nprotect+=2;
    sc = (char *) R_alloc((long) (ct) + 1L, sizeof(char));
    sr = (char *) R_alloc((long) (ct) + 1L, sizeof(char));
    FusionForceCallType ffct;
    for(i=0; i<ct; i++) {
      ffct = frResults.GetForceCall(i);
      INTEGER(fposition)[i] = ffct.GetPosition();
      sc[i] = (char) ffct.GetCall();
      sr[i] = (char) ffct.GetReason();
    }
    sc[ct] = sr[ct] = '\0';
    PROTECT(fcall = mkString(sc));
    PROTECT(freason = mkString(sr));
    nprotect+=2;
    SET_ELEMENT(force, 0, fposition);
    SET_ELEMENT(force, 1, fcall);
    SET_ELEMENT(force, 2, freason);
    PROTECT(fnms = NEW_CHARACTER(3));
    nprotect++;
    SET_STRING_ELT(fnms, 0, mkChar("position"));
    SET_STRING_ELT(fnms, 1, mkChar("call"));
    SET_STRING_ELT(fnms, 2, mkChar("reason"));
    SET_NAMES(force, fnms);
  }  else
    force = R_NilValue;

  ct = frResults.GetOrigCallsSize();
  if( ct > 0 ) {
    PROTECT(fposition = NEW_INTEGER(ct));
    nprotect++;
    sc = (char *) R_alloc((long) (ct) + 1L, sizeof(char));
    FusionBaseCallType fbct;
    for(i=0; i<ct; i++) {
      fbct = frResults.GetOrigCall(i);
      INTEGER(fposition)[i] = fbct.GetPosition();
      sc[i] = (char) fbct.GetCall();
    }
    sc[ct] = '\0';
    PROTECT(ocall = mkString(sc));
    PROTECT(orig = NEW_LIST(2));
    nprotect+=2;
    SET_ELEMENT(orig, 0, ocall);
    SET_ELEMENT(orig, 1, fposition);
    PROTECT(onames = NEW_CHARACTER(2));
    nprotect++;
    SET_STRING_ELT(onames, 0, mkChar("call"));
    SET_STRING_ELT(onames, 1, mkChar("position"));
    SET_NAMES(orig, onames);
  } else orig = R_NilValue;

  PROTECT(rval = NEW_LIST(3));
  nprotect++;
  SET_ELEMENT(rval, 0, call);
  SET_ELEMENT(rval, 1, force);
  SET_ELEMENT(rval, 2, orig);

  PROTECT(nms = NEW_CHARACTER(3));
  nprotect++;
  SET_STRING_ELT(nms, 0, mkChar("call"));
  SET_STRING_ELT(nms, 1, mkChar("force"));
  SET_STRING_ELT(nms, 2, mkChar("orig"));

  SET_NAMES(rval, nms);

  UNPROTECT(nprotect);
  return rval;
}

//FIXME: cannot find any coherent description of these arrays
SEXP
R_affx_GetCHPUniversalResults(FusionCHPLegacyData *chp)
{
  SEXP ans;
  CHPUniversalEntry Uentry;
  FusionUniversalProbeSetResults rFU;
  int count, i;

  count = chp->GetHeader().GetNumProbeSets();
  PROTECT(ans = NEW_NUMERIC(count));

  for(i = 0; i < count; ++i) {
    chp->GetUniversalResults(i, rFU);
    REAL(ans)[i] = rFU.GetBackground();
  }

  UNPROTECT(1);
  return ans;
}

SEXP
R_affx_GetCHPGenotypingResults(FusionCHPLegacyData *chp)
{
  SEXP rval, ras1, ras2, aa, ab, bb, nocall, call, conf, callstr, alg;
  int qNbr = chp->GetHeader().GetNumProbeSets(), i, nprotect=0, nelt;
  bool bWholeGenome = false, bDynamicModel = false;
  char* cstr;

  PROTECT(call = NEW_INTEGER(qNbr));
  PROTECT(conf = NEW_NUMERIC(qNbr));
  PROTECT(callstr = NEW_CHARACTER(qNbr));
  nprotect = 3;

  //FIXME: I did not think AlgName could be "", it is, so we are stuck
  //with that
  PROTECT(alg = mkString(cstr = wcs_to_cstr(chp->GetHeader().GetAlgName())));
  Free(cstr);
  nprotect++;

  if(chp->GetHeader().GetAlgName() == L"WholeGenome") {
    bWholeGenome = true;
    PROTECT(ras1 = NEW_NUMERIC(qNbr));
    PROTECT(ras2 = NEW_NUMERIC(qNbr));
    nprotect+=2;
    Rprintf("dudey");
  }
  else if(chp->GetHeader().GetAlgName() == L"DynamicModel") {
    bDynamicModel = true;
    PROTECT(aa = NEW_NUMERIC(qNbr));
    PROTECT(ab = NEW_NUMERIC(qNbr));
    PROTECT(bb = NEW_NUMERIC(qNbr));
    PROTECT(nocall = NEW_NUMERIC(qNbr));
    nprotect+=4;
    Rprintf("howdy");
  }


  FusionGenotypeProbeSetResults f;
  for(i=0; i<qNbr; i++) {
    chp->GetGenotypingResults(i, f);
    INTEGER(call)[i] = f.GetAlleleCall();
    SET_STRING_ELT(callstr, i, mkChar(f.GetAlleleCallString().c_str()));
    REAL(conf)[i] = f.GetConfidence();
    if( bWholeGenome ) {
      REAL(ras1)[i] =  f.GetRAS1();
      REAL(ras2)[i] =  f.GetRAS2();
    }
    if( bDynamicModel ) {
      REAL(aa)[i] = f.GetPValueAA();
      REAL(ab)[i] = f.GetPValueAB();
      REAL(bb)[i] = f.GetPValueBB();
      REAL(nocall)[i] = f.GetPValueNoCall();
    }
  }


  if( bWholeGenome ) nelt = 6; 
  else if(bDynamicModel) nelt = 8;
  else nelt = 4;

  PROTECT(rval = NEW_LIST(nelt));
  nprotect++;
  SET_ELEMENT(rval, 0, call);
  SET_ELEMENT(rval, 1, conf);
  SET_ELEMENT(rval, 2, callstr);
  if( bWholeGenome ) {
    SET_ELEMENT(rval, 3, mkString("WholeGenome"));
    SET_ELEMENT(rval, 4, ras1);
    SET_ELEMENT(rval, 5, ras1);
  } else if( bDynamicModel ) {
    SET_ELEMENT(rval, 3, mkString("DynamicModel"));
    SET_ELEMENT(rval, 4, aa);
    SET_ELEMENT(rval, 5, ab);
    SET_ELEMENT(rval, 6, bb);
    SET_ELEMENT(rval, 7, nocall);
  } else {
    SET_ELEMENT(rval, 3, mkString("None"));
  }

  SEXP nms;
  PROTECT(nms = NEW_CHARACTER(nelt));
  nprotect++;
  SET_STRING_ELT(nms, 0, mkChar("Call"));
  SET_STRING_ELT(nms, 1, mkChar("Confidence"));
  SET_STRING_ELT(nms, 2, mkChar("AlleleString"));
  SET_STRING_ELT(nms, 3, mkChar("AlgName"));
  if( bWholeGenome ) {
    SET_STRING_ELT(nms, 4, mkChar("RAS1"));
    SET_STRING_ELT(nms, 5, mkChar("RAS2"));
  }
  else if (bDynamicModel) {
    SET_STRING_ELT(nms, 4, mkChar("PvalueAA"));
    SET_STRING_ELT(nms, 5, mkChar("PvalueAB"));
    SET_STRING_ELT(nms, 6, mkChar("PvalueBB"));
    SET_STRING_ELT(nms, 7, mkChar("PvalueNoCall"));
  }
  SET_NAMES(rval, nms);
  
  UNPROTECT(nprotect);
  return rval;
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
  SET_STRING_ELT(nms, 0, mkChar("DetectionPValue"));
  SET_STRING_ELT(nms, 1, mkChar("Signal"));
  SET_STRING_ELT(nms, 2, mkChar("NumPairs"));
  SET_STRING_ELT(nms, 3, mkChar("NumUsedPairs"));
  SET_STRING_ELT(nms, 4, mkChar("Detection"));
  SET_STRING_ELT(nms, 5, mkChar("HasCompResults"));
  SET_STRING_ELT(nms, 6, mkChar("ChangePValue"));
  SET_STRING_ELT(nms, 7, mkChar("SignalLogRatio"));
  SET_STRING_ELT(nms, 8, mkChar("SignalLogRatioLow"));
  SET_STRING_ELT(nms, 9, mkChar("SignalLogRatioHigh"));
  SET_STRING_ELT(nms, 10, mkChar("NumCommonPairs"));
  SET_STRING_ELT(nms, 11, mkChar("Change"));

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
  SET_STRING_ELT(nms, 0, mkChar("ProbeSetName"));
  SET_STRING_ELT(nms, 1, mkChar("QuantificationValue"));
  SET_NAMES(result, nms);

  UNPROTECT(5);
  return result;
}

SEXP
R_affx_GetCHPEntries(FusionCHPQuantificationDetectionData *qData)
{

  SEXP qVec, pValueVec, qNm, qID;
  int qNbr = qData->GetEntryCount();

  PROTECT(qVec = NEW_NUMERIC(qNbr));
  PROTECT(pValueVec = NEW_NUMERIC(qNbr));
  PROTECT(qNm = NEW_CHARACTER(qNbr));
  PROTECT(qID = NEW_INTEGER(qNbr));
    
  double *q = NUMERIC_POINTER(qVec);;
  double *pValue = NUMERIC_POINTER(pValueVec);
  ProbeSetQuantificationDetectionData psData;
  for (int qIdx=0; qIdx < qNbr; ++qIdx) {
    qData->GetQuantificationDetectionEntry(qIdx, psData);
    q[qIdx] = psData.quantification;
    pValue[qIdx] = psData.pvalue;
    INTEGER(qID)[qIdx] = psData.id;
    SET_STRING_ELT(qNm, qIdx, mkChar(psData.name.c_str()));
  }
    
  SEXP result;
  PROTECT(result = NEW_LIST(4));
  SET_ELEMENT(result, 0, qNm);
  SET_ELEMENT(result, 1, qVec);
  SET_ELEMENT(result, 2, pValueVec);
  SET_ELEMENT(result, 3, qID);
  SEXP nms;
  PROTECT(nms = NEW_CHARACTER(4));
  SET_STRING_ELT(nms, 0, mkChar("ProbeSetName"));
  SET_STRING_ELT(nms, 1, mkChar("QuantificationValue"));
  SET_STRING_ELT(nms, 2, mkChar("PValue"));
  SET_STRING_ELT(nms, 3, mkChar("ID"));
  SET_NAMES(result, nms);

  UNPROTECT(6);
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
      case FusionResequencing:
	PROTECT(quantEntries = R_affx_GetCHPReseqResults(chp));
	break;
      case FusionGenotyping:
	PROTECT(quantEntries = R_affx_GetCHPGenotypingResults(chp));
	break;
      case FusionUniversal:
	PROTECT(quantEntries = R_affx_GetCHPUniversalResults(chp));
	break;
      case FusionUnknown:
      default:
	warning("unhandled quantification entry index '%d'", 
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

SEXP R_affx_ReadTilingDataSeqHeader(TilingSequenceData seq)
{
  SEXP header, hnames;
  char* cstr;
  
  //read in the header/params
  PROTECT(header = NEW_LIST(4));
  PROTECT(hnames = NEW_CHARACTER(4));
  SET_NAMED_ELT(header, 0, mkString(cstr = wcs_to_cstr(seq.name)), hnames, "name");
  Free(cstr);
  SET_NAMED_ELT(header, 1, mkString(cstr = wcs_to_cstr(seq.groupName)), hnames,
		"groupName"); 
  Free(cstr);
  SET_NAMED_ELT(header, 2, mkString(cstr = wcs_to_cstr(seq.version)), hnames,
		"version");
  Free(cstr);
  SET_NAMED_ELT(header, 3,
		R_affx_GetList(seq.parameters),
		hnames, "parameters");
  SET_NAMES(header, hnames);
  UNPROTECT(2);
  return header;
}

SEXP R_affx_ReadTilingDataSeqEntries(FusionCHPTilingData *chp, int Entry) 
{
  int eCount, i;
  SEXP rval, position, value, nms;
  CHPTilingEntry e;

  eCount = chp->GetTilingSequenceEntryCount(Entry);
  PROTECT(position = NEW_INTEGER(eCount));
  PROTECT(value = NEW_NUMERIC(eCount));
 
  for(i=0; i<eCount; i++) {
    chp->GetTilingSequenceEntry(i, e);
    INTEGER(position)[i] = e.position;
    REAL(value)[i] =e.value;
  }

  PROTECT(rval = NEW_LIST(2));
  SET_ELEMENT(rval, 0, position);
  SET_ELEMENT(rval, 1, value);

  PROTECT(nms = NEW_CHARACTER(2));
  SET_STRING_ELT(nms, 0, mkChar("position"));
  SET_STRING_ELT(nms, 1, mkChar("value"));
  SET_NAMES(rval, nms);
  UNPROTECT(4);
  return rval;
}

SEXP
R_affx_ReadCHP(FusionCHPTilingData *chp, bool isBrief)
{
  SEXP lst, nms, seqList, seqi, seqiNms, tmp;
  int lstIdx = 0, lstNbr = 6, numSeq=0, i;

  PROTECT(lst = NEW_LIST(lstNbr));
  PROTECT(nms = NEW_CHARACTER(lstNbr));

  lstIdx = R_affx_AddCHPTileMeta(chp->FileId(), chp->GetAlgName(),
			     chp->GetAlgVersion(),
			     lst, nms, lstIdx);  
  SET_NAMED_ELT(lst, lstIdx, R_affx_GetList(chp->GetAlgParams()), nms,
					    "AlgorithmParameters");
  lstIdx++;

  numSeq = chp->GetNumberSequences();
  PROTECT(tmp = NEW_INTEGER(1));
  INTEGER(tmp)[0] = numSeq;
  SET_NAMED_ELT(lst, lstIdx, tmp, nms, "NumberofSequences");
  lstIdx++;
  UNPROTECT(1);

  PROTECT(seqList = NEW_LIST(numSeq));
  for(i=0; i<numSeq; i++) {
    chp->OpenTilingSequenceDataSet(i);
    PROTECT(seqi = NEW_LIST(2));
    SET_ELEMENT(seqi, 0,
		R_affx_ReadTilingDataSeqHeader(chp->GetTilingSequenceData())); 
    SET_ELEMENT(seqi, 1, R_affx_ReadTilingDataSeqEntries(chp, i));
    PROTECT(seqiNms = NEW_CHARACTER(2));
    SET_STRING_ELT(seqiNms, 0, mkChar("seq"));
    SET_STRING_ELT(seqiNms, 1, mkChar("entries"));
    SET_NAMES(seqi, seqiNms);
    SET_ELEMENT(seqList, i, seqi);
    UNPROTECT(2);
  }

  SET_NAMED_ELT(lst, lstIdx, seqList, nms, "Sequences");
  lstIdx++;
  SET_NAMES(lst, nms);
  UNPROTECT(3);

  return(lst);
}

SEXP
R_affx_ReadCHP(FusionCHPQuantificationData *chp, bool isBrief)
{
  SEXP lst, nms, nQ;
  int lstIdx = 0, lstNbr = 8, qNbr;
  PROTECT(lst = NEW_LIST(lstNbr));
  PROTECT(nms = NEW_CHARACTER(lstNbr));

  lstIdx = R_affx_AddCHPMeta(chp->FileId(), chp->GetAlgName(),
			     chp->GetAlgVersion(), chp->GetArrayType(),
			     lst, nms, lstIdx);
  SET_NAMED_ELT(lst, lstIdx, R_affx_GetList(chp->GetAlgParams()), nms,
			  "AlgorithmParameters");
  lstIdx++;
  SET_NAMED_ELT(lst, lstIdx, R_affx_GetList(chp->GetSummaryParams()), nms, 
			  "SummaryParameters");
  lstIdx++;
  qNbr = chp->GetEntryCount();
  PROTECT(nQ = NEW_INTEGER(1));
  INTEGER(nQ)[0] = qNbr;
  SET_NAMED_ELT(lst, lstIdx, nQ, nms, "NumQuantificationEntries");
  lstIdx++;

  lstIdx = R_affx_AddCHPEntries(chp, lst, nms, lstIdx, isBrief);

  SET_NAMES(lst, nms);
  UNPROTECT(3);
  return(lst);
}

/* the enumerated data types for a MultiData object

ExpressionMultiDataType 	
ExpressionControlMultiDataType 	
GenotypeMultiDataType 	
GenotypeControlMultiDataType 	
CopyNumberMultiDataType 	
CytoMultiDataType 
  */

SEXP 
R_affx_ReadCHP(FusionCHPMultiDataData *chp, bool isBrief)
{
  SEXP lst, nms, cts, conf, call, probenames, genodata, gnms;
  int lstIdx = 0, lstNbr, nExpr, nExprC, nGeno, nGenoC,
    nprotect = 0, i, nDataTypes;

  PROTECT(cts = NEW_INTEGER(4)); //FIXME: change if enum changes
  nprotect++;
  INTEGER(cts)[0] = nExpr = chp->GetEntryCount(ExpressionMultiDataType);
  INTEGER(cts)[1] = nExprC = chp->GetEntryCount(ExpressionControlMultiDataType);
  INTEGER(cts)[2] = nGeno = chp->GetEntryCount(GenotypeMultiDataType);
  INTEGER(cts)[3] = nGenoC = chp->GetEntryCount(GenotypeControlMultiDataType);

  PROTECT(nms = NEW_CHARACTER(4));
  SET_STRING_ELT(nms, 0, mkChar("Expression"));
  SET_STRING_ELT(nms, 1, mkChar("ExpressionControl"));
  SET_STRING_ELT(nms, 2, mkChar("Genotype"));
  SET_STRING_ELT(nms, 3, mkChar("GenotypeControl"));
  SET_NAMES(cts, nms);
  UNPROTECT(1);

  /*  for now ignore the next two, as affxparser seems to be using an
  old version of some files
      INTEGER(cts)[4] = nCopy = chp->GetEntryCount(CopyNumberMultiDataType);
      INTEGER(cts)[5] = nCyto = chp->GetEntryCount(CytoMultiDataType);
  */

  nDataTypes = 0;
  if(nExpr > 0) nDataTypes++;
  if(nExprC > 0) nDataTypes++;
  if(nGeno > 0) nDataTypes++;
  if(nGenoC > 0) nDataTypes++;

  lstNbr = nDataTypes + 7;
  PROTECT(lst = NEW_LIST(lstNbr));
  PROTECT(nms = NEW_CHARACTER(lstNbr));
  nprotect+=2;
 
  lstIdx = R_affx_AddCHPMeta(chp->FileId(), chp->GetAlgName(),
			     chp->GetAlgVersion(), chp->GetArrayType(),
			     lst, nms, lstIdx);
  SET_NAMED_ELT(lst, lstIdx, R_affx_GetList(chp->GetAlgParams()), nms,
		"AlgorithmParameters");
  lstIdx++;
  SET_NAMED_ELT(lst, lstIdx, R_affx_GetList(chp->GetSummaryParams()),
			     nms, "SummaryParameters");
  lstIdx++;

  SET_NAMED_ELT(lst, lstIdx, cts, nms, "MultiDataTypeCounts");
  lstIdx++;

  if(nGeno > 0) {
    PROTECT(conf = NEW_NUMERIC(nGeno));
    PROTECT(call = NEW_INTEGER(nGeno)); //should be char vector
    PROTECT(probenames = NEW_CHARACTER(nGeno));
    for (i = 0; i < nGeno; i++) {
      INTEGER(call)[i] = chp->GetGenoCall(GenotypeMultiDataType, i);
      REAL(conf)[i] = chp->GetGenoConfidence(GenotypeMultiDataType, i);
      SET_STRING_ELT(probenames, i,
		     mkChar(chp->GetProbeSetName(GenotypeMultiDataType,
						 i).c_str())); 
    }
    PROTECT(genodata = NEW_LIST(3));
    PROTECT(gnms = NEW_CHARACTER(3));
    SET_NAMED_ELT(genodata, 0, call, gnms, "Call");
    SET_NAMED_ELT(genodata, 1, conf, gnms, "Confidence");
    SET_NAMED_ELT(genodata, 2, probenames, gnms, "ProbeNames");
    SET_NAMES(genodata, gnms);
    SET_NAMED_ELT(lst, lstIdx, genodata, nms, "Genotype");
    UNPROTECT(5);
  }

  SET_NAMES(lst, nms);
  UNPROTECT(nprotect);
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
      error("argument '%s' should be '%s'", "fname",
	       "character(1)");
    if (IS_LOGICAL(withQuantifications) == FALSE || 
	LENGTH(withQuantifications) != 1)
      error("argument '%s' should be '%s'", 
	       "withQuantifications", "logical(1)");

    const char *chpFileName = CHAR(STRING_ELT(fname, 0));
    bool
      isBrief = (LOGICAL(withQuantifications)[0] == TRUE ? false : true),
      processed = false;
    int protectionCount = 0;
    SEXP result = R_NilValue;

    FusionCHPData *chp = FusionCHPDataReg::Read(chpFileName);
    if (chp == NULL)
      error("could not read '%s'", chpFileName);
  
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
      FusionCHPMultiDataData *mChp = FusionCHPMultiDataData::FromBase(chp);
      if(mChp != NULL) {
	processed = true;
	PROTECT(result = R_affx_ReadCHP(mChp, isBrief));
	++protectionCount;
	delete mChp;
      }
    }

    if (processed==false) {
      warning("unable to read CHP file '%s'", chpFileName);
      delete chp;
    }

    UNPROTECT(protectionCount);
    return result;
  }
}
