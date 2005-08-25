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

#include "CELData.h"
#include "DataSetHeader.h"
#include "ExceptionBase.h"

#include <iostream>

using namespace std;


using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;

static const std::wstring CelRowsLabel = L"affymetrix-calvin-cel-rows";
static const std::wstring CelColsLabel = L"affymetrix-calvin-cel-cols";
/*! The attribute name of the cel file version number */
static const std::wstring CelFileVersionNumberName = L"affymetrix-cel-file-version";

/*
 * Default constructor
 */
CelFileData::CelFileData()
{
	dpInten = 0;
	dpStdev = 0;
	dpPixels = 0;
	Clear();
}

/*
 * Constructor
 */
CelFileData::CelFileData(const std::string &filename) 
{
	dpInten = 0;
	dpStdev = 0;
	dpPixels = 0;

	Clear();
	SetFilename(filename);
	DataGroupHeader dcHdr(CelDataGroupName);
	genericData.Header().AddDataGroupHdr(dcHdr);
}

/*
 * Destructor
 */
CelFileData::~CelFileData() 
{
	Clear(); 
}

/*
 * Clear the object members.
 */
void CelFileData::Clear()
{
	genericData.Header().Clear();
	setIntensityMetaData = false;
	setStdDevMetaData = false;
	setPixelMetaData = false;
	setOutlierMetaData = false;
	setMaskMetaData = false;

	if (dpInten){ dpInten->Delete(); dpInten = 0; }
	if (dpStdev) { dpStdev->Delete(); dpStdev = 0; }
	if (dpPixels) { dpPixels->Delete(); dpPixels = 0; }
	outlierPlaneRead = false;
	outliers.clear();
	maskPlaneRead = false;
	masked.clear();
	cachedRows = -1;
	cachedCols = -1;
}

/*
 * Set file name
 */
void CelFileData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

/*
 * Get file name
 */
std::string CelFileData::GetFilename() const
{
	return ((GenericData&)genericData).Header().GetFilename();
}

void CelFileData::SetIntensityCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CelIntensityLabel);
	dpHdr.AddFloatColumnType();
	if(setIntensityMetaData)
	{
		UpdateDataSetRowCount(dpHdr);
	}
	else
	{
		InsertDataSetHeader(dpHdr);
		setIntensityMetaData = true;
	}
}

void CelFileData::SetStdDevCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CelStdDevLabel);
	dpHdr.AddFloatColumnType();
	if(setStdDevMetaData)
	{
		UpdateDataSetRowCount(dpHdr);
	}
	else
	{
		InsertDataSetHeader(dpHdr);
		setStdDevMetaData = true;
	}
}

void CelFileData::SetPixelCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CelPixelLabel);
	dpHdr.AddShortColumnType();
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

void CelFileData::SetOutlierCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CelOutlierLabel);
	dpHdr.AddShortColumnType();
	dpHdr.AddShortColumnType();
	if(setOutlierMetaData)
	{
		UpdateDataSetRowCount(dpHdr);
	}
	else
	{
		InsertDataSetHeader(dpHdr);
		setOutlierMetaData = true;
	}
}

void CelFileData::SetMaskCount(int32_t ln)
{
	DataSetHeader dpHdr;
	dpHdr.SetRowCnt(ln);
	dpHdr.SetName(CelMaskLabel);
	dpHdr.AddShortColumnType();
	dpHdr.AddShortColumnType();
	if(setMaskMetaData)
	{
		UpdateDataSetRowCount(dpHdr);
	}
	else
	{
		InsertDataSetHeader(dpHdr);
		setMaskMetaData = true;
	}
}

void CelFileData::UpdateDataSetRowCount(const DataSetHeader &hdr)
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

void CelFileData::InsertDataSetHeader(const DataSetHeader &hdr)
{
	DataGroupHeader* dcHdr = &genericData.Header().GetDataGroup(0);
	dcHdr->AddDataSetHdr(hdr);
}

/*
 * Return the number of cells on the array.
 */
int32_t CelFileData::GetNumCells()
{
	int32_t rows = 0;
	try
	{
		PrepareIntensityPlane();
		if (dpInten)
		{
			rows = dpInten->Rows();
		}
	}
	catch(CalvinException&)
	{
	}
	return rows;
}

/*
 * Prepare to read intensity data
 */
void CelFileData::PrepareIntensityPlane()
{
	if (dpInten == 0)
	{
		dpInten = genericData.DataSet(CelDataGroupName, CelIntensityLabel);
		if (dpInten)
			dpInten->Open();
	}
}

/*
 * Prepare to read the standard deviation data
 */
void CelFileData::PrepareStdevPlane()
{
	if (dpStdev == 0)
	{
		dpStdev = genericData.DataSet(CelDataGroupName, CelStdDevLabel);
		if (dpStdev)
			dpStdev->Open();
	}
}

/*
 * Prepare to read the number of pixel data
 */
void CelFileData::PrepareNumPixelPlane()
{
	if (dpPixels == 0)
	{
		dpPixels = genericData.DataSet(CelDataGroupName, CelPixelLabel);
		if (dpPixels)
			dpPixels->Open();
	}
}

/*
 * Prepare to read the outlier data
 */
void CelFileData::PrepareOutlierPlane()
{
	if (outlierPlaneRead)
		return;

	cout << "Reading Outlier Data..." << endl;
	outlierPlaneRead = true;	// Read attempted

	DataSet* dpOutlier = genericData.DataSet(CelDataGroupName, CelOutlierLabel);
	if (dpOutlier)
	{
		if (dpOutlier->Open())
		{
			int32_t rows = dpOutlier->Rows();

			for (int32_t row = 0; row < rows; ++row)
			{
				int16_t x = 0, y = 0;
				dpOutlier->GetData(row, 0, x);
				dpOutlier->GetData(row, 1, y);
				XYCoord xy(x,y);
				outliers.insert(xy);
			}
		}
		dpOutlier->Delete();
	}
}

/*
 * Prepare to read the mask data
 */
void CelFileData::PrepareMaskedPlane()
{
	if (maskPlaneRead)
		return;

	cout << "Reading Masked Data..." << endl;

	maskPlaneRead = true;	// Read attempted

	DataSet* dpMasked = genericData.DataSet(CelDataGroupName, CelMaskLabel);
	if (dpMasked)
	{
		if (dpMasked->Open())
		{
			int32_t rows = dpMasked->Rows();

			for (int32_t row = 0; row < rows; ++row)
			{
				int16_t x = 0, y = 0;
				dpMasked->GetData(row, 0, x);
				dpMasked->GetData(row, 1, y);
				XYCoord xy(x,y);
				masked.insert(xy);
			}
		}
		dpMasked->Delete();
	}
}

/*
 * Prepare to read all data
 */
void CelFileData::PrepareAllPlanes()
{
	PrepareIntensityPlane();
	PrepareStdevPlane();
	PrepareNumPixelPlane();
	PrepareOutlierPlane();
	PrepareMaskedPlane();
}

/*
 * Get the cell intensity, standard deviation, number of pixels, outlier flag and mask flag.
 */
void CelFileData::GetData(int32_t cellIdx, float& intensity, float& stdev, int16_t& numPixels, bool& outlier, bool& masked)
{
	PrepareAllPlanes();

	// Index checking is done in the Generic layer

	if (dpInten)
	{
		// if out-of-bounds, forward the exception
		dpInten->GetData(cellIdx, 0, intensity);
	}

	if (dpStdev)
	{
		try
		{
			stdev = 0.0f;
			if (dpStdev)
			{
				if (cellIdx < dpStdev->Rows())
					dpStdev->GetData(cellIdx, 0, stdev);
			}
		}
		catch(CalvinException&)
		{
			stdev = 0.0f;
		}
	}

	if (dpPixels)
	{
		try
		{
			numPixels = 0;
			if (dpPixels)
			{
				if (cellIdx < dpPixels->Rows())
					dpPixels->GetData(cellIdx, 0, numPixels);
			}
		}
		catch(CalvinException&)
		{
			numPixels = 0;
		}
	}

	outlier = IsOutlier(cellIdx);
	masked = IsMasked(cellIdx);
}

/*
 * Get the intensities for a range of cell indexes.
 */
bool CelFileData::GetIntensities(int32_t cellIdxStart, int32_t count, FloatVector& values)
{
	PrepareIntensityPlane();
	if (dpInten && dpInten->IsOpen())
	{
		dpInten->GetData(0, cellIdxStart, count, values);
		return (count == values.size());
	}
	return false;
}

/*
 * Get the standard deviations for a range of cell indexes.
 */
bool CelFileData::GetStdev(int32_t cellIdxStart, int32_t count, FloatVector& values)
{
	PrepareStdevPlane();
	if (dpStdev && dpStdev->IsOpen())
	{
		dpStdev->GetData(0, cellIdxStart, count, values);
		return (count == values.size());
	}
	return false;
}

/*
 * Get the number of pixels for a range of cell indexes.
 */
bool CelFileData::GetNumPixels(int32_t cellIdxStart, int32_t count, Int16Vector& values)
{
	PrepareNumPixelPlane();
	if (dpPixels && dpPixels->IsOpen())
	{
		dpPixels->GetData(0, cellIdxStart, count, values);
		return (count == values.size());
	}
	return false;
}

/*
 * Get the outlier flags for a range of cell indexes.
 */
bool CelFileData::GetOutliers(int32_t cellIdxStart, int32_t count, BoolVector& values)
{
	PrepareOutlierPlane();
	if (outliers.empty())
		return false;

	int32_t lastCell = cellIdxStart+count;
	if (lastCell > GetNumCells())
		lastCell = GetNumCells();
	for (int32_t cellIdx = cellIdxStart; cellIdx < lastCell; ++cellIdx)
		values.push_back(IsOutlier(cellIdx));

	return true;
}

/*
 * Get the mask flags for a range of cell indexes.
 */
bool CelFileData::GetMasked(int32_t cellIdxStart, int32_t count, BoolVector& values)
{
	PrepareMaskedPlane();
	if (masked.empty())
		return false;

	int32_t lastCell = cellIdxStart+count;
	if (lastCell > GetNumCells())
		lastCell = GetNumCells();
	for (int32_t cellIdx = cellIdxStart; cellIdx < lastCell; ++cellIdx)
		values.push_back(IsMasked(cellIdx));

	return true;
}

/*
 * Get the coordinates of all outlier cells (i.e. outlier flag is true).
 */
void CelFileData::GetOutlierCoords(XYCoordVector& coords)
{
  cout << "in function GetOutlierCoords." << endl;

	std::set<XYCoord>::iterator begin = outliers.begin();
	std::set<XYCoord>::iterator end = outliers.end();
	for (std::set<XYCoord>::iterator ii = outliers.begin(); ii != outliers.end(); ++ii)
	{
	  cout << "adding coordinate: " << ii->xCoord << endl;
	  
		XYCoord xy(ii->xCoord, ii->yCoord);
		coords.push_back(xy);
	}
}

/*
 * Get the coordinates of all masked cells (i.e. mask flag is true).
 */
void CelFileData::GetMaskedCoords(XYCoordVector& coords)
{
	std::set<XYCoord>::iterator begin = masked.begin();
	std::set<XYCoord>::iterator end = masked.end();
	for (std::set<XYCoord>::iterator ii = masked.begin(); ii != masked.end(); ++ii)
	{
		XYCoord xy(ii->xCoord, ii->yCoord);
		coords.push_back(xy);
	}
}

/*
 * Indicates whether there are standard deviation values.
 */
bool CelFileData::HasStdev()
{
	DataSetHeader* dph = FindDataSetHeader(CelStdDevLabel);
	if (dph)
		return (dph->GetRowCnt() > 0);
	return false;
}

/*
 * Indicates whether there are number of pixel values.
 */
bool CelFileData::HasNumPixels()
{
	DataSetHeader* dph = FindDataSetHeader(CelPixelLabel);
	if (dph)
		return (dph->GetRowCnt() > 0);
	return false;
}

/*
 * Set the version of the CEL file
 */
void CelFileData::SetVersion(u_int8_t value)
{
	ParameterNameValueType paramType;
	paramType.SetName(CelFileVersionNumberName);
	paramType.SetValueUInt8(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

/*
 * Get the version of the CEL file
 */
u_int8_t CelFileData::GetVersion()
{
	u_int8_t result = 0;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(CelFileVersionNumberName, paramType))
	{
		result = paramType.GetValueUInt8();
	}
	return result;
}

/*
 * Set the array type
 */
void CelFileData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value);
}

/*
 * Get the array type
 */
std::wstring CelFileData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

/*
 * Set name of the algorithm used to generate the results.
 */
void CelFileData::SetAlgorithmName(const std::wstring& value)
{
	SetWStringToGenericHdr(ALGORITHM_NAME_PARAM_NAME, value);
}

/*
 * Get the name of the algorithm used to generate the results.
 */
std::wstring CelFileData::GetAlgorithmName()
{
	return GetWStringFromGenericHdr(ALGORITHM_NAME_PARAM_NAME);
}

/*
 * Set the number of rows of cells
 */
void CelFileData::SetRows(int32_t value)
{
	SetInt32ToGenericHdrParameterList(CelRowsLabel, value);
	cachedRows = value;
}

/*
 * Get the number of rows of cells.
 */
int32_t CelFileData::GetRows()
{
	if (cachedRows == -1)
		cachedRows = GetInt32FromGenericHdrParameterList(CelRowsLabel);
	return cachedRows;
}

/*
 * Set the number of cols of cells
 */
void CelFileData::SetCols(int32_t value)
{
	SetInt32ToGenericHdrParameterList(CelColsLabel, value);
	cachedCols = value;
}

/*
 * Get the number of cols of cells
 */
int32_t CelFileData::GetCols()
{
	if (cachedCols)
		cachedCols = GetInt32FromGenericHdrParameterList(CelColsLabel);
	return cachedCols;
}

/*  TODO consider throwing an exception */
/*
 * Read an int32_t value from the GenericDataHeader parameter list.
 */
int32_t CelFileData::GetInt32FromGenericHdrParameterList(const std::wstring& name)
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

/*
 * Set a int32_t value into the GenericDataHeader parameter list.
 */
void CelFileData::SetInt32ToGenericHdrParameterList(const std::wstring& name, int32_t value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueInt32(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

/*
 * Read an wstring value from the GenericDataHeader parameter list.
 */
std::wstring CelFileData::GetWStringFromGenericHdr(const std::wstring& name)
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

/*
 * Set a wstring value into the GenericDataHeader parameter list.
 */
void CelFileData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

/*
 * Determines if the cell at a given index is an outlier.
 * Assumes that the outlier DataSet has been read
 */
bool CelFileData::IsOutlier(int32_t cellIdx)
{
	int16_t x, y;
	ComputeXY(cellIdx, x, y);
	XYCoord xy(x,y);
	return (outliers.find(xy) != outliers.end());
}
/*
 * Determines if the cell at a given index is masked.
 * Assumes that the masked DataSet has been read
 */
bool CelFileData::IsMasked(int32_t cellIdx)
{
	int16_t x, y;
	ComputeXY(cellIdx, x, y);
	XYCoord xy(x,y);
	return (masked.find(xy) != masked.end());
}

/*
 * Determine the x-y coordinate given a cell index.
 */
void CelFileData::ComputeXY(int32_t cellIdx, int16_t& x, int16_t& y)
{
	y = (int16_t)(cellIdx/GetCols());
	x = (int16_t)(cellIdx - GetCols()*y);
}

/*
 * Find a DataSetHeader by name.
 */
DataSetHeader* CelFileData::FindDataSetHeader(const std::wstring& name)
{
	DataGroupHeader* dch = genericData.FindDataGroupHeader(CelDataGroupName);
	if (dch)
	{
		DataSetHeader* dph = genericData.FindDataSetHeader(dch, name);
		if (dph)
			return dph;
	}
	return 0;
}

/*
 * Return the algorithm parameters.  The algorithm parameter prefix is removed from the name.
 */
void CelFileData::GetAlgorithmParameters(ParameterNameValueTypeVector& algParams)
{
	// Get all parameters that start with the algorithm parameter prefix
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->GetNameValParamsBeginsWith(ALGORITHM_PARAM_NAME_PREFIX, algParams);

	// Remove the prefix from the name
	int32_t len = (int32_t)wcslen(ALGORITHM_PARAM_NAME_PREFIX);
	for (ParameterNameValueTypeIt ii = algParams.begin(); ii != algParams.end(); ++ii)
	{
		std::wstring s = ii->GetName();
		s.erase(0, len);
		ii->SetName(s);
	}
}

/*
 * Add an algorithm parameter.  The algorithm parameter prefix will be added by the method.
 */
void CelFileData::AddAlgorithmParameter(ParameterNameValueType& nvt)
{
	std::wstring s = nvt.GetName();
	s.insert(0, ALGORITHM_PARAM_NAME_PREFIX);
	nvt.SetName(s);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(nvt);
}

/*
 * Return an algorithm parameter given a name.
 */
bool CelFileData::FindAlgorithmParameter(const std::wstring& name, ParameterNameValueType& param)
{
	std::wstring paramName = name;
	paramName.insert(0, ALGORITHM_PARAM_NAME_PREFIX);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(paramName, param))
	{
		int32_t len = (int32_t)wcslen(ALGORITHM_PARAM_NAME_PREFIX);
		std::wstring s = param.GetName();
		s.erase(0, len);
		param.SetName(s);
		return true;
	}
	return false;
}
