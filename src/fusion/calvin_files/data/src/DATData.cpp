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


#include "calvin_files/data/src/DATData.h"
//
#include "calvin_files/array/src/ArrayId.h"
#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/data/src/GenericDataTypes.h"
//


using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

// Constant column names.
static const std::wstring MinIntensityColName = L"Min Intensity";
static const std::wstring MaxIntensityColName = L"Max Intensity";

DATData::DATData()
{
	globalGridStatus = 0;
	dsPixel = 0;
	dsSMPixel = 0;
	dsStats = 0;
	Clear();
}

DATData::DATData(const std::string &filename) 
{ 
	globalGridStatus = 0;
	dsPixel = 0;
	dsStats = 0;
	dsSMPixel = 0;
	Clear();
	SetFilename(filename);
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(SCAN_ACQUISITION_DATA_TYPE);
	DataGroupHeader dcHdr(DAT_DATAGROUP);
	genericData.Header().AddDataGroupHdr(dcHdr);
}

DATData::~DATData() 
{ 
	Clear(); 
}

void DATData::Clear()
{
	if (dsPixel){ dsPixel->Delete();  dsPixel = 0; }
	if (dsSMPixel){ dsSMPixel->Delete();  dsSMPixel = 0; }
	if (dsStats){ dsStats->Delete(); dsStats = 0; }
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
	dpHdr.AddUShortColumn(DAT_PIXEL);
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
	dpHdr.AddUShortColumn(MinIntensityColName);
	dpHdr.AddUShortColumn(MaxIntensityColName);
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
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value, ARRAY_TYPE_MAX_LEN);
}

/*
 * Get the array id.
 */
AffymetrixGuidType DATData::GetArrayId()
{
	AffymetrixGuidType guid;
	GenericDataHeader *parentGDH = GetParentArrayGenericDataHeader();
	ParameterNameValueType nvt;
	if (parentGDH->FindNameValParam(ARRAY_ID_PARAM_NAME, nvt))
	{
		guid = nvt.GetValueAscii();
	}

	return guid;
}

/*
 * Set the array id.
 */
void DATData::SetArrayId(AffymetrixGuidType& value)
{
	ParameterNameValueType nvt;
	nvt.SetName(ARRAY_ID_PARAM_NAME);
	nvt.SetValueAscii(value, AFFY_GUID_LEN);
	GenericDataHeader *parentGDH = GetParentArrayGenericDataHeader();
	parentGDH->AddNameValParam(nvt);
}

/*
 * Get the array barcode.
 */
std::wstring DATData::GetArrayBarcode()
{
	std::wstring result;
	GenericDataHeader *parentGDH = GetParentArrayGenericDataHeader();
	ParameterNameValueType nvt;
	if (parentGDH->FindNameValParam(ARRAY_BARCODE_PARAM_NAME, nvt))
	{
		result = nvt.GetValueText();
	}

	return result;
}

/*
 * Set the array barcode.
 */
void DATData::SetArrayBarcode(std::wstring& value)
{
	ParameterNameValueType nvt;
	nvt.SetName(ARRAY_BARCODE_PARAM_NAME);
	nvt.SetValueText(value, ARRAY_BARCODE_MAX_LEN);
	GenericDataHeader *parentGDH = GetParentArrayGenericDataHeader();
	parentGDH->AddNameValParam(nvt);
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

std::wstring DATData::GetExposureTime()
{
	return GetWStringFromGenericHdr(PLATE_EXPOSURE_TIME);
}

void DATData::SetExposureTime(std::wstring& value)
{
	SetWStringToGenericHdr(PLATE_EXPOSURE_TIME, value);
}

int32_t DATData::GetPegSubImageIndex()
{
	return GetInt32FromGenericHdr(PLATE_PEG_SUBIMAGE_INDEX);
}

void DATData::SetPegSubImageIndex(int32_t value)
{
	SetInt32ToGenericHdr(PLATE_PEG_SUBIMAGE_INDEX, value);
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

void DATData::AddSubgrid(u_int32_t status, const FRegion& subgrid)
{
	subgridsStatus.push_back(status);
	subgrids.push_back(subgrid);
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

void DATData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value, reserve);
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
	PreparePixelSet();
	if (dsPixel && dsPixel->IsOpen())
	{
		if (rowCnt+startRow <= (u_int32_t)GetRows())
		{
			int32_t startIdx = startRow*GetCols();
			int32_t count = rowCnt*GetCols();
			if (startIdx+count <= dsPixel->Rows())
			{
				int32_t returned = dsPixel->GetDataRaw(0, startIdx, count, pixelBuffer);
				return (count == returned);
			}
		}
	}
	return false;
}

/*
 * Fills the array with requested pixel values.	
 */
bool DATData::GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t startCol, u_int32_t rowCnt, u_int32_t colCnt)
{
	PrepareSmallMemoryPixelSet();
	if (dsSMPixel && dsSMPixel->IsOpen())
	{
		if (rowCnt+startRow <= (u_int32_t)GetRows() && colCnt+startCol <= (u_int32_t)GetCols())
		{
			u_int16_t* pb = pixelBuffer;

			// Copy one row at a time into the buffer.
			for (int32_t row = startRow; row < startRow+rowCnt; ++row)
			{
				int32_t dsRow = row*GetCols() + startCol;
				if (dsRow+colCnt <= dsSMPixel->Rows())
				{
					int32_t returned = dsSMPixel->GetDataRaw(0, dsRow, colCnt, pb);
					if (returned != colCnt)
						return false;

					pb += colCnt;
				}
			}

			return true;
		}
	}
	return false;
}

bool DATData::GetRange(u_int16_t& min, u_int16_t& max)
{
	PrepareStatsSet();
	if (dsStats && dsStats->IsOpen())
	{
		if (dsStats->Rows() > 0)
		{
			dsStats->GetData(0, 0, min);
			dsStats->GetData(0, 1, max);
			return true;
		}
	}
	return false;
}

void DATData::PreparePixelSet()
{
	PrepareSet(DAT_PIXEL, dsPixel);
}

void DATData::PrepareSmallMemoryPixelSet()
{
	genericData.UseMemoryMapping(false);
	genericData.LoadEntireDataSetHint(false);
	PrepareSet(DAT_PIXEL, dsSMPixel);
	genericData.UseMemoryMapping(true);
}

void DATData::PrepareStatsSet()
{
	PrepareSet(DAT_STATS, dsStats);
}

void DATData::PrepareSet(const std::wstring& name, DataSet*& ds)
{
	if (ds == 0)
	{
		ds = genericData.DataSet(DAT_DATAGROUP, name);
		if (ds)
			ds->Open();
	}
}

GenericDataHeader* DATData::GetParentArrayGenericDataHeader()
{
	GenericDataHeader* parentGDH = genericData.Header().GetGenericDataHdr()->FindParent(ARRAY_TYPE_IDENTIFIER);

	if (parentGDH == 0)
	{
		// Create a new parent GenericDataHeader and add to the current GenericDataHeader
		GenericDataHeader gdh;
		gdh.SetFileTypeId(ARRAY_TYPE_IDENTIFIER);
		genericData.Header().GetGenericDataHdr()->AddParent(gdh);
		parentGDH = genericData.Header().GetGenericDataHdr()->FindParent(ARRAY_TYPE_IDENTIFIER);
	}

	return parentGDH;
}

/*
 * Add a grid alignment algorithm parameter.
 */
void DATData::AddGridAlignmentAlgorithmParameter(const ParameterNameValueType& nvt)
{
	gridAlignParams.push_back(nvt);
}

/*
 * Remove all grid alignment algorithm parameters.
 */
void DATData::ClearGridAlignmentAlgorithmParameters()
{
	gridAlignParams.clear();
}

/*
 * Return a grid alignment algorithm parameter given a name.
 */
bool DATData::FindGridAlignmentAlgorithmParameter(const std::wstring& name, ParameterNameValueType& param)
{
	for (ParameterNameValueTypeIt ii = gridAlignParams.begin(); ii != gridAlignParams.end(); ++ii)
	{
		if (name == ii->GetName())
		{
			param = *ii;
			return true;
		}
	}
	return false;
}

/*
 * Return the grid alignment algorithm parameters.
 */
void DATData::GetGridAlignmentAlgorithmParameters(ParameterNameValueTypeVector& algParams)
{
	algParams = gridAlignParams;
}
