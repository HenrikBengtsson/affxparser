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


#include "calvin_files/parsers/src/DataSetHeaderReader.h"
//
#include "calvin_files/data/src/DataGroupHeader.h"
#include "calvin_files/parsers/src/FileInput.h"
//
#include <sys/stat.h>
#include <sys/types.h>
//

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
u_int32_t DataSetHeaderReader::ReadMinimumInfo(std::ifstream& fileStream, DataSetHeader& dsh)
{
	ReadDataSetStartFilePos(fileStream, dsh);
	ReadDataFilePos(fileStream, dsh);
	u_int32_t nextDataSetFilePos = ReadNextDataSetFilePos(fileStream, dsh);
	ReadName(fileStream, dsh);
	
	return nextDataSetFilePos;
}

/*
 * Reads the complete DataSetHeader information.
 */
u_int32_t DataSetHeaderReader::Read(std::ifstream& fileStream, DataSetHeader& dsh)
{
	ReadDataSetStartFilePos(fileStream, dsh);
	ReadDataFilePos(fileStream, dsh);
	u_int32_t nextDataSetFilePos = ReadNextDataSetFilePos(fileStream, dsh);
	ReadName(fileStream, dsh);
	ReadParameters(fileStream, dsh);
	ReadColumns(fileStream, dsh);
	ReadRowCount(fileStream, dsh);

//	dph.SetDataStartFilePos(fileStream.tellg());	// set the offset to the start of the data

	return nextDataSetFilePos;
}

/*
 * Read the file position of the start of the DataSet.
 */
void DataSetHeaderReader::ReadDataSetStartFilePos(std::ifstream& fileStream, DataSetHeader& dsh)
{
	dsh.SetHeaderStartFilePos(fileStream.tellg());
}

/*
 * Read the file position to the start of the data.
 */
void DataSetHeaderReader::ReadDataFilePos(std::ifstream& fileStream, DataSetHeader& dsh)
{
	dsh.SetDataStartFilePos(FileInput::ReadUInt32(fileStream));
}

/*
 * Read the file position to the next DataSet.
 */
u_int32_t DataSetHeaderReader::ReadNextDataSetFilePos(std::ifstream& fileStream, DataSetHeader& dsh)
{
	u_int32_t nextDataSetFilePos = FileInput::ReadUInt32(fileStream);
	dsh.SetNextSetFilePos(nextDataSetFilePos);
	return nextDataSetFilePos;
}

/*
 * Read the DataSetHeader name.
 */
void DataSetHeaderReader::ReadName(std::ifstream& fileStream, DataSetHeader& dsh)
{
	std::wstring name = FileInput::ReadString16(fileStream);
	dsh.SetName(name);
}

/*
 * Read the parameter list (name-value-type).
 */
void DataSetHeaderReader::ReadParameters(std::ifstream& fileStream, DataSetHeader& dsh)
{
	u_int32_t params = FileInput::ReadUInt32(fileStream);
	for (u_int32_t iparam = 0; iparam < params; ++iparam)
	{
		const void* mimeValue = 0;
		std::wstring paramName = FileInput::ReadString16(fileStream);
		int32_t mimeSize = FileInput::ReadBlob(fileStream, mimeValue);
		std::wstring paramType = FileInput::ReadString16(fileStream);
		ParameterNameValueType nvt(paramName, (void*)mimeValue, mimeSize, paramType);
		// deleting 'const void*' is undefined, cast it to a char*
		delete[] (char*)mimeValue;
		dsh.AddNameValParam(nvt);
	}
}

/*
 * Read column information.
 */
void DataSetHeaderReader::ReadColumns(std::ifstream& fileStream, DataSetHeader& dsh)
{
	// Read the number of columns
	u_int32_t columns = FileInput::ReadUInt32(fileStream);

	for (u_int32_t icol = 0; icol < columns; ++icol)
	{
		// Read the name
		std::wstring name = FileInput::ReadString16(fileStream);
		// Read the type
		int8_t type = FileInput::ReadInt8(fileStream);
		// Read the size
		int32_t size = FileInput::ReadInt32(fileStream);

		dsh.AddColumn(ColumnInfo(name, (DataSetColumnTypes)type, size));
	}
}

/*
 * Read the number of rows.
 */
void DataSetHeaderReader::ReadRowCount(std::ifstream& fileStream, DataSetHeader& dsh)
{
	int32_t numRows = FileInput::ReadInt32(fileStream);
	dsh.SetRowCnt(numRows);
}
