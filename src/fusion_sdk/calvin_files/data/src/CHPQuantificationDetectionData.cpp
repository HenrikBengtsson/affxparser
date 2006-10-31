////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#include "CHPQuantificationDetectionData.h"
#include "DataSetHeader.h"
#include "StringUtils.h"

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

/*! The group and data set to store the quantification/detection data. */
const static std::wstring QUANTIFICATION_DETECTION_NAME = L"QuantificationDetection";

/*! The column name for the probe set name. */
const static std::wstring QUANTIFICATION_DETECTION_PROBE_SET_NAME = L"ProbeSetName";

/*! The column name for the probe set id. */
const static std::wstring QUANTIFICATION_DETECTION_PROBE_SET_ID = L"ProbeSetId";

/*! The column name for the quantification value. */
const static std::wstring QUANTIFICATION_DETECTION_QUANTIFICATION_NAME = L"Quantification";

/*! The column name for the detection value. */
const static std::wstring QUANTIFICATION_DETECTION_DETECTION_NAME = L"Detection";

/*! A prefix for chip summary parameter ids. */
const static std::wstring CHIP_SUMMARY_PARAMETER_NAME_PREFIX = L"affymetrix-chipsummary-";

CHPQuantificationDetectionData::CHPQuantificationDetectionData()
{
    maxProbeSetName = -1;
	firstColumnType = UnicodeCharColType;
	entries = NULL;
	Clear();
}

CHPQuantificationDetectionData::CHPQuantificationDetectionData(const std::string& filename) 
{ 
    maxProbeSetName = -1;
	firstColumnType = UnicodeCharColType;
	entries = NULL;
	Clear();
	SetFilename(filename);
	DataGroupHeader dcHdr(QUANTIFICATION_DETECTION_NAME);
	genericData.Header().AddDataGroupHdr(dcHdr);
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_QUANTIFICATION_DETECTION_TYPE);
}

CHPQuantificationDetectionData::~CHPQuantificationDetectionData() 
{ 
	Clear(); 
}

DataSetHeader &CHPQuantificationDetectionData::GetDataSetHeader()
{
    return genericData.Header().GetDataGroup(0).GetDataSet(0);
}

void CHPQuantificationDetectionData::Clear()
{
	if (entries){ entries->Delete();  entries = 0; }
	genericData.Header().Clear();
}

void CHPQuantificationDetectionData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string CHPQuantificationDetectionData::GetFilename() const
{
	return ((GenericData&)genericData).Header().GetFilename();
}

int32_t CHPQuantificationDetectionData::GetEntryCount()
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr = dcHdr->GetDataSet(0);
	return dpHdr.GetRowCnt();
}

void CHPQuantificationDetectionData::SetEntryCount(int32_t ln, int32_t maxln)
{
    firstColumnType = ASCIICharColType;
	maxProbeSetName = maxln;
	ParameterNameValueType param;
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(QUANTIFICATION_DETECTION_NAME);
	AddColumns(dpHdr, false);
	dcHdr->AddDataSetHdr(dpHdr);
}

void CHPQuantificationDetectionData::SetEntryCount(int32_t ln)
{
    firstColumnType = IntColType;
	maxProbeSetName = -1;
	ParameterNameValueType param;
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(QUANTIFICATION_DETECTION_NAME);
	AddColumns(dpHdr, true);
	dcHdr->AddDataSetHdr(dpHdr);
}

void CHPQuantificationDetectionData::GetQuantificationDetectionEntry(int index, affymetrix_calvin_data::ProbeSetQuantificationDetectionData &entry)
{
	OpenQuantificationDetectionDataSet();
	if (entries && entries->IsOpen())
	{
        entry.id = -1;
        entry.name.clear();
		if (firstColumnType == ASCIICharColType)
			entries->GetData(index, 0, entry.name);
		else if (firstColumnType == UnicodeCharColType)
		{
            std::wstring wprobeSetName;
            entries->GetData(index, 0, wprobeSetName);
            entry.name = StringUtils::ConvertWCSToMBS(wprobeSetName);
		}
        else if (firstColumnType == IntColType)
            entries->GetData(index, 0, entry.id);
        entries->GetData(index, 1, entry.quantification);
        entries->GetData(index, 2, entry.pvalue);
	}
}

void CHPQuantificationDetectionData::AddColumns(DataSetHeader& hdr, bool keyIsID)
{
    if (keyIsID == false)
        hdr.AddAsciiColumn(QUANTIFICATION_DETECTION_PROBE_SET_NAME, maxProbeSetName);
    else
        hdr.AddIntColumn(QUANTIFICATION_DETECTION_PROBE_SET_ID);
	hdr.AddFloatColumn(QUANTIFICATION_DETECTION_QUANTIFICATION_NAME);
    hdr.AddFloatColumn(QUANTIFICATION_DETECTION_DETECTION_NAME);
}

void CHPQuantificationDetectionData::OpenQuantificationDetectionDataSet()
{
	if (entries == NULL)
	{
		entries = genericData.DataSet(0, 0);
		if (entries)
		{
			entries->Open();
			firstColumnType = entries->Header().GetColumnInfo(0).GetColumnType();
		}
	}
}

std::wstring CHPQuantificationDetectionData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

void CHPQuantificationDetectionData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value, ARRAY_TYPE_MAX_LEN);
}

std::wstring CHPQuantificationDetectionData::GetAlgName()
{
	return GetWStringFromGenericHdr(ALGORITHM_NAME_PARAM_NAME);
}

void CHPQuantificationDetectionData::SetAlgName(const std::wstring& value)
{
	SetWStringToGenericHdr(ALGORITHM_NAME_PARAM_NAME, value);
}

std::wstring CHPQuantificationDetectionData::GetAlgVersion()
{
	return GetWStringFromGenericHdr(ALG_VERSION_PARAM_NAME);
}

void CHPQuantificationDetectionData::SetAlgVersion(const std::wstring& value)
{
	SetWStringToGenericHdr(ALG_VERSION_PARAM_NAME, value);
}

ParameterNameValueTypeList CHPQuantificationDetectionData::GetAlgParams()
{
	ParameterNameValueTypeList nvt;
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

void CHPQuantificationDetectionData::AddAlgParams(const ParameterNameValueTypeList& params)
{
	ParameterNameValueType param;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it != params.end(); it++)
	{
		param = *it;
		param.SetName(ALGORITHM_PARAM_NAME_PREFIX_S + param.GetName());
		hdr->AddNameValParam(param);
	}
}

ParameterNameValueTypeList CHPQuantificationDetectionData::GetSummaryParams()
{
	ParameterNameValueTypeList nvt;
	ParameterNameValueTypeIt begin, end;
	ParameterNameValueType param;

	genericData.Header().GetGenericDataHdr()->GetNameValIterators(begin, end);
	while(begin != end)
	{
		std::wstring key = begin->GetName();
		if(key.compare(0, CHIP_SUMMARY_PARAMETER_NAME_PREFIX.size(),CHIP_SUMMARY_PARAMETER_NAME_PREFIX) == 0)
		{
			param = *begin;
			key.erase(0, CHIP_SUMMARY_PARAMETER_NAME_PREFIX.size());
			param.SetName(key);
			nvt.push_back(param);
		}
		begin++;
	}
	return nvt;
}

void CHPQuantificationDetectionData::AddSummaryParams(const ParameterNameValueTypeList& params)
{
	ParameterNameValueType param;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it != params.end(); it++)
	{
		param = *it;
		param.SetName(CHIP_SUMMARY_PARAMETER_NAME_PREFIX + param.GetName());
		hdr->AddNameValParam(param);
	}
}

std::wstring CHPQuantificationDetectionData::GetWStringFromGenericHdr(const std::wstring& name)
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

void CHPQuantificationDetectionData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value, reserve);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

