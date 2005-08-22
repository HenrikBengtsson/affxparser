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

#include "stdafx.h"	
#include "GCOSDatFileWriter.h"
#include "FileWriteException.h"
#include "StringUtils.h"
#include "GCOSDatData.h"
#include "GCOSParameterNames.h"
#include <algorithm>

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_exceptions;

/*
 * Constructor - accepts a reference to the FusionDatData object to be written
 */
GCOSDatFileWriter::GCOSDatFileWriter()
{
	datData = 0;
	isOpen = false;
	mapOffset = 0;
	minData = 0;
	maxData = 0;
	avgData = 0.0f;
	stdev = 0.0f;
}

GCOSDatFileWriter::~GCOSDatFileWriter()
{
	// Just in case
	Close();
}

/*
 * Writes the min and max pixel intensities to the file
 */
void GCOSDatFileWriter::SetMinMaxData(u_int16_t min, u_int16_t max)
{
	if (min > max)
	{
		// min < max
		wchar_t buf[10];
		swprintf(buf, L"%u", max);
		OutOfRangeParameterException e(L"", buf);
		throw e;
	}
	minData = min;
	maxData = max;
}

bool GCOSDatFileWriter::Open(FusionDatData& data)
{
	if (isOpen)
		Close();
	datData = &data;
	isOpen = OpenFile();
	return true;
}

bool GCOSDatFileWriter::Close()
{
	if (!isOpen)
		return true;

	WriteCompleteHeader();
	datio.Close();
	isOpen = false;
	datData = 0;
	return true;
}

/*
 * Maps the pixel data for access starting at firstRow and continuing for rowsToMap
 */
void GCOSDatFileWriter::MapData(int32_t firstRow, int32_t rowsToMap)
{
	if (datData == 0)
		return;

	datio.MapData(firstRow, rowsToMap);
}

/*
 * Get a pointer to the pixel data that has been mapped for access
 */
u_int16_t* GCOSDatFileWriter::GetMappedPixelDataPtr()
{
	if (datio.IsMappedFile() == FALSE)
		return 0;

	return datio.GetDataPtr();
}

/*
 * Get the first row that has been mapped
 */
int32_t GCOSDatFileWriter::GetFirstRowMapped()
{
	if (datio.IsMappedFile() == FALSE)
		return 0;
	return datio.FirstLineMapped();
}

/*
 * Get the number of rows that have been mapped.
 */
int32_t GCOSDatFileWriter::GetRowsMapped()
{
	if (datio.IsMappedFile() == FALSE)
		return 0;
	return datio.LinesMapped();
}

/*
 * Get the maximum number of rows that can be mapped.
 */
int32_t GCOSDatFileWriter::GetMaxRowsToMap()
{
	if (datData == 0)
		return 0;
	return MAXFILEMAPSIZE / (datData->ImageParams().GetCols() * sizeof(WORD));
}

bool GCOSDatFileWriter::OpenFile()
{
	if (datData->ImageParams().GetCols() == 0 ||
			datData->ImageParams().GetRows() == 0)
	{
		affymetrix_calvin_exceptions::FileCreateException e;
		throw e;
	}

	mapOffset = 0;

	int32_t linesMapped = GetMaxRowsToMap();
	if (linesMapped > datData->ImageParams().GetRows())
		linesMapped = datData->ImageParams().GetRows();

	datio.SetSize(datData->ImageParams().GetRows(), datData->ImageParams().GetCols());
	datio.SetName(datData->GetFileName().c_str());
	if (datio.Open(WRITE, ALLOWREAD) == FALSE ||
			datio.WriteHeader() == FALSE ||
			datio.MapData(0, linesMapped) == FALSE)
	{
		affymetrix_calvin_exceptions::FileCreateException e;
		throw e;
	}
	return true;
}

bool GCOSDatFileWriter::WriteCompleteHeader()
{
	if (datData == 0)
		return false;

	datio.minData = (WORD)minData;
	datio.maxData = (WORD)maxData;
	datio.meanData = avgData;
	datio.stddev = stdev;

	datio.PixelSize = (int)(datData->ScanParams().GetPixelSize());
	datio.Orientation = GetOrientation();
	datio.SetScanDate(GetDate().c_str(), GetTime().c_str());
	datio.SetScannerID(StringUtils::ConvertWCSToMBS(datData->ScanParams().GetScannerID()).c_str());	// is this safe?
	datio.SetScannerType(StringUtils::ConvertWCSToMBS(datData->ScanParams().GetScannerType()).c_str());
	datio.SetChipType(StringUtils::ConvertWCSToMBS(datData->GetArrayType()).c_str());
	datio.fPixelSize = datData->ScanParams().GetPixelSize();
	datio.fArcRadius = GetArcRadius();
	datio.fLaserSpotSize = GetLaserSpotSize();
	datio.Filter = (int)datData->ScanParams().GetFilter();
	datio.SetExperimentName(GetExperimentName().c_str());
	
	// Update the header
	return (datio.WriteHeader() == TRUE);
}

int GCOSDatFileWriter::GetOrientation()
{
	return datData->ScanParams().GetOrientation() == Top ? 3 : 6;	// ???
}

std::string GCOSDatFileWriter::GetDate()
{
	std::string date = StringUtils::ConvertWCSToMBS(datData->ScanParams().GetScanDate().Date().c_str());
	int year, month, day;
	if (sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day) == 3)
	{
		char dateBuf[10];
		sprintf(dateBuf, "%02d/%02d/%02d", month, day, year%100);
		return dateBuf;
	}
	return "";
}

std::string GCOSDatFileWriter::GetTime()
{
	return StringUtils::ConvertWCSToMBS(datData->ScanParams().GetScanDate().Time().c_str());
}

double GCOSDatFileWriter::GetArcRadius()
{
	ParameterNameValueType nvt;
	if (datData->FindParameter(ARCRADIUS_NAME, nvt))
	{
		try
		{
			return nvt.GetValueFloat();
		}
		catch(affymetrix_calvin_exceptions::ParameterMismatchException&)
		{	// ignore for now
		}
	}
	return 0.0;
}

double GCOSDatFileWriter::GetLaserSpotSize()
{
	ParameterNameValueType nvt;
	if (datData->FindParameter(LASER_SPOTSIZE_NAME, nvt))
	{
		try
		{
			return nvt.GetValueFloat();
		}
		catch(affymetrix_calvin_exceptions::ParameterMismatchException&)
		{	// ignore for now
		}
	}
	return 0.0;
}

std::string GCOSDatFileWriter::GetExperimentName()
{
	ParameterNameValueType nvt;
	if (datData->FindParameter(EXPERIMENT_NAME_NAME, nvt))
	{
		try
		{
			return StringUtils::ConvertWCSToMBS(nvt.GetValueText());
		}
		catch(affymetrix_calvin_exceptions::ParameterMismatchException&)
		{	// ignore for now
		}
	}
	return "";
}