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


#include "calvin_files/data/src/CHPData.h"
//
#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/utils/src/StringUtils.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

// Constant column names.
#define CallColName std::wstring( L"Call")
#define ScoreColName std::wstring( L"Score")
#define BackgroundColName std::wstring( L"Background")
#define ConfidenceColName std::wstring( L"Confidence")
#define RAS1ColName std::wstring( L"RAS1")
#define RAS2ColName std::wstring( L"RAS2")
#define AAColName std::wstring( L"AA Call p-value")
#define ABColName std::wstring( L"AB Call p-value")
#define BBColName std::wstring( L"BB Call p-value")
#define NoCallColName std::wstring( L"No Call p-value")
#define ProbeSetNameColName std::wstring( L"Probe Set Name")
#define DetectionColName std::wstring( L"Detection")
#define DetectionPValueColName std::wstring( L"Detection p-value")
#define SignalColName std::wstring( L"Signal")
#define NumberPairsColName std::wstring( L"Number of Pairs")
#define NumberPairsUsedColName std::wstring( L"Number of Pairs Used")
#define ChangeColName std::wstring( L"Change")
#define ChangePValueColName std::wstring( L"Change p-value")
#define SignalLogRatioColName std::wstring( L"Signal Log Ratio")
#define SignalLogRatioLowColName std::wstring( L"Signal Log Ratio Low")
#define SignalLogRatioHighColName std::wstring( L"Signal Log Ratio High")
#define CommonPairsColName std::wstring( L"Common Pairs")
#define CenterXColName std::wstring( L"Center X")
#define CenterYColName std::wstring( L"Center Y")
#define SmoothFactorColName std::wstring( L"Smooth Factor")
#define PositionColName std::wstring( L"Position")
#define ReasonColName std::wstring( L"Reason")
#define ForceCallColName std::wstring( L"Force Call")
#define OriginalCallColName std::wstring( L"Original Call")


CHPData::CHPData()
{
	entriesExp = 0;
	entriesGeno = 0;
	entriesUniv = 0;
	entriesReseq = 0;
	bgZones = 0;
	forceSet = 0;
	origSet = 0;
	wideProbeSetNames = false;
	Clear();
}

CHPData::CHPData(const std::string& filename, const std::string &assayType) 
{ 
	entriesExp = 0;
	entriesGeno = 0;
	entriesUniv = 0;
	entriesReseq = 0;
	bgZones = 0;
	forceSet = 0;
	origSet = 0;
	wideProbeSetNames = false;
	Clear();
	SetFilename(filename);

	std::wstring groupName;
	if (assayType == CHP_EXPRESSION_ASSAY_TYPE)
		groupName = CHP_EXPR_GROUP;

	else if (assayType == CHP_RESEQUENCING_ASSAY_TYPE)
		groupName = CHP_RESEQ_GROUP;

	else if (assayType == CHP_GENOTYPING_ASSAY_TYPE)
		groupName = CHP_GENO_GROUP;

	else if (assayType == CHP_UNIVERSAL_ASSAY_TYPE)
		groupName = CHP_UNIV_GROUP;

	else
		return;

	DataGroupHeader dcHdr(groupName);
	genericData.Header().AddDataGroupHdr(dcHdr);
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(assayType);
	DataGroupHeader dcHdrBg(CHP_BG_ZONE_GROUP);
	genericData.Header().AddDataGroupHdr(dcHdrBg);

	// Now add the force and orig sets for reseq designs.
	if (assayType == CHP_RESEQUENCING_ASSAY_TYPE)
	{
		DataGroupHeader dcHdrForce(CHP_RESEQ_FORCE_CALL_GROUP);
		genericData.Header().AddDataGroupHdr(dcHdrForce);
		DataGroupHeader dcHdrOrig(CHP_RESEQ_ORIG_CALL_GROUP);
		genericData.Header().AddDataGroupHdr(dcHdrOrig);
	}

}

void CHPData::GetEntry(int32_t row, CHPGenotypeEntry& e)
{
	PrepareGenoEntryDataSet();
	if (entriesGeno && entriesGeno->IsOpen())
	{
		std::string probeSetName;
		if (wideProbeSetNames == false)
			entriesGeno->GetData(row, 0, probeSetName);
		else
		{
			std::wstring wprobeSetName;
			entriesGeno->GetData(row, 0, wprobeSetName);
			probeSetName = StringUtils::ConvertWCSToMBS(wprobeSetName);
		}
		e.SetProbeSetName(probeSetName);

		u_int8_t call = 0;
		entriesGeno->GetData(row, 1, call);
		e.SetCall(call);

		float confidence = 0.0;
		entriesGeno->GetData(row, 2, confidence);
		e.SetConfidence(confidence);

		float ras1 = 0.0;
		entriesGeno->GetData(row, 3, ras1);
		e.SetRAS1(ras1);

		float ras2 = 0.0;
		entriesGeno->GetData(row, 4, ras2);
		e.SetRAS2(ras2);

		float aaCall = 0.0;
		entriesGeno->GetData(row, 5, aaCall);
		e.SetAACall(aaCall);

		float abCall = 0.0;
		entriesGeno->GetData(row, 6, abCall);
		e.SetABCall(abCall);

		float bbCall = 0.0;
		entriesGeno->GetData(row, 7, bbCall);
		e.SetBBCall(bbCall);

		float noCall = 0.0;
		entriesGeno->GetData(row, 8, noCall);
		e.SetNoCall(noCall);
	}
}

void CHPData::GetEntry(int32_t row, CHPExpressionEntry& e)
{
	PrepareExprEntryDataSet();
	if (entriesExp && entriesExp->IsOpen())
	{
		int colIndex = 0;
		std::string probeSetName;
		if (wideProbeSetNames == false)
			entriesExp->GetData(row, colIndex++, probeSetName);
		else
		{
			std::wstring wprobeSetName;
			entriesExp->GetData(row, colIndex++, wprobeSetName);
			probeSetName = StringUtils::ConvertWCSToMBS(wprobeSetName);
		}
		e.SetProbeSetName(probeSetName);

		u_int8_t detection = 0;
		entriesExp->GetData(row, colIndex++, detection);
		e.SetDetection(detection);

		float detectionPValue = 0.0;
		entriesExp->GetData(row, colIndex++, detectionPValue);
		e.SetDetectionPValue(detectionPValue);

		float signal = 0.0;
		entriesExp->GetData(row, colIndex++, signal);
		e.SetSignal(signal);

		u_int16_t numPairs = 0;
		entriesExp->GetData(row, colIndex++, numPairs);
		e.SetNumPairs(numPairs);

		u_int16_t numPairsUsed = 0;
		entriesExp->GetData(row, colIndex++, numPairsUsed);
		e.SetNumPairsUsed(numPairsUsed);

		e.SetHasComparisonData(false);

		if (entriesExp->Cols() > colIndex)
		{
			e.SetHasComparisonData(true);

			u_int8_t change = 0;
			entriesExp->GetData(row, colIndex++, change);
			e.SetChange(change);

			float changePValue = 0.0;
			entriesExp->GetData(row, colIndex++, changePValue);
			e.SetChangePValue(changePValue);

			float sigLogRatio = 0.0;
			entriesExp->GetData(row, colIndex++, sigLogRatio);
			e.SetSigLogRatio(sigLogRatio);

			float sigLogRatioLo = 0.0;
			entriesExp->GetData(row, colIndex++, sigLogRatioLo);
			e.SetSigLogRatioLo(sigLogRatioLo);

			float sigLogRatioHi = 0.0;
			entriesExp->GetData(row, colIndex++, sigLogRatioHi);
			e.SetSigLogRatioHi(sigLogRatioHi);

			u_int16_t commonPairs = 0;
			entriesExp->GetData(row, colIndex++, commonPairs);
			e.SetCommonPairs(commonPairs);
		}
	}
}

void CHPData::GetEntry(int32_t row, CHPUniversalEntry& e)
{
	PrepareUnivEntryDataSet();
	if (entriesUniv && entriesUniv->IsOpen())
	{
		float background = 0.0;
		entriesUniv->GetData(row, 0, background);
		e.SetBackground(background);
	}
}

void CHPData::GetEntry(int32_t row, CHPReseqEntry& e)
{
	PrepareReseqEntryDataSet();
	if (entriesReseq && entriesReseq->IsOpen())
	{
		int8_t call;
		entriesReseq->GetData(row, 0, call);
		e.call=call;
		entriesReseq->GetData(row, 1, e.score);
	}
}

void CHPData::PrepareGenoEntryDataSet()
{
	if (entriesGeno == 0)
	{
		entriesGeno = genericData.DataSet(CHP_GENO_GROUP, CHP_GENO_GROUP);
		if (entriesGeno)
		{
			entriesGeno->Open();
			wideProbeSetNames = (entriesGeno->Header().GetColumnInfo(0).GetColumnType() == UnicodeCharColType);
		}
	}
}

void CHPData::PrepareExprEntryDataSet()
{
	if (entriesExp == 0)
	{
		entriesExp = genericData.DataSet(CHP_EXPR_GROUP, CHP_EXPR_GROUP);
		if (entriesExp)
		{
			entriesExp->Open();
			wideProbeSetNames = (entriesExp->Header().GetColumnInfo(0).GetColumnType() == UnicodeCharColType);
		}
	}
}

void CHPData::PrepareUnivEntryDataSet()
{
	if (entriesUniv == 0)
	{
		entriesUniv = genericData.DataSet(CHP_UNIV_GROUP, CHP_UNIV_GROUP);
		if (entriesUniv)
		{
			entriesUniv->Open();
		}
	}
}

void CHPData::PrepareReseqEntryDataSet()
{
	if (entriesReseq == 0)
	{
		entriesReseq = genericData.DataSet(CHP_RESEQ_GROUP, CHP_RESEQ_GROUP);
		if (entriesReseq)
		{
			entriesReseq->Open();
		}
	}
}

void CHPData::AddColumns(DataSetHeader& hdr, bool hasCompData)
{
	if(genericData.Header().GetGenericDataHdr()->GetFileTypeId() == CHP_EXPRESSION_ASSAY_TYPE)
		AddExprColumns(hdr, hasCompData);

	if(genericData.Header().GetGenericDataHdr()->GetFileTypeId() == CHP_GENOTYPING_ASSAY_TYPE)
		AddGenoColumns(hdr);

	else if(genericData.Header().GetGenericDataHdr()->GetFileTypeId() == CHP_UNIVERSAL_ASSAY_TYPE)
		AddUnivColumns(hdr);

	else if(genericData.Header().GetGenericDataHdr()->GetFileTypeId() == CHP_RESEQUENCING_ASSAY_TYPE)
		AddReseqColumns(hdr);
}

void CHPData::AddReseqColumns(DataSetHeader& hdr)
{
	hdr.SetName(CHP_RESEQ_GROUP);
	//call - char
	hdr.AddByteColumn(CallColName);
	//Score - float
	hdr.AddFloatColumn(ScoreColName);
}

void CHPData::AddUnivColumns(DataSetHeader& hdr)
{
	hdr.SetName(CHP_UNIV_GROUP);
	//Background - float
	hdr.AddFloatColumn(BackgroundColName);
}

void CHPData::AddGenoColumns(DataSetHeader& hdr)
{
	hdr.SetName(CHP_GENO_GROUP);
	//Probeset name - string
	hdr.AddAsciiColumn(ProbeSetNameColName, maxProbeSetName);
	//Call - unsigned char
	hdr.AddUByteColumn(CallColName);
	//Confidence - float
	hdr.AddFloatColumn(ConfidenceColName);
	//RAS1 - float
	hdr.AddFloatColumn(RAS1ColName);
	//RAS2 - float
	hdr.AddFloatColumn(RAS2ColName);
	//AA Call - float
	hdr.AddFloatColumn(AAColName);
	//AB Call - float
	hdr.AddFloatColumn(ABColName);
	//BB Call - float
	hdr.AddFloatColumn(BBColName);
	//No Call - float
	hdr.AddFloatColumn(NoCallColName);
}

void CHPData::AddExprColumns(DataSetHeader& hdr, bool hasCompData)
{
	hdr.SetName(CHP_EXPR_GROUP);
	//Probeset name - string
	hdr.AddAsciiColumn(ProbeSetNameColName, maxProbeSetName);
	//Detection - unsigned char
	hdr.AddUByteColumn(DetectionColName);
	//Detection p-value - float
	hdr.AddFloatColumn(DetectionPValueColName);
	//Signal - float
	hdr.AddFloatColumn(SignalColName);
	//Number of pairs - unsigned short
	hdr.AddUShortColumn(NumberPairsColName);
	//Number of pairs used - unsigned short
	hdr.AddUShortColumn(NumberPairsUsedColName);

	if (hasCompData == true)
	{
		//Change - unsigned char
		hdr.AddUByteColumn(ChangeColName);
		//Change p-value - float
		hdr.AddFloatColumn(ChangePValueColName);
		//Signal Log Ratio - float
		hdr.AddFloatColumn(SignalLogRatioColName);
		//Signal Log Ratio Low - float
		hdr.AddFloatColumn(SignalLogRatioLowColName);
		//Signal Log Ratio High - float
		hdr.AddFloatColumn(SignalLogRatioHighColName);
		//Common Pairs - unsigned short
		hdr.AddUShortColumn(CommonPairsColName);
	}
}

/// backwards compatibility for gcos like calls
std::string CHPData::GetAssayType()
{
	return genericData.Header().GetGenericDataHdr()->GetFileTypeId();
}

void CHPData::GetBackgroundZones(int32_t row, int32_t rowCnt, CHPBackgroundZoneVector& zones)
{
	CHPBackgroundZone z;
	for(int i = row; i < rowCnt; i++)
	{
		GetBackgroundZone(i, z);
		zones.push_back(z);
		z.Clear();
	}
}

void CHPData::GetBackgroundZone(int32_t row, CHPBackgroundZone& zone)
{
	PrepareBackgroundZoneDataSet();
	if (bgZones && bgZones->IsOpen())
	{
		float centerX = 0.0;
		bgZones->GetData(row, 0, centerX);
		zone.SetCenterX(centerX);

		float centerY = 0.0;
		bgZones->GetData(row, 1, centerY);
		zone.SetCenterY(centerY);

		float background = 0.0;
		bgZones->GetData(row, 2, background);
		zone.SetBackground(background);

		float smoothFactor = 0.0;
		bgZones->GetData(row, 3, smoothFactor);
		zone.SetSmoothFactor(smoothFactor);
	}
}

void CHPData::GetForceCall(int32_t row, CHPReseqForceCall& force)
{
	PrepareForceDataSet();
	if (forceSet && forceSet->IsOpen())
	{
		forceSet->GetData(row, 0, force.position);
		int8_t value;
		forceSet->GetData(row, 1, value);
		force.call = value;
		forceSet->GetData(row, 2, value);
		force.reason = value;
	}
}

void CHPData::GetOrigCall(int32_t row, CHPReseqOrigCall &orig)
{
	PrepareOrigDataSet();
	if (origSet && origSet->IsOpen())
	{
		origSet->GetData(row, 0, orig.position);
		int8_t value;
		origSet->GetData(row, 1, value);
		orig.call = value;
	}
}

CHPData::~CHPData() 
{ 
	Clear(); 
}

void CHPData::Clear()
{
	if (entriesGeno){ entriesGeno->Delete();  entriesGeno = 0; }
	if (entriesExp){ entriesExp->Delete();  entriesExp = 0; }
	if (entriesUniv){ entriesUniv->Delete();  entriesUniv = 0; }
	if (entriesReseq){ entriesReseq->Delete();  entriesReseq = 0; }
	if (bgZones){ bgZones->Delete();  bgZones = 0; }
	if (forceSet) { forceSet->Delete(); forceSet = 0; }
	if (origSet) { origSet->Delete(); origSet = 0; }
	genericData.Header().Clear();
	cachedRows = -1;
	cachedCols = -1;
}

void CHPData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string CHPData::GetFilename() const
{
	return ((GenericData&)genericData).Header().GetFilename();
}

u_int8_t CHPData::GetVersion()
{
	return genericData.Header().GetVersion();
}

u_int8_t CHPData::GetMagic()
{
	return genericData.Header().GetMagicNumber();
}

int32_t CHPData::GetEntryCount()
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr = dcHdr->GetDataSet(0);
	return dpHdr.GetRowCnt();
}

void CHPData::SetEntryCount(int32_t ln, int32_t maxln, bool hasCompData)
{
	maxProbeSetName = maxln;
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	AddColumns(dpHdr, hasCompData);
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	dcHdr->AddDataSetHdr(dpHdr);
}

int32_t CHPData::GetBackgroundZoneCnt()
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(1);
	DataSetHeader dpHdr = dcHdr->GetDataSet(0);
	return dpHdr.GetRowCnt();
}

void CHPData::SetBackgroundZoneCnt(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CHP_BG_ZONE_GROUP);

	//center X coord - float
	dpHdr.AddFloatColumn(CenterXColName);
	//center Y coord - float
	dpHdr.AddFloatColumn(CenterYColName);
	//background - float
	dpHdr.AddFloatColumn(BackgroundColName);
	//smoothing factor - float
	dpHdr.AddFloatColumn(SmoothFactorColName);

	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(1);
	dcHdr->AddDataSetHdr(dpHdr);
}

int32_t CHPData::GetForceCnt()
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(2);
	DataSetHeader dpHdr = dcHdr->GetDataSet(0);
	return dpHdr.GetRowCnt();
}

void CHPData::SetForceCnt(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CHP_RESEQ_FORCE_CALL_GROUP);

	//position - int
	dpHdr.AddIntColumn(PositionColName);
	//call - byte
	dpHdr.AddByteColumn(ForceCallColName);
	//reason - byte
	dpHdr.AddByteColumn(ReasonColName);

	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(2);
	dcHdr->AddDataSetHdr(dpHdr);
}

int32_t CHPData::GetOrigCnt()
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(3);
	DataSetHeader dpHdr = dcHdr->GetDataSet(0);
	return dpHdr.GetRowCnt();
}

void CHPData::SetOrigCnt(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CHP_RESEQ_ORIG_CALL_GROUP);

	//position - int
	dpHdr.AddIntColumn(PositionColName);
	//call - byte
	dpHdr.AddByteColumn(OriginalCallColName);

	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(3);
	dcHdr->AddDataSetHdr(dpHdr);
}

std::wstring CHPData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

void CHPData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value, ARRAY_TYPE_MAX_LEN);
}

std::wstring CHPData::GetAlgName()
{
	return GetWStringFromGenericHdr(ALGORITHM_NAME_PARAM_NAME);
}

void CHPData::SetAlgName(const std::wstring& value)
{
	SetWStringToGenericHdr(ALGORITHM_NAME_PARAM_NAME, value);
}

std::wstring CHPData::GetParentCell()
{
	return GetWStringFromGenericHdr(CHP_PARENT_CELL);
}

void CHPData::SetParentCell(const std::wstring& value)
{
	SetWStringToGenericHdr(CHP_PARENT_CELL, value);
}

std::wstring CHPData::GetAlgVersion()
{
	return GetWStringFromGenericHdr(ALG_VERSION_PARAM_NAME);
}

void CHPData::SetAlgVersion(const std::wstring& value)
{
	SetWStringToGenericHdr(ALG_VERSION_PARAM_NAME, value);
}

std::wstring CHPData::GetProgId()
{
	return GetWStringFromGenericHdr(CHP_PROGID);
}

void CHPData::SetProgId(const std::wstring& value)
{
	SetWStringToGenericHdr(CHP_PROGID, value);
}

ParameterNameValueType CHPData::GetAlgParam(const std::wstring& tag)
{
	std::wstring name = ALGORITHM_PARAM_NAME_PREFIX_S + tag;
	ParameterNameValueType paramType;
    GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
    hdr->FindNameValParam(name, paramType);
	ParameterNameValueType type = paramType;
	type.SetName(tag);
	return type;
}

ParameterNameValueType CHPData::GetChipSum(const std::wstring& tag)
{
	std::wstring name = CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S + tag;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->FindNameValParam(name, paramType);
	ParameterNameValueType type = paramType;
	type.SetName(tag);
	return type;
}

ParameterNameValueTypeVector CHPData::GetAlgParams()
{
	ParameterNameValueTypeVector nvt;
	ParameterNameValueTypeIt begin, end;
	ParameterNameValueType param;

	genericData.Header().GetGenericDataHdr()->GetNameValIterators(begin, end);
	while(begin != end)
	{
		std::wstring key = begin->GetName();
		if(key.compare(0, ALGORITHM_PARAM_NAME_PREFIX_S.size(),ALGORITHM_PARAM_NAME_PREFIX_S) == 0)
		{
			param = *begin;
			key.erase(0, ALGORITHM_PARAM_NAME_PREFIX_S.size());
			param.SetName(key);
			nvt.push_back(param);
		}
		begin++;
	}
	return nvt;
}

void CHPData::AddAlgParam(const std::wstring& name, const std::wstring& param)
{
	std::wstring paramName = ALGORITHM_PARAM_NAME_PREFIX_S + name;
	SetWStringToGenericHdr(paramName, param);
}

void CHPData::AddAlgParam(const std::wstring& name, int param)
{
	std::wstring paramName = ALGORITHM_PARAM_NAME_PREFIX_S + name;
	SetInt32ToGenericHdr(paramName, param);
}

void CHPData::AddAlgParam(const std::wstring& name, float param)
{
	std::wstring paramName = ALGORITHM_PARAM_NAME_PREFIX_S + name;
	SetFloatToGenericHdr(paramName, param);
}

ParameterNameValueTypeVector CHPData::GetChipSums()
{
	ParameterNameValueTypeVector nvt;
	ParameterNameValueTypeIt begin, end;
	ParameterNameValueType param;

	genericData.Header().GetGenericDataHdr()->GetNameValIterators(begin, end);
	while(begin != end)
	{
		std::wstring key = begin->GetName();
		if(key.compare(0, CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S.size(),CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S) == 0)
		{
			param = *begin;
			key.erase(0, CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S.size());
			param.SetName(key);
			nvt.push_back(param);
		}
		begin++;
	}
	return nvt;
}

void CHPData::AddChipSum(const std::wstring& name, const std::wstring& param)
{
	std::wstring paramName = CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S + name;
	SetWStringToGenericHdr(paramName, param);
}

void CHPData::AddChipSum(const std::wstring& name, float param)
{
	std::wstring paramName = CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S + name;
	SetFloatToGenericHdr(paramName, param);
}

void CHPData::AddChipSum(const std::wstring& name, int32_t param)
{
	std::wstring paramName = CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S + name;
	SetInt32ToGenericHdr(paramName, param);
}

int32_t CHPData::GetRows()
{
	if (cachedRows == -1)
		cachedRows = GetInt32FromGenericHdr(CEL_ROWS_PARAM_NAME);
	return cachedRows;
}

void CHPData::SetRows(int32_t value)
{
	SetInt32ToGenericHdr(CEL_ROWS_PARAM_NAME, value);
	cachedCols = value;
}

int32_t CHPData::GetCols()
{
	if (cachedCols == -1)
		cachedCols = GetInt32FromGenericHdr(CEL_COLS_PARAM_NAME);
	return cachedCols;
}

void CHPData::SetCols(int32_t value)
{
	SetInt32ToGenericHdr(CEL_COLS_PARAM_NAME, value);
	cachedCols = value;
}

std::wstring CHPData::GetWStringFromGenericHdr(const std::wstring& name)
{
	std::wstring result;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(name, paramType))
	{
		result = paramType.GetValueText();
	}
	return result;
}

void CHPData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value, reserve);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

int32_t CHPData::GetInt32FromGenericHdr(const std::wstring& name)
{
	int32_t result = 0;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(name, paramType))
	{
		result = paramType.GetValueInt32();
	}
	return result;
}

void CHPData::SetInt32ToGenericHdr(const std::wstring& name, int32_t value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueInt32(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

void CHPData::SetFloatToGenericHdr(const std::wstring& name, float value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueFloat(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

void CHPData::PrepareBackgroundZoneDataSet()
{
	if (bgZones == 0)
	{
		bgZones = genericData.DataSet(CHP_BG_ZONE_GROUP, CHP_BG_ZONE_GROUP);
		if (bgZones)
		{
			bgZones->Open();
		}
	}
}

void CHPData::PrepareForceDataSet()
{
	if (forceSet == 0)
	{
		forceSet = genericData.DataSet(CHP_RESEQ_FORCE_CALL_GROUP, CHP_RESEQ_FORCE_CALL_GROUP);
		if (forceSet)
		{
			forceSet->Open();
		}
	}
}

void CHPData::PrepareOrigDataSet()
{
	if (origSet == 0)
	{
		origSet = genericData.DataSet(CHP_RESEQ_ORIG_CALL_GROUP, CHP_RESEQ_ORIG_CALL_GROUP);
		if (origSet)
		{
			origSet->Open();
		}
	}
}

