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



#include "calvin_files/fusion/src/CalvinAdapter/CalvinCELDataAdapter.h"
//
#include "calvin_files/data/src/GenericDataTypes.h"
#include "calvin_files/parameter/src/CELAlgorithmParameterNames.h"
#include "calvin_files/parsers/src/CelFileReader.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include <cstdlib>
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;

/*
 * Constructor
 */
CalvinCELDataAdapter::CalvinCELDataAdapter()
{
}

/*
 * Destructor
 */
CalvinCELDataAdapter::~CalvinCELDataAdapter()
{
	calvinCel.Clear();
}

/*
 * Can this object read the file
 */
bool CalvinCELDataAdapter::CanReadFile()
{
	CelFileReader reader;
	try
	{
		reader.Read(calvinCel);
	}
	catch(...)
	{
		return false;
	}

	return true;
}

/*
 * Return the generic data object.
 */
GenericData *CalvinCELDataAdapter::GetGenericData()
{
	return &calvinCel.GetGenericData();
}

/*
 */
void CalvinCELDataAdapter::SetFileName(const std::string& value)
{
	calvinCel.SetFilename(value);
}

/*
 */
std::string CalvinCELDataAdapter::GetFileName() const
{
	return calvinCel.GetFilename();
}

/*
 */
AffymetrixGuidType CalvinCELDataAdapter::GetFileId()
{
	return calvinCel.GetFileHeader()->GetGenericDataHdr()->GetFileId();
}

/*
 */
std::wstring CalvinCELDataAdapter::GetParams()
{
	std::wstring params;
	ParameterNameValueTypeVector algParams;
	calvinCel.GetAlgorithmParameters(algParams);
	for (ParameterNameValueTypeIt ii = algParams.begin(); ii!= algParams.end(); ++ii)
	{
		if (ii != algParams.begin())
			params += L";";
		params += ii->GetName();
		params += L":";
		params += ii->ToString();
	}
	return params;
}

/*
 */
std::wstring CalvinCELDataAdapter::GetAlgorithmParameter(const wchar_t *tag)
{
	std::wstring result;
	ParameterNameValueType param;
	if (calvinCel.FindAlgorithmParameter(tag, param))
	{
		result = param.ToString();
	}
	return result;
}

/*
 */
std::wstring CalvinCELDataAdapter::GetAlgorithmParameterTag(int index)
{
	ParameterNameValueTypeVector algParams;
	calvinCel.GetAlgorithmParameters(algParams);

	std::wstring result;
	try
	{
		result = algParams.at(index).GetName();
	}
	catch(...)
	{
	}
	return result;
}

/*
 */
int CalvinCELDataAdapter::GetNumberAlgorithmParameters()
{
	ParameterNameValueTypeVector algParams;
	calvinCel.GetAlgorithmParameters(algParams);
	return (int)algParams.size();
}

/*
 */
std::wstring CalvinCELDataAdapter::GetAlgorithmParameters()
{
	return GetParams();
}

/*
 */
void CalvinCELDataAdapter::GetParameters(FusionTagValuePairTypeList& values)
{
	ParameterNameValueTypeVector algParams;
	calvinCel.GetAlgorithmParameters(algParams);
	for (ParameterNameValueTypeIt ii = algParams.begin(); ii!= algParams.end(); ++ii)
	{
		FusionTagValuePairType ft;
		ft.Tag = ii->GetName();
		ft.Value = ii->ToString();
		ft.DetailedType() = *ii;
		values.push_back(ft);
	}
}

/*
 */
std::wstring CalvinCELDataAdapter::GetDatHeader()
{
	std::wstring datHeader;

	//GenDataHdrVectorIt begin, end; 
	GenericDataHeader* gdh = calvinCel.GetFileHeader()->GetGenericDataHdr()->FindParent(SCAN_ACQUISITION_DATA_TYPE);
	if (gdh)
	{
		// found the right header, now look for the parameter
		ParameterNameValueType nvt;
		if (gdh->FindNameValParam(DAT_HEADER_PARAM_NAME, nvt))
		{
			if (nvt.GetParameterType() == ParameterNameValueType::TextType)
				datHeader = nvt.GetValueText();
		}
		else if (gdh->FindNameValParam(PARTIAL_DAT_HEADER_PARAM_NAME, nvt))
		{
			if (nvt.GetParameterType() == ParameterNameValueType::TextType)
			{
				std::wstring partialDatHeader = nvt.GetValueText();

				u_int16_t min = 0;
				u_int16_t max = 0;

				// Find the max and min parameters and append to the string.
				if (gdh->FindNameValParam(MAX_PIXEL_INTENSITY_PARAM_NAME, nvt))
				{
					if (nvt.GetParameterType() == ParameterNameValueType::UInt16Type)
						max = nvt.GetValueUInt16();
				}

				if (gdh->FindNameValParam(MIN_PIXEL_INTENSITY_PARAM_NAME, nvt))
				{
					if (nvt.GetParameterType() == ParameterNameValueType::UInt16Type)
						min = nvt.GetValueUInt16();
				}

				wchar_t buf[30]=L"";
				FormatString2(buf, 30, L"[%d..%d]", min, max);
				datHeader = buf;
				datHeader += partialDatHeader;
			}
		}
	}
	return datHeader;
}

/*
 */
int CalvinCELDataAdapter::GetCellMargin()
{
	ParameterNameValueType nvt;
	if (calvinCel.FindAlgorithmParameter(CELLMARGIN_PARAM_NAME, nvt))
	{
		switch(nvt.GetParameterType())
		{
		case ParameterNameValueType::Int32Type:
			return nvt.GetValueInt32();
			break;
		case ParameterNameValueType::Int16Type:
			return (int)nvt.GetValueInt16();
			break;
		case ParameterNameValueType::Int8Type:
			return (int)nvt.GetValueInt8();
			break;
		case ParameterNameValueType::UInt32Type:
			return (int)nvt.GetValueUInt32();
			break;
		case ParameterNameValueType::UInt16Type:
			return (int)nvt.GetValueUInt16();
			break;
		case ParameterNameValueType::UInt8Type:
			return (int)nvt.GetValueUInt8();
			break;
		case ParameterNameValueType::AsciiType:
			return (int)atoi(nvt.GetValueAscii().c_str());
		default:
			return 0;
			break;
		}
	}
	else
	{
		return 0;
	}
}

unsigned int CalvinCELDataAdapter::GetNumOutliers()
{
	XYCoordVector coords;
	calvinCel.GetOutlierCoords(coords);
	return (unsigned int)coords.size();
}

unsigned int CalvinCELDataAdapter::GetNumMasked()
{
	XYCoordVector coords;
	calvinCel.GetMaskedCoords(coords);
	return (unsigned int)coords.size();
}

/*
 * Get the grid coordinates.
 */
affymetrix_fusion_io::FGridCoords CalvinCELDataAdapter::GetGridCorners()
{
	affymetrix_fusion_io::FGridCoords zeroGrid;
	affymetrix_fusion_io::FGridCoords grid;
	ParameterNameValueType nvt;

	if (calvinCel.FindAlgorithmParameter(GRIDULX_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.upperleft.x = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}

	if (calvinCel.FindAlgorithmParameter(GRIDULY_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.upperleft.y = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}

	if (calvinCel.FindAlgorithmParameter(GRIDURX_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.upperright.x = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}

	if (calvinCel.FindAlgorithmParameter(GRIDURY_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.upperright.y = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}

	if (calvinCel.FindAlgorithmParameter(GRIDLRX_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.lowerright.x = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}

	if (calvinCel.FindAlgorithmParameter(GRIDLRY_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.lowerright.y = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}

	if (calvinCel.FindAlgorithmParameter(GRIDLLX_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.lowerleft.x = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}

	if (calvinCel.FindAlgorithmParameter(GRIDLLY_PARAM_NAME, nvt) && nvt.GetParameterType() == ParameterNameValueType::FloatType)
	{
		grid.lowerleft.y = nvt.GetValueFloat();
	}
	else
	{
		return zeroGrid;
	}
	return grid;

}

// Index/position conversions
/*
 */
int CalvinCELDataAdapter::IndexToX(int index)
{
	return index % calvinCel.GetCols();
}

/*
 */
int CalvinCELDataAdapter::IndexToY(int index)
{
	return index / calvinCel.GetCols();
}

/*
 */
int CalvinCELDataAdapter::XYToIndex(int x, int y)
{
	return y*calvinCel.GetCols() + x;
}

// Accessors for intensity information.
/*
 */
void CalvinCELDataAdapter::GetEntry(int index, FusionCELFileEntryType &entry)
{
	float intensity;
	float stdev;
	int16_t numPixels; 
	bool outlier, masked;
	calvinCel.GetData(index, intensity, stdev, numPixels, outlier, masked);

	entry.Intensity = (float)intensity;
	entry.Stdv = stdev;
	entry.Pixels = numPixels;
}

/*
 */
void CalvinCELDataAdapter::GetEntry(int x, int y, FusionCELFileEntryType &entry)
{
	int index = XYToIndex(x,y);

	float intensity;
	float stdev;
	int16_t numPixels; 
	bool outlier, masked;
	calvinCel.GetData(index, intensity, stdev, numPixels, outlier, masked);

	entry.Intensity = (float)intensity;
	entry.Stdv = stdev;
	entry.Pixels = numPixels;
}

/*
 */
float CalvinCELDataAdapter::GetIntensity(int index)
{
  // allocate a vector, fill it with one item
	FloatVector v;
	calvinCel.GetIntensities(index, 1, v);
  // and return that one item.
	return v.at(0);
}

/*
 */
int CalvinCELDataAdapter::GetIntensities(int index,std::vector<float>& intensities)
{
  // Pass the vector along to get filled.
	calvinCel.GetIntensities(index, intensities.size(), intensities);
  // no errors for now.
  return 0;
}

/*
 */
float CalvinCELDataAdapter::GetIntensity(int x, int y)
{
	int index = XYToIndex(x,y);
	FloatVector v;
	calvinCel.GetIntensities(index, 1, v);
	return v.at(0);
}

/*
 */
float CalvinCELDataAdapter::GetStdv(int index)
{
	FloatVector v;
	calvinCel.GetStdev(index, 1, v);
	return v.at(0);
}

/*
 */
float CalvinCELDataAdapter::GetStdv(int x, int y)
{
	int index = XYToIndex(x,y);
	FloatVector v;
	calvinCel.GetStdev(index, 1, v);
	return v.at(0);
}

/*
 */
short CalvinCELDataAdapter::GetPixels(int index)
{
	Int16Vector v;
	calvinCel.GetNumPixels(index, 1, v);
	return v.at(0);
}

/*
 */
short CalvinCELDataAdapter::GetPixels(int x, int y)
{
	int index = XYToIndex(x,y);
	Int16Vector v;
	calvinCel.GetNumPixels(index, 1, v);
	return v.at(0);
}

// Accessors for the mask/outlier flags
/*
 */
bool CalvinCELDataAdapter::IsMasked(int x, int y)
{
	BoolVector v;
	int32_t index = XYToIndex(x, y);
	if (calvinCel.GetMasked(index, 1, v))
		return v.at(0);
	else
		return false;
}

/*
 */
bool CalvinCELDataAdapter::IsMasked(int index)
{
	BoolVector v;
	if (calvinCel.GetMasked(index, 1, v))
		return v.at(0);
	else
		return false;
}

/*
 */
bool CalvinCELDataAdapter::IsOutlier(int x, int y)
{
	BoolVector v;
	int32_t index = XYToIndex(x, y);
	if (calvinCel.GetOutliers(index, 1, v))
		return v.at(0);
	else
		return false;
}

/*
 */
bool CalvinCELDataAdapter::IsOutlier(int index)
{
	BoolVector v;
	if (calvinCel.GetOutliers(index, 1, v))
		return v.at(0);
	else
		return false;
}


bool CalvinCELDataAdapter::ReadHeader()
{
	CelFileReader reader;
	try
	{
		reader.Read(calvinCel);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/*
 */
bool CalvinCELDataAdapter::Read(bool)
{
	CelFileReader reader;
	try
	{
		reader.Read(calvinCel);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/*
 */
bool CalvinCELDataAdapter::ReadEx(const char *filename, int /*state*/)
{
	calvinCel.SetFilename(filename);
	return Read(false);
}

/*
 * Returns the list of parameters associated with a data set
 */
ParameterNameValueTypeList CalvinCELDataAdapter::GetDataSetParameters(const std::wstring &setName)
{
	return calvinCel.GetDataSetParameters(setName);
}

/* Sets the active data group for a multi-group CEL file. Default is the first group. */
void CalvinCELDataAdapter::SetActiveDataGroup(const std::wstring &groupName)
{
	calvinCel.SetActiveChannel(groupName);
}

/* Is this a multi-color CEL file? */
bool CalvinCELDataAdapter::IsMultiColor()
{
	return calvinCel.IsMultiColor();
}

/* Returns a list of the channel (ie data group) names */
WStringVector CalvinCELDataAdapter::GetChannels()
{
	return calvinCel.GetChannels();
}

/*
 */
//void CalvinCELDataAdapter::SetDimensions(int rows, int cols)
//{
//	calvinCel.SetRows(rows);
//	calvinCel.SetCols(cols);
//}

/*
 */
//void CalvinCELDataAdapter::AddAlgorithmParameter(const wchar_t *tag, const wchar_t *value)
//{
//	ParameterNameValueType nvt;
//	nvt.SetName(tag);
//	nvt.SetValueText(value);
//	calvinCel.AddAlgorithmParameter(nvt);
//}
