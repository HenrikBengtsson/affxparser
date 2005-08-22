/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "CHPData.h"
#include "DataSetHeader.h"

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

CHPData::CHPData()
{
	entries = 0;
	bgZones = 0;
	Clear();
}

CHPData::CHPData(const std::string& filename, const std::string& type) 
{ 
	entries = 0;
	bgZones = 0;
	Clear();
	SetFilename(filename);
	if(type == CHP_EXPRESSION_ASSAY_TYPE)
	{
		DataGroupHeader dcHdr(CHP_EXPR_GROUP);
		genericData.Header().AddDataGroupHdr(dcHdr);
		genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_EXPRESSION_ASSAY_TYPE);
	}
	else if(type == CHP_RESEQUENCING_ASSAY_TYPE)
	{
		DataGroupHeader dcHdr(CHP_RESEQ_GROUP);
		genericData.Header().AddDataGroupHdr(dcHdr);
		genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_RESEQUENCING_ASSAY_TYPE);
	}
	else if(type == CHP_UNIVERSAL_ASSAY_TYPE)
	{
		DataGroupHeader dcHdr(CHP_UNIV_GROUP);
		genericData.Header().AddDataGroupHdr(dcHdr);
		genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_UNIVERSAL_ASSAY_TYPE);
	}
	else
	{
		DataGroupHeader dcHdr(CHP_GENO_GROUP);
		genericData.Header().AddDataGroupHdr(dcHdr);
		genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_GENOTYPING_ASSAY_TYPE);
	}

	DataGroupHeader dcHdr(CHP_BG_ZONE_GROUP);
	genericData.Header().AddDataGroupHdr(dcHdr);
}

/// backwards compatibility for gcos like calls
std::string CHPData::GetAssayType()
{
	return genericData.Header().GetGenericDataHdr()->GetFileTypeId();
}

void CHPData::GetExpressionEntry(int32_t row, CHPExpressionEntry& e)
{
	PrepareExprEntryDataSet();
	if (entries && entries->IsOpen())
	{
		std::wstring probeSetName;
		entries->GetData(row, 0, probeSetName);
		e.SetProbeSetName(probeSetName);

		u_int8_t detection = 0;
		entries->GetData(row, 1, detection);
		e.SetDetection(detection);

		float detectionPValue = 0.0;
		entries->GetData(row, 2, detectionPValue);
		e.SetDetectionPValue(detectionPValue);

		float signal = 0.0;
		entries->GetData(row, 3, signal);
		e.SetSignal(signal);

		u_int16_t numPairs = 0;
		entries->GetData(row, 4, numPairs);
		e.SetNumPairs(numPairs);

		u_int16_t numPairsUsed = 0;
		entries->GetData(row, 5, numPairsUsed);
		e.SetNumPairsUsed(numPairsUsed);

		u_int8_t change = 0;
		entries->GetData(row, 6, change);
		e.SetChange(change);

		float changePValue = 0.0;
		entries->GetData(row, 7, changePValue);
		e.SetChangePValue(changePValue);

		float sigLogRatio = 0.0;
		entries->GetData(row, 8, sigLogRatio);
		e.SetSigLogRatio(sigLogRatio);

		float sigLogRatioLo = 0.0;
		entries->GetData(row, 9, sigLogRatioLo);
		e.SetSigLogRatioLo(sigLogRatioLo);

		float sigLogRatioHi = 0.0;
		entries->GetData(row, 10, sigLogRatioHi);
		e.SetSigLogRatioHi(sigLogRatioHi);

		u_int16_t commonPairs = 0;
		entries->GetData(row, 11, commonPairs);
		e.SetCommonPairs(commonPairs);
	}
}

void CHPData::GetGenotypeEntry(int32_t row, CHPGenotypeEntry& e)
{
	PrepareGenoEntryDataSet();
	if (entries && entries->IsOpen())
	{
		std::wstring probeSetName;
		entries->GetData(row, 0, probeSetName);
		e.SetProbeSetName(probeSetName);

		u_int8_t call = 0;
		entries->GetData(row, 1, call);
		e.SetCall(call);

		float confidence = 0.0;
		entries->GetData(row, 2, confidence);
		e.SetConfidence(confidence);

		float ras1 = 0.0;
		entries->GetData(row, 3, ras1);
		e.SetRAS1(ras1);

		float ras2 = 0.0;
		entries->GetData(row, 4, ras2);
		e.SetRAS2(ras2);

		float aaCall = 0.0;
		entries->GetData(row, 5, aaCall);
		e.SetAACall(aaCall);

		float abCall = 0.0;
		entries->GetData(row, 6, abCall);
		e.SetABCall(abCall);

		float bbCall = 0.0;
		entries->GetData(row, 7, bbCall);
		e.SetBBCall(bbCall);

		float noCall = 0.0;
		entries->GetData(row, 8, noCall);
		e.SetNoCall(noCall);
	}
}

void CHPData::GetUniversalEntry(int32_t row, CHPUniversalEntry& e)
{
	PrepareUnivEntryDataSet();
	if (entries && entries->IsOpen())
	{
		float background = 0.0;
		entries->GetData(row, 0, background);
		e.SetBackground(background);
	}
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

CHPData::~CHPData() 
{ 
	Clear(); 
}

void CHPData::Clear()
{
	if (entries){ entries->Delete();  entries = 0; }
	if (bgZones){ bgZones->Delete();  bgZones = 0; }
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

void CHPData::SetEntryCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	AddColumns(dpHdr);
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	dcHdr->AddDataSetHdr(dpHdr);
}

void CHPData::AddColumns(DataSetHeader& hdr)
{
	if(genericData.Header().GetGenericDataHdr()->GetFileTypeId() == CHP_EXPRESSION_ASSAY_TYPE)
	{
		AddExpressionColumns(hdr);
	}
	else if(genericData.Header().GetGenericDataHdr()->GetFileTypeId() == CHP_RESEQUENCING_ASSAY_TYPE)
	{
		AddResequencingColumns(hdr);
	}
	else if(genericData.Header().GetGenericDataHdr()->GetFileTypeId() == CHP_UNIVERSAL_ASSAY_TYPE)
	{
		AddUniversalColumns(hdr);
	}
	else
	{
		AddGenotypingColumns(hdr);
	}
}

void CHPData::AddResequencingColumns(DataSetHeader& hdr)
{
	hdr.SetName(CHP_RESEQ_GROUP);

	//TODO: add resequencing columns
}

void CHPData::AddExpressionColumns(DataSetHeader& hdr)
{
	hdr.SetName(CHP_EXPR_GROUP);
	//Probeset name - string
	hdr.AddUnicodeColumnType(32);
	//Detection - unsigned char
	hdr.AddUByteColumnType();
	//Detection p-value - float
	hdr.AddFloatColumnType();
	//Signal - float
	hdr.AddFloatColumnType();
	//Number of pairs - unsigned short
	hdr.AddUShortColumnType();
	//Number of pairs used - unsigned short
	hdr.AddUShortColumnType();
	//Change - unsigned char
	hdr.AddUByteColumnType();
	//Change p-value - float
	hdr.AddFloatColumnType();
	//Signal Log Ratio - float
	hdr.AddFloatColumnType();
	//Signal Log Ratio Low - float
	hdr.AddFloatColumnType();
	//Signal Log Ratio High - float
	hdr.AddFloatColumnType();
	//Common Pairs - unsigned short
	hdr.AddUShortColumnType();
}

void CHPData::AddGenotypingColumns(DataSetHeader& hdr)
{
	hdr.SetName(CHP_GENO_GROUP);
	//Probeset name - string
	hdr.AddUnicodeColumnType(32);
	//Call - unsigned char
	hdr.AddUByteColumnType();
	//Confidence - float
	hdr.AddFloatColumnType();
	//RAS1 - float
	hdr.AddFloatColumnType();
	//RAS2 - float
	hdr.AddFloatColumnType();
	//AA Call - float
	hdr.AddFloatColumnType();
	//AB Call - float
	hdr.AddFloatColumnType();
	//BB Call - float
	hdr.AddFloatColumnType();
	//No Call - float
	hdr.AddFloatColumnType();
}

void CHPData::AddUniversalColumns(DataSetHeader& hdr)
{
	hdr.SetName(CHP_UNIV_GROUP);
	//Background - float
	hdr.AddFloatColumnType();
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
	dpHdr.AddFloatColumnType();
	//center Y coord - float
	dpHdr.AddFloatColumnType();
	//background - float
	dpHdr.AddFloatColumnType();
	//smoothing factor - float
	dpHdr.AddFloatColumnType();

	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(1);
	dcHdr->AddDataSetHdr(dpHdr);
}

std::wstring CHPData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

void CHPData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value);
}

std::wstring CHPData::GetAlgName()
{
	return GetWStringFromGenericHdr(CHP_ALG_NAME);
}

void CHPData::SetAlgName(const std::wstring& value)
{
	SetWStringToGenericHdr(CHP_ALG_NAME, value);
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
	return GetWStringFromGenericHdr(CHP_ALG_VERSION);
}

void CHPData::SetAlgVersion(const std::wstring& value)
{
	SetWStringToGenericHdr(CHP_ALG_VERSION, value);
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
	ParameterNameValueType type;
	type.SetName(tag);
	type.SetValueText(GetWStringFromGenericHdr(CHP_ALG_PARAM + tag));
	return type;
}

ParameterNameValueType CHPData::GetChipSum(const std::wstring& tag)
{
	ParameterNameValueType type;
	type.SetName(tag);
	type.SetValueText(GetWStringFromGenericHdr(CHP_CHIP_SUM + tag));
	return type;
}

ParameterNameValueTypeVector CHPData::GetAlgParams()
{
	ParameterNameValueTypeVector nvt;
	ParameterNameValueTypeIt begin, end;

	genericData.Header().GetGenericDataHdr()->GetNameValIterators(begin, end);
	while(begin != end)
	{
		std::wstring key = begin->GetName();
		if(key.compare(0, CHP_ALG_PARAM.size(),CHP_ALG_PARAM) == 0)
		{
			nvt.push_back(*begin);
		}
		begin++;
	}
	return nvt;
}

void CHPData::AddAlgParam(const std::wstring& name, const std::wstring& param)
{
	std::wstring paramName = CHP_ALG_PARAM + name;
	SetWStringToGenericHdr(paramName, param);
}

ParameterNameValueTypeVector CHPData::GetChipSums()
{
	ParameterNameValueTypeVector nvt;
	ParameterNameValueTypeIt begin, end;

	genericData.Header().GetGenericDataHdr()->GetNameValIterators(begin, end);
	while(begin != end)
	{
		std::wstring key = begin->GetName();
		if(key.compare(0, CHP_CHIP_SUM.size(),CHP_CHIP_SUM) == 0)
		{
			nvt.push_back(*begin);
		}
		begin++;
	}
	return nvt;
}

void CHPData::AddChipSum(const std::wstring& name, const std::wstring& param)
{
	std::wstring paramName = CHP_CHIP_SUM + name;
	SetWStringToGenericHdr(paramName, param);
}

int32_t CHPData::GetRows()
{
	if (cachedRows == -1)
		cachedRows = GetInt32FromGenericHdr(CHP_ROWS);
	return cachedRows;
}

void CHPData::SetRows(int32_t value)
{
	SetInt32ToGenericHdr(CHP_ROWS, value);
	cachedCols = value;
}

int32_t CHPData::GetCols()
{
	if (cachedCols == -1)
		cachedCols = GetInt32FromGenericHdr(CHP_COLS);
	return cachedCols;
}

void CHPData::SetCols(int32_t value)
{
	SetInt32ToGenericHdr(CHP_COLS, value);
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

void CHPData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value);
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

void CHPData::PrepareExprEntryDataSet()
{
	entries = genericData.DataSet(CHP_EXPR_GROUP, CHP_EXPR_GROUP);
	if (entries)
	{
		entries->Open();
	}
}

void CHPData::PrepareGenoEntryDataSet()
{
	entries = genericData.DataSet(CHP_GENO_GROUP, CHP_GENO_GROUP);
	if (entries)
	{
		entries->Open();
	}
}

void CHPData::PrepareUnivEntryDataSet()
{
	entries = genericData.DataSet(CHP_UNIV_GROUP, CHP_UNIV_GROUP);
	if (entries)
	{
		entries->Open();
	}
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

