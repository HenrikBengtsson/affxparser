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

#include "FusionDatFileWriter.h"
#include "DevelopmentException.h"

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;

/*
 * Deletes the FusionDatFileWriter object 
 */
void FusionDatFileWriter::Delete()
{
	NotImplementedException e;
	throw e;
}

/* Opens the writer.
 *	This creates and opens the file and writes the header.  It closes a previously opened file.
 */
bool FusionDatFileWriter::Open(FusionDatData& data)
{
	NotImplementedException e;
	throw e;
}

/* Closes the writer.
 *	This updates and closes the file.
*/
bool FusionDatFileWriter::Close()
{
	NotImplementedException e;
	throw e;
}

/*
 * Maps the pixel data for access starting at firstRow and continuing for rowsToMap
 */
void FusionDatFileWriter::MapData(int32_t firstRow, int32_t rowsToMap)
{
	NotImplementedException e;
	throw e;
}

/*
 * Get a pointer to the pixel data that has been mapped for access
 */
u_int16_t* FusionDatFileWriter::GetMappedPixelDataPtr()
{
	NotImplementedException e;
	throw e;
}

/*
 * Get the first row that has been mapped
 */
int32_t FusionDatFileWriter::GetFirstRowMapped()
{
	NotImplementedException e;
	throw e;
}

/*
 * Get the number of rows that have been mapped.
 */
int32_t FusionDatFileWriter::GetRowsMapped()
{
	NotImplementedException e;
	throw e;
}

/*
 * Get the maximum number of rows that can be mapped.
 */
int32_t FusionDatFileWriter::GetMaxRowsToMap()
{
	NotImplementedException e;
	throw e;
}

/*
 * Writes the min and max pixel intensities to the file
 */
void FusionDatFileWriter::SetMinMaxData(u_int16_t min, u_int16_t max)
{
	NotImplementedException e;
	throw e;
}

/*
 * Writes the average pixel intensity to the file 
 */
void FusionDatFileWriter::SetAvgData(float value)	// (used to be double)
{
	NotImplementedException e;
	throw e;
}

/*
 * Writes the standard deviation to the file
 */
void FusionDatFileWriter::SetStdev(float value)
{
	NotImplementedException e;
	throw e;
}