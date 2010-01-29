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


#include "calvin_files/data/src/CHPQuantificationData.h"
//
#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/utils/src/StringUtils.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

/*! The group and data set and column name to store the quantification data. */
const static std::wstring QUANTIFICATION_QUANTIFICATION_NAME = L"Quantification";

/*! The column name for the probe set name. */
const static std::wstring QUANTIFICATION_PROBE_SET_NAME = L"ProbeSetName";

/*! The column name for the probe set id. */
const static std::wstring QUANTIFICATION_PROBE_SET_ID = L"ProbeSetId";

CHPQuantificationData::CHPQuantificationData()
{
    maxProbeSetName = -1;
	firstColumnType = UnicodeCharColType;
	entries = NULL;
	Clear();
}

CHPQuantificationData::CHPQuantificationData(const std::string& filename) 
{ 
    maxProbeSetName = -1;
	firstColumnType = UnicodeCharColType;
	entries = NULL;
	Clear();
	SetFilename(filename);
	DataGroupHeader dcHdr(QUANTIFICATION_QUANTIFICATION_NAME);
	genericData.Header().AddDataGroupHdr(dcHdr);
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_QUANTIFICATION_TYPE);
}

CHPQuantificationData::~CHPQuantificationData() 
{ 
	Clear(); 
}

DataSetHeader &CHPQuantificationData::GetDataSetHeader()
{
    return genericData.Header().GetDataGroup(0).GetDataSet(0);
}

void CHPQuantificationData::Clear()
{
	if (entries){ entries->Delete();  entries = 0; }
	genericData.Header().Clear();
}

void CHPQuantificationData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string CHPQuantificationData::GetFilename() const
{
	return ((GenericData&)genericData).Header().GetFilename();
}

int32_t CHPQuantificationData::GetEntryCount()
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr = dcHdr->GetDataSet(0);
	return dpHdr.GetRowCnt();
}

void CHPQuantificationData::SetEntryCount(int32_t ln, int32_t maxln)
{
    firstColumnType = ASCIICharColType;
	maxProbeSetName = maxln;
	ParameterNameValueType param;
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(QUANTIFICATION_QUANTIFICATION_NAME);
	AddColumns(dpHdr, false);
	dcHdr->AddDataSetHdr(dpHdr);
}

void CHPQuantificationData::SetEntryCount(int32_t ln)
{
    firstColumnType = IntColType;
	maxProbeSetName = -1;
	ParameterNameValueType param;
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(QUANTIFICATION_QUANTIFICATION_NAME);
	AddColumns(dpHdr, true);
	dcHdr->AddDataSetHdr(dpHdr);
}

void CHPQuantificationData::GetQuantificationEntry(int index, affymetrix_calvin_data::ProbeSetQuantificationData &entry)
{
	OpenQuantificationDataSet();
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
	}
}

void CHPQuantificationData::AddColumns(DataSetHeader& hdr, bool keyIsID)
{
    if (keyIsID == false)
        hdr.AddAsciiColumn(QUANTIFICATION_PROBE_SET_NAME, maxProbeSetName);
    else
        hdr.AddIntColumn(QUANTIFICATION_PROBE_SET_ID);
	hdr.AddFloatColumn(QUANTIFICATION_QUANTIFICATION_NAME);
}

void CHPQuantificationData::OpenQuantificationDataSet()
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

std::wstring CHPQuantificationData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

void CHPQuantificationData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value, ARRAY_TYPE_MAX_LEN);
}

std::wstring CHPQuantificationData::GetAlgName()
{
	return GetWStringFromGenericHdr(ALGORITHM_NAME_PARAM_NAME);
}

void CHPQuantificationData::SetAlgName(const std::wstring& value)
{
	SetWStringToGenericHdr(ALGORITHM_NAME_PARAM_NAME, value);
}

std::wstring CHPQuantificationData::GetAlgVersion()
{
	return GetWStringFromGenericHdr(ALG_VERSION_PARAM_NAME);
}

void CHPQuantificationData::SetAlgVersion(const std::wstring& value)
{
	SetWStringToGenericHdr(ALG_VERSION_PARAM_NAME, value);
}

ParameterNameValueTypeList CHPQuantificationData::GetAlgParams()
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

void CHPQuantificationData::AddAlgParams(const ParameterNameValueTypeList& params)
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

ParameterNameValueTypeList CHPQuantificationData::GetSummaryParams()
{
	ParameterNameValueTypeList nvt;
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

void CHPQuantificationData::AddSummaryParams(const ParameterNameValueTypeList& params)
{
	ParameterNameValueType param;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it != params.end(); it++)
	{
		param = *it;
		param.SetName(CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S + param.GetName());
		hdr->AddNameValParam(param);
	}
}

std::wstring CHPQuantificationData::GetWStringFromGenericHdr(const std::wstring& name)
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

void CHPQuantificationData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value, reserve);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

