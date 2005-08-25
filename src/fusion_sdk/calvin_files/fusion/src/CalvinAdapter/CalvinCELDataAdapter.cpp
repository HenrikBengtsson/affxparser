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


#include "CalvinCELDataAdapter.h"
#include "CelFileReader.h"

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;

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
	catch(InvalidFileTypeException&)
	{
		return false;
	}

	return true;
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

unsigned int CalvinCELDataAdapter::GetNumOutliers()
{
	XYCoordVector coords;
	calvinCel.GetOutlierCoords(coords);
	return (int)coords.size();
}

unsigned int CalvinCELDataAdapter::GetNumMasked()
{
	XYCoordVector coords;
	calvinCel.GetMaskedCoords(coords);
	return (int)coords.size();
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
	FloatVector v;
	calvinCel.GetIntensities(index, 1, v);
	return v.at(0);
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

/* Replaced based on sugestions from Luis
bool CalvinCELDataAdapter::IsMasked(int x, int y)
{
	BoolVector v;
	int32_t index = XYToIndex(x, y);
	calvinCel.GetMasked(index, 1, v);
	return v.at(0);
}
bool CalvinCELDataAdapter::IsMasked(int index)
{
	BoolVector v;
	calvinCel.GetMasked(index, 1, v);
	return v.at(0);
}
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

bool CalvinCELDataAdapter::IsMasked(int index)
{
      BoolVector v;
      if (calvinCel.GetMasked(index, 1, v))
            return v.at(0);
      else
            return false;
}

/* Replaced based on sugestions from Luis
bool CalvinCELDataAdapter::IsOutlier(int x, int y)
{
	BoolVector v;
	int32_t index = XYToIndex(x, y);
	calvinCel.GetOutliers(index, 1, v);
	return v.at(0);
}
bool CalvinCELDataAdapter::IsOutlier(int index)
{
	BoolVector v;
	calvinCel.GetOutliers(index, 1, v);
	return v.at(0);
}
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
 */
void CalvinCELDataAdapter::SetDimensions(int rows, int cols)
{
	calvinCel.SetRows(rows);
	calvinCel.SetCols(cols);
}

/*
 */
void CalvinCELDataAdapter::AddAlgorithmParameter(const wchar_t *tag, const wchar_t *value)
{
	ParameterNameValueType nvt;
	nvt.SetName(tag);
	nvt.SetValueText(value);
	calvinCel.AddAlgorithmParameter(nvt);
}
