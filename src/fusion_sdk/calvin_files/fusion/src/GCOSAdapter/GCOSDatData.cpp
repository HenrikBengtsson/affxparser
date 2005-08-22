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

#include "GCOSDatData.h"
#include "GCOSParameterNames.h"
#include <algorithm>

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_exceptions;

GCOSScanParameters::GCOSScanParameters()
{
	pixelSize = 0.0f;
	orientation = Top;
	filter = 0;
}

/*
 * Sets the pixel size in microns
 */
void GCOSScanParameters::SetPixelSize(float value)
{
	if (value <= 0.0f)
	{
		OutOfRangeParameterException e(L"0.0", L"");
		throw e;
	}
	pixelSize = value;
}

/*
 * Sets the scanner filter 
 */
void GCOSScanParameters::SetFilter(int32_t value)
{
	if (value <= 0)
	{
		OutOfRangeParameterException e(L"0", L"");
		throw e;
	}
	filter = value;;
}

GCOSGridParameters::GCOSGridParameters()
{
}

bool GCOSGridParameters::GetSubgrid(int32_t index, affymetrix_calvin_utilities::Subgrid& subgrid)
{
	if (index < 0 || (u_int32_t)index >= subgrids.size())
		return false;
	subgrid = subgrids[index];
	return true;
}


GCOSImageParameters::GCOSImageParameters()
{
	minData = 0;
	maxData = 0;
	avgData = 0.0f;
	stdev = 0.0f;
	rows = 0;
	cols = 0;
	flipped = true;
}

/*
 * Sets the number of rows of pixels in the image
 */
void GCOSImageParameters::SetRows(int32_t value)
{
	if (value <= 0)
	{
		OutOfRangeParameterException e(L"0", L"");
		throw e;
	}
	rows = value;
}

/*
 * Sets the number of columns of pixels in the image
 */
void GCOSImageParameters::SetCols(int32_t value)
{
	if (value <= 0)
	{
		OutOfRangeParameterException e(L"0", L"");
		throw e;
	}
	cols = value;
}

GCOSDatData::GCOSDatData()
{
	version = 0;
}

/*
	* Allocates a array of hold pixel values
	*/
u_int16_t* GCOSDatData::AllocPixelBuffer(u_int32_t rowCount)
{
	return new u_int16_t[rowCount*imageParams.GetCols()];
}

/*
 * Fills the array with pixel values
 */
bool GCOSDatData::GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t rowCnt)
{
	// Go to the ImageData object. - TODO
	return false;
}

/*
 * Releases the allocated buffer
 */
void GCOSDatData::DeallocPixelBuffer(u_int16_t* pixelBuffer)
{
	delete [] pixelBuffer;
}

/*
 * Adds a parameter to the parameter list. If the parameter is already in the list then the value is updated.
 */
void GCOSDatData::AddParameter(const ParameterNameValueType& value)
{
	CheckParameterTypeAndRange(value);

	ParameterNameValueTypeIt ii = FindParameter(value);
	if (ii == params.end())
	{	// Add
		params.push_back(value);
	}
	else
	{	// Update
		*ii = value;
	}
}

/*
 * Finds a parameter from the parameter list by name.
 */
bool GCOSDatData::FindParameter(const std::wstring& name, ParameterNameValueType& found)
{
	ParameterNameValueTypeIt begin, end;
	begin = params.begin();
	end = params.end();

	affymetrix_calvin_parameter::ParameterNameValueType nvt;
	nvt.SetName(name);

	ParameterNameValueTypeIt ii = std::find(begin, end, nvt);
	if (ii != end)
	{
		found = *ii;
	}
	else
		return false;
	return true;
}

/*
 * Return an iterator referencing the found parameter.
 */
ParameterNameValueTypeIt GCOSDatData::FindParameter(const ParameterNameValueType& nvt)
{
	ParameterNameValueTypeIt begin, end;
	begin = params.begin();
	end = params.end();

	ParameterNameValueTypeIt ii = std::find(begin, end, nvt);
	if (ii != end)
	{
		return ii;
	}
	else
		return end;
}

/*
 * Gets parameter list iterators
 */
void GCOSDatData::GetParameterIts(ParameterNameValueTypeConstIt& begin, ParameterNameValueTypeConstIt& end)
{
	begin = params.begin();
	end = params.end();
}

/*
 * Clears (empties) the parameters list
 */
void GCOSDatData::ClearParameters()
{
	params.clear();
}

void GCOSDatData::CheckParameterTypeAndRange(const ParameterNameValueType& value)
{
	std::wstring name = value.GetName();
	ParameterNameValueType::ParameterType type = value.GetParameterType();

	if (name == ARCRADIUS_NAME)
	{
		if (type != ParameterNameValueType::FloatType)
		{
			UnexpectedParameterException e(L"float");
			throw e;
		}
	}
	else if (name == LASER_SPOTSIZE_NAME)
	{
		if (type != ParameterNameValueType::FloatType)
		{
			UnexpectedParameterException e(L"float");
			throw e;
		}
	}
	else if (name == EXPERIMENT_NAME_NAME)
	{
		if (type != ParameterNameValueType::TextType)
		{
			UnexpectedParameterException e(L"wstring");
			throw e;
		}
	}
}
