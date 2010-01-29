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


#include "calvin_files/data/src/CHPTilingData.h"
//
#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/utils/src/StringUtils.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

// Constant column names.
static const std::wstring GenomicPositionColName = L"Genomic Position";
static const std::wstring ValueColName = L"Result";


CHPTilingData::CHPTilingData()
{
	entries = 0;
	Clear();
}

CHPTilingData::CHPTilingData(const std::string& filename) 
{ 
	entries = 0;
	Clear();
	SetFilename(filename);
	DataGroupHeader dcHdr(CHP_TILING_GROUP);
	genericData.Header().AddDataGroupHdr(dcHdr);
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_TILING_TYPE);
}

void CHPTilingData::GetTilingSequenceEntry(int row, CHPTilingEntry& e)
{
	if (entries && entries->IsOpen())
	{
		entries->GetData(row, 0, e.position);
		entries->GetData(row, 1, e.value);
	}
}

CHPTilingData::~CHPTilingData() 
{ 
	Clear(); 
}

void CHPTilingData::Clear()
{
	if (entries){ entries->Delete();  entries = 0; }
	genericData.Header().Clear();
	cachedNumSequences = -1;
}

void CHPTilingData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string CHPTilingData::GetFilename() const
{
	return ((GenericData&)genericData).Header().GetFilename();
}

int32_t CHPTilingData::GetTilingSequenceEntryCount(int index)
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	DataSetHeader dpHdr = dcHdr->GetDataSet(index);
	return dpHdr.GetRowCnt();
}

void CHPTilingData::AddTilingSequenceData(int32_t numEntries, const TilingSequenceData &data)
{
	ParameterNameValueType param;
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	int nSets = dcHdr->GetDataSetCnt();
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(numEntries);
	wchar_t name[65];
	FormatString1(name, 65, L"%d", nSets);
	dpHdr.SetName(name);

	param.SetName(TILING_SEQ_NAME);
	param.SetValueText(data.name);
	dpHdr.AddNameValParam(param);

	param.SetName(TILING_SEQ_GROUP_NAME);
	param.SetValueText(data.groupName);
	dpHdr.AddNameValParam(param);

	param.SetName(TILING_SEQ_VERSION);
	param.SetValueText(data.version);
	dpHdr.AddNameValParam(param);

	for (ParameterNameValueTypeList::const_iterator it=data.parameters.begin(); it != data.parameters.end(); it++)
	{
		dpHdr.AddNameValParam(*it);
	}

	AddColumns(dpHdr);
	dcHdr->AddDataSetHdr(dpHdr);
}

TilingSequenceData CHPTilingData::GetTilingSequenceData()
{
	ParameterNameValueType param;
	TilingSequenceData data;
	ParameterNameValueTypeConstIt begin;
	ParameterNameValueTypeConstIt end;
	if (entries && entries->IsOpen())
	{
		const DataSetHeader &hdr = entries->Header();
		hdr.GetNameValIterators(begin, end);
		for (ParameterNameValueTypeConstIt it=begin; it != end; it++)
		{
			const ParameterNameValueType &param = *it;
			if (param.GetName() == TILING_SEQ_NAME)
			{
				data.name = param.GetValueText();
			}
			else if (param.GetName() == TILING_SEQ_GROUP_NAME)
			{
				data.groupName = param.GetValueText();
			}
			else if (param.GetName() == TILING_SEQ_VERSION)
			{
				data.version = param.GetValueText();
			}
			else
			{
				data.parameters.push_back(param);
			}
		}
	}
	return data;
}

void CHPTilingData::AddColumns(DataSetHeader& hdr)
{
	hdr.AddUIntColumn(GenomicPositionColName);  //genomic position - unsigned int 32
	hdr.AddFloatColumn(ValueColName); //value - float
}

void CHPTilingData::OpenTilingSequenceDataSet(int index)
{
	if (entries)
		entries->Delete();
	entries = genericData.DataSet(0, index);
	if (entries)
		entries->Open();
}

std::wstring CHPTilingData::GetAlgName()
{
	return GetWStringFromGenericHdr(TILING_ALG_NAME);
}

void CHPTilingData::SetAlgName(const std::wstring& value)
{
	SetWStringToGenericHdr(TILING_ALG_NAME, value);
}

std::wstring CHPTilingData::GetAlgVersion()
{
	return GetWStringFromGenericHdr(TILING_ALG_VERSION);
}

void CHPTilingData::SetAlgVersion(const std::wstring& value)
{
	SetWStringToGenericHdr(TILING_ALG_VERSION, value);
}

int32_t CHPTilingData::GetNumberSequences()
{
	if (cachedNumSequences == -1)
		cachedNumSequences = GetInt32FromGenericHdr(TILING_NUM_SEQS);
	return cachedNumSequences;
}

void CHPTilingData::SetNumberSequences(int32_t value)
{
	SetInt32ToGenericHdr(TILING_NUM_SEQS, value);
	cachedNumSequences = value;
}

ParameterNameValueTypeList CHPTilingData::GetAlgParams()
{
	const int len = (int)TILING_PARAM_SUFFIX.length();
	int index;
	ParameterNameValueType param;
	ParameterNameValueTypeList params;
	ParameterNameValueTypeIt begin;
	ParameterNameValueTypeIt end;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->GetNameValIterators(begin, end);
	for (ParameterNameValueTypeIt it=begin; it != end; it++)
	{
		param = *it;
		index = (int) param.GetName().find(TILING_PARAM_SUFFIX);
		if (index == 0)
		{
			param.SetName( param.GetName().substr(len, param.GetName().length()-len));
			params.push_back(param);
		}
	}
	return params;
}

void CHPTilingData::AddAlgParams(const ParameterNameValueTypeList& params)
{
	ParameterNameValueType param;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it != params.end(); it++)
	{
		param = *it;
		param.SetName(TILING_PARAM_SUFFIX + param.GetName());
		hdr->AddNameValParam(param);
	}
}

std::wstring CHPTilingData::GetWStringFromGenericHdr(const std::wstring& name)
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

void CHPTilingData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

int32_t CHPTilingData::GetInt32FromGenericHdr(const std::wstring& name)
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

void CHPTilingData::SetInt32ToGenericHdr(const std::wstring& name, int32_t value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueInt32(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}


