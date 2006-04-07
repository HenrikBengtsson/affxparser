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

#include "CHPSignalData.h"
#include "DataSetHeader.h"
#include "StringUtils.h"

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

/*! A prefix for chip summary parameter ids. */
static const std::wstring CHIP_SUMMARY_PARAMETER_NAME_PREFIX = L"affymetrix-chipsummary-";

CHPSignalData::CHPSignalData()
{
	wideProbeSetNames = false;
	entries = NULL;
	Clear();
}

CHPSignalData::CHPSignalData(const std::string& filename) 
{ 
	wideProbeSetNames = false;
	entries = NULL;
	Clear();
	SetFilename(filename);
	DataGroupHeader dcHdr(SIGNAL_SIGNAL_NAME);
	genericData.Header().AddDataGroupHdr(dcHdr);
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_SIGNAL_TYPE);
}

CHPSignalData::~CHPSignalData() 
{ 
	Clear(); 
}

void CHPSignalData::Clear()
{
	if (entries){ entries->Delete();  entries = 0; }
	genericData.Header().Clear();
}

void CHPSignalData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string CHPSignalData::GetFilename() const
{
	return ((GenericData&)genericData).Header().GetFilename();
}

int32_t CHPSignalData::GetEntryCount()
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr = dcHdr->GetDataSet(0);
	return dpHdr.GetRowCnt();
}

void CHPSignalData::SetEntryCount(int32_t ln, int32_t maxln)
{
	maxProbeSetName = maxln;
	ParameterNameValueType param;
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(SIGNAL_SIGNAL_NAME);
	AddColumns(dpHdr);
	dcHdr->AddDataSetHdr(dpHdr);
}

void CHPSignalData::GetSignalEntry(int index, affymetrix_calvin_data::ProbeSetSignalData &entry)
{
	OpenSignalDataSet();
	if (entries && entries->IsOpen())
	{
		if (wideProbeSetNames == false)
			entries->GetData(index, 0, entry.name);
		else
		{
			std::wstring wprobeSetName;
			entries->GetData(index, 0, wprobeSetName);
			 entry.name = StringUtils::ConvertWCSToMBS(wprobeSetName);
		}
		entries->GetData(index, 1, entry.signal);
	}
}

void CHPSignalData::AddColumns(DataSetHeader& hdr)
{
	hdr.AddAsciiColumn(SIGNAL_PROBE_SET_NAME, maxProbeSetName);
	hdr.AddFloatColumn(SIGNAL_SIGNAL_NAME);
}

void CHPSignalData::OpenSignalDataSet()
{
	if (entries == NULL)
	{
		entries = genericData.DataSet(0, 0);
		if (entries)
		{
			entries->Open();
			wideProbeSetNames = (entries->Header().GetColumnInfo(0).GetColumnType() == UnicodeCharColType);
		}
	}
}

std::wstring CHPSignalData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

void CHPSignalData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value, ARRAY_TYPE_MAX_LEN);
}

std::wstring CHPSignalData::GetAlgName()
{
	return GetWStringFromGenericHdr(ALGORITHM_NAME_PARAM_NAME);
}

void CHPSignalData::SetAlgName(const std::wstring& value)
{
	SetWStringToGenericHdr(ALGORITHM_NAME_PARAM_NAME, value);
}

std::wstring CHPSignalData::GetAlgVersion()
{
	return GetWStringFromGenericHdr(ALG_VERSION_PARAM_NAME);
}

void CHPSignalData::SetAlgVersion(const std::wstring& value)
{
	SetWStringToGenericHdr(ALG_VERSION_PARAM_NAME, value);
}

ParameterNameValueTypeList CHPSignalData::GetAlgParams()
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

void CHPSignalData::AddAlgParams(const ParameterNameValueTypeList& params)
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

ParameterNameValueTypeList CHPSignalData::GetSummaryParams()
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

void CHPSignalData::AddSummaryParams(const ParameterNameValueTypeList& params)
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

std::wstring CHPSignalData::GetWStringFromGenericHdr(const std::wstring& name)
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

void CHPSignalData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value, reserve);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

