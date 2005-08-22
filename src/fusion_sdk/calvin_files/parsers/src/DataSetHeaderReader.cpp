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

#include "DataSetHeaderReader.h"
#include "DataGroupHeader.h"
#include "FileInput.h"
#include <sys/stat.h>
#include <sys/types.h>

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
DataSetHeaderReader::DataSetHeaderReader()
{
}

/*
 * Read the names and file offsets for all DataSets associated with a DataGroup.
 */

void DataSetHeaderReader::ReadAllMinimumInfo(std::ifstream& fileStream, DataGroupHeader& dch, u_int32_t dataSetCnt)
{
	// Get the first dataSet offset
	u_int32_t nextDataSetFilePos = dch.GetDataSetPos();

	for (u_int32_t i = 0; i < dataSetCnt; ++i)
	{
		DataSetHeader dph;

		// Move to the indicated position in the file
		fileStream.seekg(nextDataSetFilePos, std::ios_base::beg);

		nextDataSetFilePos = ReadMinimumInfo(fileStream, dph);

		// Add the DataSetHeader to the file header
		dch.AddDataSetHdr(dph);
	}
}

/*
 * Read the complete information for all DataSetHeaders associated with a DataGroup.
 */
void DataSetHeaderReader::ReadAll(std::ifstream& fileStream, DataGroupHeader& dch, u_int32_t dataSetCnt)
{
	// Get the first dataSet offset
	u_int32_t nextDataSetFilePos = dch.GetDataSetPos();

	for (u_int32_t i = 0; i < dataSetCnt; ++i)
	{
		DataSetHeader dph;

		// Move to the indicated position in the file
		fileStream.seekg(nextDataSetFilePos, std::ios_base::beg);

		nextDataSetFilePos = Read(fileStream, dph);

		// Add the DataSetHeader to the file header
		dch.AddDataSetHdr(dph);
	}
}

/*
	* Reads the minimum DataSetHeader information.
	*/
u_int32_t DataSetHeaderReader::ReadMinimumInfo(std::ifstream& fileStream, DataSetHeader& dph)
{
	// Set the file position of the start of the DataSet.
	dph.SetHeaderStartFilePos(fileStream.tellg());

	// Read the file offset to the start of the data
	dph.SetDataStartFilePos(FileInput::ReadUInt32(fileStream));

	// Read the file offset to the next DataSet 
	u_int32_t nextDataSetFilePos = FileInput::ReadUInt32(fileStream);
		
	// Read the DataSetHeader name
	std::wstring name = FileInput::ReadString16(fileStream);
	dph.SetName(name);

	return nextDataSetFilePos;
}

/*
 * Reads the complete DataSetHeader information.
 */
u_int32_t DataSetHeaderReader::Read(std::ifstream& fileStream, DataSetHeader& dph)
{
	// Set the file position of the start of the DataSet.
	dph.SetHeaderStartFilePos(fileStream.tellg());

	// Read the file offset to the start of the data
	dph.SetDataStartFilePos(FileInput::ReadUInt32(fileStream));

	// Read the file offset to the next DataSet 
	u_int32_t nextDataSetFilePos = FileInput::ReadUInt32(fileStream);
		
	// Read the DataSetHeader name
	std::wstring name = FileInput::ReadString16(fileStream);
	dph.SetName(name);

	// Read the name-value pairs
	u_int32_t params = FileInput::ReadUInt32(fileStream);
	for (u_int32_t iparam = 0; iparam < params; ++iparam)
	{
		const void* mimeValue = 0;
		std::wstring paramName = FileInput::ReadString16(fileStream);
		int32_t mimeSize = FileInput::ReadBlob(fileStream, mimeValue);
		std::wstring paramType = FileInput::ReadString16(fileStream);
		ParameterNameValueType nvt(paramName, mimeValue, mimeSize, paramType);
		delete[] mimeValue;
		dph.AddNameValParam(nvt);
	}

	// Read the number of columns
	u_int32_t columns = FileInput::ReadUInt32(fileStream);

	for (u_int32_t icol = 0; icol < columns; ++icol)
	{
		// Read the type
		int8_t type = FileInput::ReadInt8(fileStream);
		// Read the size
		int32_t size = FileInput::ReadInt32(fileStream);

		dph.AddColumnType(ColumnType((DataSetColumnTypes)type, size));
	}

	// Read the number of rows
	int32_t numRows = FileInput::ReadInt32(fileStream);

	dph.SetRowCnt(numRows);
//	dph.SetDataStartFilePos(fileStream.tellg());	// set the offset to the start of the data

	return nextDataSetFilePos;
}
