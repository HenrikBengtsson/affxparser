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


#include "calvin_files/writers/src/MMDataSetDataWriter.h"
//
#include "calvin_files/writers/src/DataSetHeaderWriter.h"
#include "calvin_files/writers/src/FileOutput.h"
#include "calvin_files/writers/src/FileWriteException.h"
//

using namespace affymetrix_calvin_io;

/*! maximum file mapping size = 200 MB */
const int32_t MAXFILEMAPSIZE = 1024*1024*200;

/*
 * Constructor
 */
MMDataSetDataWriter::MMDataSetDataWriter(DataSetHeader& dsHdr, const std::string& name)
: dataSetHeader(dsHdr), filename(name)
{
	dataSetDataSize = dataSetHeader.GetDataSize();
	bytesPerRow = dataSetHeader.GetRowSize();
}

/*
 * Destructor
 */
MMDataSetDataWriter::~MMDataSetDataWriter()
{
	Close();
}

/*
 * Opens the writer.
 */
bool MMDataSetDataWriter::Open()
{
	Close();
	return OpenData();
}

/*
 * Open a memory-mapped view of the DataSet data section.
 */
bool MMDataSetDataWriter::OpenData()
{
	if (dataSetHeader.GetRowCnt() == 0 ||
			dataSetHeader.GetColumnCnt() == 0)
	{
		return false;
	}

	int32_t rowsToMap = GetMaxRowsToMap();
	if (rowsToMap > dataSetHeader.GetRowCnt())
		rowsToMap = dataSetHeader.GetRowCnt();

	mmfile.SetFilename(filename);
	if (mmfile.Open(READ_AND_WRITE, ALLOWREAD_AND_WRITE) &&
			mmfile.MapData(dataSetHeader.GetDataStartFilePos(), rowsToMap*bytesPerRow))
	{
	}
	else
	{
		return false;
	}
	return true;

}

/*
 * Closes the writer.
 */
bool MMDataSetDataWriter::Close()
{
	mmfile.Close();
	return true;
}

/*
 * Get the suggested maximum number of DataSet rows to map.
 */
int32_t MMDataSetDataWriter::GetMaxRowsToMap()
{
	if (bytesPerRow == 0)
		return 0;
	return MAXFILEMAPSIZE / bytesPerRow;
}

/*
 * Maps the pixel data for access starting at firstRow and continuing for rowsToMap.
 */
bool MMDataSetDataWriter::MapData(int32_t firstRow, int32_t rowsToMap)
{
	if (firstRow + rowsToMap > dataSetHeader.GetRowCnt())
		rowsToMap = dataSetHeader.GetRowCnt() - firstRow;

	return mmfile.MapData(firstRow*bytesPerRow+dataSetHeader.GetDataStartFilePos(), rowsToMap*bytesPerRow);
}

/*
 * Get a pointer to the data that has been mapped for access.
 */
char* MMDataSetDataWriter::GetMappedDataPtr()
{
	return mmfile.GetDataPtr();
}

/*
 * Get the first DataSet row that has been mapped.
 */
int32_t MMDataSetDataWriter::GetFirstRowMapped()
{
	return ( (mmfile.GetFirstMappedBytePos()-dataSetHeader.GetDataStartFilePos())/bytesPerRow );
}

/*
 * Get the number of DataSet rows that have been mapped.
 */
int32_t MMDataSetDataWriter::GetRowsMapped()
{
	return ( mmfile.GetBytesMapped()/bytesPerRow );
}


