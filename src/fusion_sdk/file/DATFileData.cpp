////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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

//
#include "file/DATFileData.h"
//
#include "file/FileIO.h"
#include "file/FileWriter.h"
//
#include <cstring>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

using namespace affxdat;
using namespace std;

/*! The DAT file magic number. */
#define DAT_FILE_MAGIC_NUMBER 0xFC

/*! The size of the header. */
#define DAT_FILE_HEADER_SIZE 512

//////////////////////////////////////////////////////////////////////

CDATFileHeaderData::~CDATFileHeaderData()
{
}

//////////////////////////////////////////////////////////////////////

CDATFileHeaderData::CDATFileHeaderData()
{
	m_nCols = 0;
	m_nRows = 0;
	m_MinValue = (uint16_t) -1;
	m_MaxValue = 0;
}

//////////////////////////////////////////////////////////////////////

bool CDATFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_FileName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

bool CDATFileData::Read()
{
	// First close the previously opened file.
	Close();

	// Open the file.
	m_File.open(m_FileName.c_str(), ios::in|ios::binary);
	if (!m_File)
	{
		return false;
	}

	// Check if new type.
	uint8_t magic=0;
	ReadUInt8(m_File, magic);
	if (magic != DAT_FILE_MAGIC_NUMBER)
	{
		return false;
	}

	// Read the rows and columns.
	uint16_t cols;
	uint16_t rows;
	ReadUInt16_I(m_File, cols);
	ReadUInt16_I(m_File, rows);
	m_HeaderData.SetRows(rows);
	m_HeaderData.SetCols(cols);

	// The total number of pixels.
	uint32_t total;
	ReadUInt32_I(m_File, total);

	// Read the min/max values
	uint32_t min;
	uint32_t max;
	ReadUInt32_I(m_File, min);
	ReadUInt32_I(m_File, max);
	m_HeaderData.SetMinValue(min);
	m_HeaderData.SetMaxValue(max);

	// Skip the unused entries
	m_File.seekg(2*sizeof(double) + 9 + 9 + 7 + 7 + 6 + 7 + 4 + 18, ios::cur);

	// Get the next item from the DAT file. This should include the scanner ID
	// and the array type.
	char buf[221];
	m_File.read(buf, 220);

	int len=0;
	char *endIndex = strstr(buf, ".1sq");
	if (endIndex != NULL)
	{
		int index = (int)(strlen(buf) - strlen(endIndex));
		while (index >= 0 && buf[index] != ' ')
		{
			++len;
			--index;
		}
		char *arrayType = &buf[index+1];
		arrayType[len-1] = 0;
		m_HeaderData.SetArrayType(arrayType);

		index=0;
		while (buf[index] != ' ')
			++index;
		buf[index] = 0;
		m_HeaderData.SetScannerID(buf);
	}


	// Skip more data
	m_File.seekg(2*sizeof(double) + sizeof(int32_t), ios::cur);

	// Now get the grid.
	int16_t coord;
	GridCoordinatesType grid;

	ReadInt16_I(m_File, coord);
	grid.upperleft.x = (int32_t) coord;
	ReadInt16_I(m_File, coord);
	grid.upperleft.y = (int32_t) coord;

	ReadInt16_I(m_File, coord);
	grid.upperright.x = (int32_t) coord;
	ReadInt16_I(m_File, coord);
	grid.upperright.y = (int32_t) coord;

	ReadInt16_I(m_File, coord);
	grid.lowerright.x = (int32_t) coord;
	ReadInt16_I(m_File, coord);
	grid.lowerright.y = (int32_t) coord;

	ReadInt16_I(m_File, coord);
	grid.lowerleft.x = (int32_t) coord;
	ReadInt16_I(m_File, coord);
	grid.lowerleft.y = (int32_t) coord;

	m_HeaderData.SetGridCorners(grid);

	return true;
}

//////////////////////////////////////////////////////////////////////

uint16_t CDATFileData::GetPixel(int x, int y)
{
	uint16_t pixel;
	int32_t offset = DAT_FILE_HEADER_SIZE + (y*m_HeaderData.GetCols() + x)*sizeof(uint16_t);
	m_File.seekg(offset, ios::beg);
	ReadUInt16_I(m_File, pixel);
	return pixel;
}

////////////////////////////////////////////////////////////////////////

void CDATFileData::GetPixels(int rowIndex, uint16_t *pixels)
{
	int cols = m_HeaderData.GetCols();
	uint16_t pixel;
	int32_t offset = DAT_FILE_HEADER_SIZE + (rowIndex*cols*sizeof(uint16_t));
	m_File.seekg(offset, ios::beg);
	m_File.read((char *)pixels, sizeof(uint16_t) * cols);
	for (int i=0; i<cols; i++)
	{
		pixel = MmGetUInt16_I(&pixels[i]);
		pixels[i] = pixel;
	}
}

////////////////////////////////////////////////////////////////////////

void CDATFileData::GetPixels(int rowIndex, int rowCount, uint16_t *pixels)
{
	int cols = m_HeaderData.GetCols();
	uint16_t pixel;
	int32_t offset = DAT_FILE_HEADER_SIZE + (rowIndex*cols*sizeof(uint16_t));
	m_File.seekg(offset, ios::beg);
	int npixels=cols*rowCount;
	m_File.read((char *)pixels, sizeof(uint16_t) * npixels);
	for (int i=0; i<npixels; i++)
	{
		pixel = MmGetUInt16_I(&pixels[i]);
		pixels[i] = pixel;
	}
}

////////////////////////////////////////////////////////////////////////

CDATFileData::CDATFileData()
{
}

//////////////////////////////////////////////////////////////////////

CDATFileData::~CDATFileData()
{
	Close();
}

//////////////////////////////////////////////////////////////////////

void CDATFileData::Close()
{
	if (m_File.is_open())
		m_File.close();
}

//////////////////////////////////////////////////////////////////////

bool CDATFileData::IsGCOSDATFile(const char *datFile)
{
	// Open the file, check the magic number.
	ifstream instr(datFile, ios::in|ios::binary);
	uint8_t magic=0;
	ReadUInt8(instr, magic);
	instr.close();
	return (magic == DAT_FILE_MAGIC_NUMBER);
}

//////////////////////////////////////////////////////////////////////

bool CDATFileData::UpdateGridCorners(const char *datFile, const GridCoordinatesType &grid)
{
	// First check if it is a GCOS DAT file.
	if (IsGCOSDATFile(datFile) == false)
		return false;

	// Open the file and seek to the coordinates.
	ofstream outstr(datFile, ios::in|ios::out|ios::binary);
	if (!outstr)
	{
		return false;
	}
	int offset = sizeof(uint8_t) + 2*sizeof(uint16_t) + 3*sizeof(uint32_t) + 
		2*sizeof(double) + 9 + 9 + 7 + 7 + 6 + 7 + 4 + 18 + 220 +
		2*sizeof(double) + sizeof(int32_t);
	outstr.seekp(offset, ios::beg);

	// Now update the grid coordinates.
	WriteInt16_I(outstr, (int16_t)grid.upperleft.x);
	WriteInt16_I(outstr, (int16_t)grid.upperleft.y);

	WriteInt16_I(outstr, (int16_t)grid.upperright.x);
	WriteInt16_I(outstr, (int16_t)grid.upperright.y);

	WriteInt16_I(outstr, (int16_t)grid.lowerright.x);
	WriteInt16_I(outstr, (int16_t)grid.lowerright.y);

	WriteInt16_I(outstr, (int16_t)grid.lowerleft.x);
	WriteInt16_I(outstr, (int16_t)grid.lowerleft.y);

	outstr.close();

	return true;
}

//////////////////////////////////////////////////////////////////////
