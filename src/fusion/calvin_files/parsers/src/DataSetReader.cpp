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


#include "calvin_files/parsers/src/DataSetReader.h"
//
#include "calvin_files/parsers/src/FileInput.h"
//

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
DataSetReader::DataSetReader(std::ifstream& is, DataSetHeader& dph) : fileStream(is), dataSetHdr(dph)
{
	// Position the file stream to the start of the DataSet
	fileStream.seekg(dataSetHdr.GetDataStartFilePos());
}

/*
 * Destructor
 */
DataSetReader::~DataSetReader()
{
}

/*
 * Get the DataSet name
 */
std::wstring DataSetReader::GetDataSetName() const
{
	return dataSetHdr.GetName();
}

/*
 * Read int8_t from the DataSet.  File stream position is incremented
 */
int32_t DataSetReader::ReadBuffer(char* buffer, int32_t count)
{
	fileStream.read(buffer, count);
	return fileStream.gcount();
}

/*
 * Read int8_t from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(int8_t& value)
{
	value = FileInput::ReadInt8(fileStream);
}

/*
 * Read u_int8_t from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(u_int8_t& value)
{
	value = FileInput::ReadUInt8(fileStream);
}

/*
 * Read int16_t from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(int16_t& value)
{
	value = FileInput::ReadInt16(fileStream);
}

/*
 * Read u_int16_t from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(u_int16_t& value)
{
	value = FileInput::ReadUInt16(fileStream);
}

/*
 * Read int32_t from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(int32_t& value)
{
	value = FileInput::ReadInt32(fileStream);
}

/*
 * Read u_int32_t from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(u_int32_t& value)
{
	value = FileInput::ReadUInt32(fileStream);
}

/*
 * Read float from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(float& value)
{
	value = FileInput::ReadFloat(fileStream);
}

/*
 * Read string from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(std::string& value)
{
	value = FileInput::ReadString8(fileStream);
}

/*
 * Read wstring from the DataSet.  File stream position is incremented
 */
void DataSetReader::Read(std::wstring& value)
{
	value = FileInput::ReadString16(fileStream);
}
