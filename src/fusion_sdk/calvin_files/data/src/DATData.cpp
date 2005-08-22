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

#include "DATData.h"
#include "DataSetHeader.h"


using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

static const std::wstring PIXEL_SIZE_PARAM_NAME = L"affymetrix-pixel-size";
static const std::wstring SCANNER_TYPE_PARAM_NAME = L"affymetrix-scanner-type";
static const std::wstring SCANNER_ID_PARAM_NAME = L"affymetrix-scanner-id";
static const std::wstring SCAN_DATE_PARAM_NAME = L"affymetrix-scan-date";
static const std::wstring ROWS_PARAM_NAME = L"affymetrix-pixel-rows";
static const std::wstring COLS_PARAM_NAME = L"affymetrix-pixel-cols";

DATData::DATData()
{
	dpPixel = 0;
	dpStats = 0;
	Clear();
}

DATData::DATData(const std::string &filename) 
{ 
	dpPixel = 0;
	dpStats = 0;
	Clear();
	SetFilename(filename);
	DataGroupHeader dcHdr(DAT_DATAGROUP);
	genericData.Header().AddDataGroupHdr(dcHdr);
}

DATData::~DATData() 
{ 
	Clear(); 
}

void DATData::Clear()
{
	if (dpPixel){ dpPixel->Delete();  dpPixel = 0; }
	if (dpStats){ dpStats->Delete(); dpStats = 0; }
	globalGrid.Clear();
	subgrids.clear();

	genericData.Header().Clear();
	setPixelMetaData = false;
	setStatsMetaData = false;
	cachedRows = -1;
	cachedCols = -1;
}

void DATData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string DATData::GetFilename()
{
	return genericData.Header().GetFilename();
}

void DATData::SetPixelCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(DAT_PIXEL);
	dpHdr.AddUShortColumnType();
	if(setPixelMetaData)
	{
		UpdateDataSetRowCount(dpHdr);
	}
	else
	{
		InsertDataSetHeader(dpHdr);
		setPixelMetaData = true;
	}
}

void DATData::SetStatsCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(DAT_STATS);
	dpHdr.AddShortColumnType();
	dpHdr.AddShortColumnType();
	if(setStatsMetaData)
	{
		UpdateDataSetRowCount(dpHdr);
	}
	else
	{
		InsertDataSetHeader(dpHdr);
		setStatsMetaData = true;
	}
}

void DATData::UpdateDataSetRowCount(const DataSetHeader &hdr)
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	bool found = false;
	int sz = dcHdr->GetDataSetCnt();
	for(int i = 0; i < sz; i++)
	{
		DataSetHeader* dpHdr = &dcHdr->GetDataSet(i);
		if(dpHdr->GetName() == hdr.GetName())
		{
			dpHdr->SetRowCnt(hdr.GetRowCnt());
			found = true;
			break;
		}
	}
	if(!found)
	{
		InsertDataSetHeader(hdr);
	}
}

void DATData::InsertDataSetHeader(const DataSetHeader &hdr)
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	dcHdr->AddDataSetHdr(hdr);
}

std::wstring DATData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

void DATData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value);
}

float DATData::GetPixelSize()
{
	float result = 0.0f;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(PIXEL_SIZE_PARAM_NAME, paramType))
	{
		result = paramType.GetValueFloat();
	}
	return result;
}

void DATData::SetPixelSize(float value)
{
	ParameterNameValueType paramType;
	paramType.SetName(PIXEL_SIZE_PARAM_NAME);
	paramType.SetValueFloat(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

std::wstring DATData::GetScannerType()
{
	return GetWStringFromGenericHdr(SCANNER_TYPE_PARAM_NAME);
}

void DATData::SetScannerType(const std::wstring& value)
{
	SetWStringToGenericHdr(SCANNER_TYPE_PARAM_NAME, value);
}

std::wstring DATData::GetScannerID()
{
	return GetWStringFromGenericHdr(SCANNER_ID_PARAM_NAME);
}

void DATData::SetScannerID(const std::wstring& value)
{
	SetWStringToGenericHdr(SCANNER_ID_PARAM_NAME, value);
}

affymetrix_calvin_utilities::DateTime DATData::GetScanDate()
{
	std::wstring dateString = GetWStringFromGenericHdr(SCAN_DATE_PARAM_NAME);
	DateTime dt = DateTime::Parse(dateString);
	return dt;
}

void DATData::SetScanDate(affymetrix_calvin_utilities::DateTime value)
{
	std::wstring dateString = value.ToString();
	SetWStringToGenericHdr(SCAN_DATE_PARAM_NAME, dateString);
}

int32_t DATData::GetRows()
{
	if (cachedRows == -1)
		cachedRows = GetInt32FromGenericHdr(ROWS_PARAM_NAME);
	return cachedRows;
}

void DATData::SetRows(int32_t value)
{
	SetInt32ToGenericHdr(ROWS_PARAM_NAME, value);
	cachedCols = value;
}

int32_t DATData::GetCols()
{
	if (cachedCols == -1)
		cachedCols = GetInt32FromGenericHdr(COLS_PARAM_NAME);
	return cachedCols;
}

void DATData::SetCols(int32_t value)
{
	SetInt32ToGenericHdr(COLS_PARAM_NAME, value);
	cachedCols = value;
}

std::wstring DATData::GetWStringFromGenericHdr(const std::wstring& name)
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

void DATData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

int32_t DATData::GetInt32FromGenericHdr(const std::wstring& name)
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

void DATData::SetInt32ToGenericHdr(const std::wstring& name, int32_t value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueInt32(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

/*
 * Fills the array with pixel values
*/
bool DATData::GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t rowCnt)
{
	PreparePixelPlane();
	if (dpPixel && dpPixel->IsOpen())
	{
		if (rowCnt+startRow <= (u_int32_t)GetRows())
		{
			int32_t startIdx = startRow*GetCols();
			int32_t count = rowCnt*GetCols();
			if (startIdx+count <= dpPixel->Rows())
			{
				int32_t returned = dpPixel->GetDataRaw(0, startIdx, count, pixelBuffer);
				return (count == returned);
			}
		}
	}
	return false;
}

bool DATData::GetRange(u_int16_t& min, u_int16_t& max)
{
	PrepareStatsPlane();
	if (dpStats && dpStats->IsOpen())
	{
		if (dpStats->Rows() > 0)
		{
			dpStats->GetData(0, 0, min);
			dpStats->GetData(0, 1, max);
			return true;
		}
	}
	return false;
}

void DATData::PreparePixelPlane()
{
	PreparePlane(DAT_PIXEL, dpPixel);
}

void DATData::PrepareStatsPlane()
{
	PreparePlane(DAT_STATS, dpStats);
}

void DATData::PreparePlane(const std::wstring& name, DataSet*& dp)
{
	if (dp == 0)
	{
		dp = genericData.DataSet(DAT_DATAGROUP, name);
		if (dp)
			dp->Open();
	}
}