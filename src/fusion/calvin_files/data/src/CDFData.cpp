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


#include "calvin_files/data/src/CDFData.h"
//
#include "calvin_files/data/src/DataException.h"
#include "calvin_files/data/src/DataSetHeader.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_parameter;

CDFData::CDFData() 
{
	tocDataSet = 0;
	Clear();
}

CDFData::CDFData(const std::string &filename) 
{
	tocDataSet = 0;
	Clear();
	SetFilename(filename);
}
	
CDFData::~CDFData() 
{
	Clear();
}

void CDFData::Clear()
{
	genericData.Header().Clear();
	setTocMetaData = false;
	mode = NoMode;

	curIndex = 0;
	curGroupFilePos = 0;

	if (tocDataSet)
	{
		tocDataSet->Delete();
		tocDataSet = 0;
	}

	nameToFilePosMap.clear();
}

int32_t CDFData::GetFormatVersion()
{
	return 0;
}

void CDFData::SetArrayRows(u_int32_t value)
{
	SetUInt32ToGenericHdr(CDF_ROWS_PARAM, value);
}

u_int32_t CDFData::GetArrayRows()
{
	return GetUInt32FromGenericHdr(CDF_ROWS_PARAM);
}

void CDFData::SetArrayCols(u_int32_t value)
{
	SetUInt32ToGenericHdr(CDF_COLS_PARAM, value);
}

u_int32_t CDFData::GetArrayCols()
{
	return GetUInt32FromGenericHdr(CDF_COLS_PARAM);
}

void CDFData::SetRefSequence(const std::string &seq)
{
	GenericDataHeader* gPtr = genericData.Header().GetGenericDataHdr();
	ParameterNameValueType paramType;
	paramType.SetName(CDF_REFSEQ_PARAM);
	paramType.SetValueAscii(seq);
	gPtr->AddNameValParam(paramType);
}

std::string CDFData::GetRefSequence()
{
	std::string result;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(CDF_REFSEQ_PARAM, paramType))
	{
		result = paramType.GetValueAscii();
	}
	return result;
}

void CDFData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string CDFData::GetFilename()
{
	return genericData.Header().GetFilename();
}

int32_t CDFData::GetProbeSetCnt()
{
	return GetUInt32FromGenericHdr(CDF_SETS_PARAM);
}

void CDFData::SetProbeSetCnt(u_int32_t cnt, CDFDataTypeIds type)
{
	SetDataTypeId(type, cnt);
	SetUInt32ToGenericHdr(CDF_SETS_PARAM, cnt);
}

void CDFData::SetDataTypeId(CDFDataTypeIds p, int32_t probesetCnt)
{
	GenericDataHeader* gPtr = genericData.Header().GetGenericDataHdr();
	std::string typeId;
	std::wstring groupName;
	if(p == Expression)
	{
		typeId = AFFY_EXPR_PS;
		groupName = CDF_PS_GROUP_LBL;
	}
	else if(p == Genotyping)
	{
		typeId = AFFY_GENO_PS;
		groupName = CDF_PS_GROUP_LBL;
	}
	else if(p == Tag)
	{
		typeId = AFFY_TAG_PS;
		groupName = CDF_PS_GROUP_LBL;
	}
	else if(p == Resequencing)
	{
		typeId = AFFY_RESEQ_PS;
		groupName = CDF_PS_GROUP_LBL;
	}
	else 
	{
		typeId = AFFY_CNTRL_PS;
		groupName = CDF_QC_GROUP_LBL;
	}
	gPtr->SetFileTypeId(typeId);
	CreateDataGroups(groupName, probesetCnt);
}

std::string CDFData::GetDataTypeId()
{
	GenericDataHeader* gPtr = genericData.Header().GetGenericDataHdr();
	return gPtr->GetFileTypeId();
}

void CDFData::CreateDataGroups(const std::wstring& p, int32_t probesetCnt)
{
	CreateContentsGroup(p, probesetCnt);
	CreateProbeSetGroups(probesetCnt);
}

void CDFData::CreateContentsGroup(const std::wstring& p, int32_t probesetCnt)
{
	DataGroupHeader c(p);
	DataSetHeader dp;
	dp.SetRowCnt(probesetCnt);
	dp.SetName(p);
	dp.AddUnicodeColumn(L"", MAX_CDF_PROBE_SET_NAME_LENGTH);
	dp.AddIntColumn(L"");
	c.AddDataSetHdr(dp);
	genericData.Header().AddDataGroupHdr(c);
}

void CDFData::CreateProbeSetGroups(int32_t probesetCnt)
{
	for(int i = 0; i < probesetCnt; i++)
	{
		DataGroupHeader c;
		genericData.Header().AddDataGroupHdr(c);
	}
}

u_int32_t CDFData::GetUInt32FromGenericHdr(const std::wstring& name)
{
	int32_t result = 0;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(name, paramType))
	{
		result = paramType.GetValueUInt32();
	}
	return result;
}

void CDFData::SetUInt32ToGenericHdr(const std::wstring& name, u_int32_t value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueUInt32(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

void CDFData::PrepareForSequentialAccess()
{
	// set the mode
	mode = SequentialMode;

	// skip-over the first DataGroup
	DataGroup dg = genericData.DataGroup(genericData.Header().GetFirstDataGroupFilePos());
	curGroupFilePos = dg.Header().GetNextGroupPos();

	// initialize the curPos
	curIndex = 0;
}

void CDFData::PrepareForAccessByProbeSetIndex()
{
	// set the mode
	mode = ProbeSetIndexMode;

	// Open the first DataGroup
	DataGroup dg = genericData.DataGroup(genericData.Header().GetFirstDataGroupFilePos());

	// Open the first DataSet
	if (tocDataSet)
		tocDataSet->Delete();
	tocDataSet = dg.DataSet(0);
	if (tocDataSet)
		tocDataSet->Open();
}

void CDFData::PrepareForAccessByProbeSetName()
{
	// set the mode
	mode = ProbeSetNameMode;

	// Open the first DataGroup
	DataGroup dg = genericData.DataGroup(genericData.Header().GetFirstDataGroupFilePos());

	// Open the first DataSet
	if (tocDataSet)
		tocDataSet->Delete();
	tocDataSet = dg.DataSet(0);
	if (tocDataSet)
	{
		if (tocDataSet->Open())
		{

			// build the name-file position map
			int32_t rows = tocDataSet->Rows();

			std::wstring probeSetName;
			u_int32_t filePos = 0;

			for (int32_t row = 0; row < rows; ++row)
			{
				tocDataSet->GetData(row, TOCProbeSetNameCol, probeSetName);
				tocDataSet->GetData(row, TOCFilePosCol, filePos);

				nameToFilePosMap[probeSetName] = filePos;
			}
		}
	}
}

/*
 */
void CDFData::GetProbeSetInformation(int32_t index, CDFProbeSetInformation& info)
{
	if (GetDataTypeId() == AFFY_CNTRL_PS)
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (index >= genericData.Header().GetNumDataGroups()-1)
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (mode == SequentialMode)
	{
		if (index == curIndex)
		{
			DataGroup dg = genericData.DataGroup(curGroupFilePos);
			info.SetDataGroup(dg);

			// Move to the next data group
			++curIndex;
			curGroupFilePos = dg.Header().GetNextGroupPos();
		}
		else
		{
			CDFAccessNotSupportedByModeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
	}
	else if (mode == ProbeSetIndexMode)
	{
		u_int32_t filePos = GetTOCFilePosByIndex(index);
		DataGroup dg = genericData.DataGroup(filePos);
		info.SetDataGroup(dg);
	}
	else
	{
		CDFAccessNotSupportedByModeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void CDFData::GetProbeSetInformation(const std::wstring& probeSetName, CDFProbeSetInformation& info)
{
	if (GetDataTypeId() == AFFY_CNTRL_PS)
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (mode != ProbeSetNameMode)
	{
		CDFAccessNotSupportedByModeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	u_int32_t filePos = GetTOCFilePosByName(probeSetName);
	DataGroup dg = genericData.DataGroup(filePos);
	info.SetDataGroup(dg);
}


void CDFData::GetQCProbeSetInformation(int32_t index, CDFQCProbeSetInformation& info)
{
	if (GetDataTypeId() != AFFY_CNTRL_PS)
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (index >= genericData.Header().GetNumDataGroups()-1)
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (mode == SequentialMode)
	{
		if (index == curIndex)
		{
			DataGroup dg = genericData.DataGroup(curGroupFilePos);
			DataSet* ds = dg.DataSet(0);
			info.SetDataSet(ds);

			// Move to the next data group
			++curIndex;
			curGroupFilePos = dg.Header().GetNextGroupPos();
		}
		else
		{
			CDFAccessNotSupportedByModeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
	}
	else if (mode == ProbeSetIndexMode)
	{
		u_int32_t filePos = GetTOCFilePosByIndex(index);
		DataGroup dg = genericData.DataGroup(filePos);
		DataSet* ds = dg.DataSet(0);
		info.SetDataSet(ds);
	}
	else
	{
		CDFAccessNotSupportedByModeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void CDFData::GetQCProbeSetInformation(const std::wstring& probeSetName, CDFQCProbeSetInformation& info)
{
	if (GetDataTypeId() != AFFY_CNTRL_PS)
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (mode != ProbeSetNameMode)
	{
		CDFAccessNotSupportedByModeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	u_int32_t filePos = GetTOCFilePosByName(probeSetName);
	DataGroup dg = genericData.DataGroup(filePos);
	DataSet* ds = dg.DataSet(0);
	info.SetDataSet(ds);
}

u_int32_t CDFData::GetTOCFilePosByIndex(int32_t index)
{
	if (tocDataSet == 0 || tocDataSet->IsOpen() == false)
	{
		DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	u_int32_t filePos = (u_int32_t)-1;
	tocDataSet->GetData(index, TOCFilePosCol, filePos);
	return filePos;
}

u_int32_t CDFData::GetTOCFilePosByName(const std::wstring& name)
{
	if (tocDataSet == 0 || tocDataSet->IsOpen() == false)
	{
		DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	std::map<std::wstring, u_int32_t>::iterator ii = nameToFilePosMap.find(name);
	if (ii == nameToFilePosMap.end())
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
	else
		return ii->second;
}

std::wstring CDFData::GetProbeSetName(int32_t index)
{
	// Access the TOC data set
	if (tocDataSet == 0)
	{
		DataGroup dg = genericData.DataGroup(genericData.Header().GetFirstDataGroupFilePos());
		tocDataSet = dg.DataSet(0);

		if (tocDataSet == 0)
		{
			DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
	}
	// Open the TOC data set if not open
	if (tocDataSet->IsOpen() == false)
	{
		tocDataSet->Open();
		if (tocDataSet->IsOpen() == false)
		{
			DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
	}
	
	if (index < 0 || index >= tocDataSet->Rows())
	{
		ProbeSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	std::wstring probeSetName;
	tocDataSet->GetData(index, TOCProbeSetNameCol, probeSetName);
	return probeSetName;
}
