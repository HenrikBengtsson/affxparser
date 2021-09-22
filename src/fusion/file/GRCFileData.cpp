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

#include "file/GRCFileData.h"
//
#include "file/FileIO.h"
//
#include <fstream>
#include <istream>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

using namespace affymetrix_grid_control;
using namespace affxgrc;
using namespace std;

//////////////////////////////////////////////////////////////////////

/*
 * Initialize the class to null values
 */
CGRCFileData::CGRCFileData()
{
}

//////////////////////////////////////////////////////////////////////

/*
 * Clean up any used memory
 */
CGRCFileData::~CGRCFileData()
{
}

//////////////////////////////////////////////////////////////////////

/*
 * Check for the file existance
 */
bool CGRCFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_strFileName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

/*
 * Read the file contents, either pre-release or release format.
 */
bool CGRCFileData::Read(GridControlData &grid)
{
	grid.Clear();
	if (IsReleaseFormat() == true)
		return ReadReleaseFormat(grid);
	else
		return ReadPreReleaseFormat(grid);
}

//////////////////////////////////////////////////////////////////////

/*
 * Checks the version and magic numbers for release values
 */
bool CGRCFileData::IsReleaseFormat()
{
	// Read the version and magic
	ifstream grc(m_strFileName.c_str(), ios::in | ios::binary);
	uint8_t ver=0;
	uint8_t magic=0;
	ReadUInt8(grc, magic);
	ReadUInt8(grc, ver);
	grc.close();
	return (ver == GRC_VERSION && magic == GRC_MAGIC);
}

//////////////////////////////////////////////////////////////////////

/*
 * Read the older pre-release (early access software) GRC file
 */
bool CGRCFileData::ReadPreReleaseFormat(GridControlData &grid)
{
	// Open the grc file.
	ifstream grc(m_strFileName.c_str(), ios::in | ios::binary);
	if (!grc)
	{
		return false;
	}

	// Read the counts.
	int32_t i32val;
	ReadInt32_I(grc, i32val);
	grid.SetColumns(i32val);
	ReadInt32_I(grc, i32val);
	grid.SetRows(i32val);
	ReadInt32_I(grc, i32val);
	ReadInt32_I(grc, i32val);
	grid.ResizeB2(i32val);
	ReadInt32_I(grc, i32val);
	grid.ResizeB1(i32val);
	ReadInt32_I(grc, i32val);
	grid.ResizeNS(i32val);
	ReadInt32_I(grc, i32val);
	m_Version = 0;

	// Read the unused data.
	int i;
	int16_t sval;
	int lChipSize = grid.GetColumns() * grid.GetRows();
	for (i=0; i<lChipSize; i++)
		ReadInt16_I(grc, sval);
	for (i=0; i<lChipSize; i++)
		ReadInt16_I(grc, sval);
	for (i=0; i<lChipSize; i++)
	{
		ReadInt16_I(grc, sval);
		ReadInt16_I(grc, sval);
	}

	// Read the coordinates.
	FeatureCoordinate coord;
	int n = grid.GetNumB2Probes();
	for (int i=0; i<n; i++)
	{
		ReadInt16_I(grc, sval);
		coord.x = sval;
		ReadInt16_I(grc, sval);
		coord.y = sval;
		grid.SetB2(i, coord);
	}
	n = grid.GetNumB1Probes();
	for (int i=0; i<n; i++)
	{
		ReadInt16_I(grc, sval);
		coord.x = sval;
		ReadInt16_I(grc, sval);
		coord.y = sval;
		grid.SetB1(i, coord);
	}
	n = grid.GetNumNSProbes();
	for (int i=0; i<n; i++)
	{
		ReadInt16_I(grc, sval);
		coord.x = sval;
		ReadInt16_I(grc, sval);
		coord.y = sval;
		grid.SetNS(i, coord);
	}

	// Close and return status.
	grc.close();
	return (grc.fail() == 0);
}

//////////////////////////////////////////////////////////////////////

/*
 * Read the release format GRC file
 */
bool CGRCFileData::ReadReleaseFormat(GridControlData &grid)
{
	// Open the grc file.
	ifstream grc(m_strFileName.c_str(), ios::in | ios::binary);
	if (!grc)
	{
		return false;
	}

	// Read the magic/version
	uint8_t ver=0;
	uint8_t magic=0;
	ReadUInt8(grc, magic);
	ReadUInt8(grc, ver);
	m_Version = ver;

	// Read the counts.
	int32_t ival;
	ReadInt32_N(grc, ival);
	grid.SetColumns(ival);
	ReadInt32_N(grc, ival);
	grid.SetRows(ival);
	ReadInt32_N(grc, ival);
	grid.ResizeB2(ival);
	ReadInt32_N(grc, ival);
	grid.ResizeB1(ival);
	ReadInt32_N(grc, ival);
	grid.ResizeNS(ival);

	// Read the coordinates.
	FeatureCoordinate coord;
	uint16_t sval;
	int n = grid.GetNumB2Probes();
	for (int i=0; i<n; i++)
	{
		ReadUInt16_N(grc, sval);
		coord.x = sval;
		ReadUInt16_N(grc, sval);
		coord.y = sval;
		grid.SetB2(i, coord);
	}
	n = grid.GetNumB1Probes();
	for (int i=0; i<n; i++)
	{
		ReadUInt16_N(grc, sval);
		coord.x = sval;
		ReadUInt16_N(grc, sval);
		coord.y = sval;
		grid.SetB1(i, coord);
	}
	n = grid.GetNumNSProbes();
	for (int i=0; i<n; i++)
	{
		ReadUInt16_N(grc, sval);
		coord.x = sval;
		ReadUInt16_N(grc, sval);
		coord.y = sval;
		grid.SetNS(i, coord);
	}

	// Close and return status.
	grc.close();
	return (grc.fail() == 0);
}

//////////////////////////////////////////////////////////////////////
