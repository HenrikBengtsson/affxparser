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

#include "DataSet.h"
#include "GenericData.h"
#include "FileInput.h"
#include <sys/types.h>
#include <sys/stat.h>

using namespace affymetrix_calvin_io;

#ifndef WIN32
#include <sys/mman.h>

#ifndef PAGE_SIZE
/// Page size used for memory mapping in non Windows environment
#define PAGE_SIZE (getpagesize())
#endif
#ifndef PAGE_MASK
/// Page mask used for memory mapping in non Windows environment
#define PAGE_MASK ~(PAGE_SIZE-1)
#endif
#ifndef PAGE_TRUNC
/// Page truncation pointer used for memory mapping in non Windows environment
#define PAGE_TRUNC(ptr) (ptr&(PAGE_MASK))
#endif
#endif

/*
 * Initialize the class.
 */
DataSet::DataSet(const std::string& fileName_, const DataSetHeader& header_, void* handle)
{
	fileName = fileName_;
	header = header_;

	mappedData = 0;
	data = 0;
//	fileOpen = false;
//	fileMapped = false;

#ifdef WIN32
	fileMapHandle = handle;
#else
	fp = 0;
#endif
	mapStart = 0;
	mapLen = 0;
}

/*

 * Clean up.
 */
DataSet::~DataSet()
{
}

/*
 * Informs the object to delete itself
 */
void DataSet::Delete()
{
	Close();
	delete this;
}

/*
 * Open the DataSet for reading
 */
bool DataSet::Open()
{
	UpdateColumnByteOffsets();

	// Compute the size of the DataSet data.
	u_int32_t bytes = header.GetDataSize();	// map the entire data dataGroup for now

#ifdef WIN32
	if (MapDataWin32(header.GetDataStartFilePos(), bytes) == false)
		return false;
#else
	// Open the file
	fp = fopen(fileName.c_str(), "r");
	if (fp == NULL)
	{
		return false;
	}

	if (MapDataPosix(header.GetDataStartFilePos(), bytes) == false)
		return false;
#endif

	return true;
}

/*
 * Close the DataSet
 */
void DataSet::Close()
{
	UnmapFile ();
}

#ifdef WIN32

std::string GetErrorMsg()
{
	LPVOID lpMsgBuf;
	if (!FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL ))
	{
		// Handle the error.
		return "";
	}

	std::string message = (char*)lpMsgBuf;

	// Free the buffer.
	LocalFree( lpMsgBuf );

	return message;
}

/*
 * Map the data on Win32
 */
bool DataSet::MapDataWin32(u_int32_t start, u_int32_t bytes)
{
	mapStart = start;

	if (bytes > MaxViewSize)
	{
		bytes = MaxViewSize;	// limit the amount of data mapped
	}

	SYSTEM_INFO sysinfo;
	GetSystemInfo (&sysinfo);
	u_int64_t qwFileOffset = u_int64_t(start);
	DWORD dwOffset = DWORD(qwFileOffset % sysinfo.dwAllocationGranularity);
	qwFileOffset = (qwFileOffset / sysinfo.dwAllocationGranularity) *
								sysinfo.dwAllocationGranularity;
	DWORD dwOffsetHigh = DWORD(qwFileOffset >> 32);
	DWORD dwOffsetLow  = DWORD(qwFileOffset & 0xFFFFFFFF);
	DWORD dwBytesToMap = bytes + dwOffset;

	if (mappedData != 0)
	{
		UnmapViewOfFile (mappedData);
	}
	mappedData = MapViewOfFile(fileMapHandle, FILE_MAP_READ, dwOffsetHigh, dwOffsetLow, dwBytesToMap);
	if (mappedData == 0)
	{
		std::string msg = GetErrorMsg();
		data = 0;
		CloseHandle (fileMapHandle);
		fileMapHandle = NULL;
		mapStart = 0;
		return false;
	}
	mapLen = bytes;
	data = (char *)mappedData + dwOffset;
	return true;
}

#else

/*
 * Map the data on Linux
 */
bool DataSet::MapDataPosix(u_int32_t start, u_int32_t bytes)
{
	mapStart = start;
	if (fp == NULL)
		return false;

	u_int32_t page_start = PAGE_TRUNC(start);
	u_int32_t page_offset = start - page_start;
	mapLen = bytes + page_offset;

	// Get the file size
	struct stat st;
	if (stat(fileName.c_str(), &st) == 0)
	{
	  // COMMENTED OUT Based on Luis' advice.	mapLen = bytes;
		int fileLen = st.st_size;
		if (fileLen < page_start + mapLen)
			mapLen = fileLen - page_start;
	}

	// Map the file.
	mappedData = mmap(NULL, mapLen, PROT_READ, MAP_SHARED, fileno(fp), page_start);
	if (mappedData == MAP_FAILED)
	{
		Close();
		return false;
	}
	else
	{
		data = ((char *)mappedData) + page_offset;
	}
	return true;
}
#endif

/*
 * Close the memory-map
 */
void DataSet::UnmapFile()
{
#ifdef WIN32

	// Unmap the view
	if (mappedData != 0 )
	{
		UnmapViewOfFile(mappedData);
		mappedData = 0;
	}
	fileMapHandle = NULL;
	data = 0;
	mapStart = 0;
	mapLen = 0;

#else
	if (fp != NULL)
	{
		if (mappedData)
		{
			munmap(mappedData, mapLen);
			mapLen = 0;
			mappedData = 0;
		}
		fclose(fp);
		fp = NULL;
	}
#endif
}

/*
 * Check the row, column and expected column type
 */
void DataSet::CheckRowColumnAndType(int32_t row, int32_t col, DataSetColumnTypes type)
{
	if (data == 0)
	{
		affymetrix_calvin_exceptions::DataSetNotOpenException e;
		throw e;
	}

	if (col < 0 || col >= header.GetColumnCnt())
	{
		affymetrix_calvin_exceptions::ColumnIndexOutOfBoundsException e;
		throw e;
	}

	if (row < 0 || row >= header.GetRowCnt())
	{
		affymetrix_calvin_exceptions::RowIndexOutOfBoundsException e;
		throw e;
	}

	// Check if the data type is expected

	if (header.GetColumnType(col).GetColumnType() != type)
	{
		affymetrix_calvin_exceptions::UnexpectedColumnTypeException e;
		throw e;
	}
}

/*
 * Detemine the address of data given row and col
 */
//char* DataSet::FilePosition(int32_t row, int32_t col)
//{
//	if (data == 0)
//	{
//		affymetrix_calvin_exceptions::DataSetNotOpenException e;
//		throw e;
//	}
//
//	// Byte offset in data set + byte offset of data set in file
//	u_int32_t startByte = BytesPerRow()*row + columnByteOffsets[col] + header.GetDataStartFilePos();
//
//#ifdef WIN32
//	// Remap the file if necessary
//	// Byte offset in data set + byte offset of data set in file
//	u_int32_t endByte = BytesPerRow()*row + columnByteOffsets[col+1] + header.GetDataStartFilePos();	// as long as col is in bounds this is safe.
//	if (startByte < mapStart || endByte > mapStart+mapLen)
//	{
//		MapDataWin32(startByte, header.GetDataSize() - startByte);
//	}
//#endif
//
//	char*	filePosition = data + (startByte-mapStart);
//	return filePosition;
//}

/*
 * Detemine the address of data given row and col.  Ensure all requested data is mapped
 */
char* DataSet::FilePosition(int32_t rowStart, int32_t col, int32_t rowCount)
{
	if (data == 0)
	{
		affymetrix_calvin_exceptions::DataSetNotOpenException e;
		throw e;
	}

	// Limit row count
	if (rowCount > header.GetRowCnt())
		rowCount= header.GetRowCnt();

	// Byte offset in data set + byte offset of data set in file
	u_int32_t startByte = BytesPerRow()*rowStart + columnByteOffsets[col] + header.GetDataStartFilePos();

#ifdef WIN32
	// Remap the file if necessary
	// Byte offset in data set + byte offset of data set in file
	u_int32_t endByte = BytesPerRow()*(rowStart+rowCount-1) + columnByteOffsets[col+1] + header.GetDataStartFilePos();	// as long as col is in bounds this is safe.
	if (startByte < mapStart || endByte > mapStart+mapLen)
	{
		MapDataWin32(startByte, header.GetDataSize() - startByte);
	}
#endif

	char*	filePosition = data + (startByte-mapStart);
	return filePosition;
}

/*
 * Update the column sizes
 */
void DataSet::UpdateColumnByteOffsets()
{
	columnByteOffsets.clear();

	int32_t accum = 0;
	int32_t cols = header.GetColumnCnt();
	for (int32_t col = 0; col < cols; ++col)
	{
		columnByteOffsets.push_back(accum);
		accum += header.GetColumnType(col).GetSize();
	}
	columnByteOffsets.push_back(accum);
}

void DataSet::GetData(int32_t row, int32_t col, u_int8_t& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadUInt8(instr);
}

void DataSet::GetData(int32_t row, int32_t col, int8_t& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadInt8(instr);
}

void DataSet::GetData(int32_t row, int32_t col, u_int16_t& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadUInt16(instr);
}

void DataSet::GetData(int32_t row, int32_t col, int16_t& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadInt16(instr);
}

void DataSet::GetData(int32_t row, int32_t col, u_int32_t& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadUInt32(instr);
}

void DataSet::GetData(int32_t row, int32_t col, int32_t& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadInt32(instr);
}

void DataSet::GetData(int32_t row, int32_t col, float& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadFloat(instr);
}

void DataSet::GetData(int32_t row, int32_t col, std::string& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadString8(instr);
}

void DataSet::GetData(int32_t row, int32_t col, std::wstring& value)
{
	// Get the data
	char* instr = FilePosition(row, col);
	value = FileInput::ReadString16(instr);
}


int32_t DataSet::ComputeEndRow(int32_t startRow, int32_t count)
{
	int32_t rows = startRow + count;
	if (count == -1 || (rows > header.GetRowCnt()))
		rows = header.GetRowCnt();

	return rows;
}

template<typename T> void DataSet::ClearAndSizeVector(std::vector<T>& values, u_int32_t size)
{
	values.clear();
	values.resize(size);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Uint8Vector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadUInt8(instr);
		}
	}
	else
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadUInt8(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Int8Vector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadInt8(instr);
		}
	}
	else
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadInt8(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Uint16Vector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadUInt16(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadUInt16(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Int16Vector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadInt16(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadInt16(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Uint32Vector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadUInt32(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadUInt32(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Int32Vector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadInt32(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadInt32(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, FloatVector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadFloat(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadFloat(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, std::vector<std::string>& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadString8(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadString8(instr);	// Is this much of an optimization?
		}
	}
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, WStringVector& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadString16(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadString16(instr);	// Is this much of an optimization?
		}
	}
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int8_t* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadUInt8(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadUInt8(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, int8_t* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadInt8(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadInt8(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int16_t* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadUInt16(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadUInt16(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, int16_t* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadInt16(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadInt16(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int32_t* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadUInt32(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadUInt32(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, int32_t* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadInt32(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadInt32(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, float* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadFloat(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadFloat(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, std::string* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadString8(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadString8(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, std::wstring* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			values[row-startRow] = FileInput::ReadString16(instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);	// ensure that the entire region is mapped.
		for (int32_t row = startRow; row < endRow; ++row)
		{
			values[row-startRow] = FileInput::ReadString16(instr);	// Is this much of an optimization?
		}
	}
	return endRow-startRow;
}
