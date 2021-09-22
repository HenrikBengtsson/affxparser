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


#include "calvin_files/data/src/DataSet.h"
//
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/parsers/src/FileInput.h"
//
#include "util/Fs.h"
//
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_calvin_io;

#ifndef _MSC_VER
#include <unistd.h>
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
 * Initialize the object to use memory-mapping to access the file.
 */
DataSet::DataSet(const std::string& fileName_, const DataSetHeader& header_, void* handle, bool loadEntireDataSetHint_)
{
	fileName = fileName_;
	header = header_;

	mappedData = 0;
	data = 0;
	isOpen = false;

#ifdef _MSC_VER
	fileMapHandle = handle;
#else
	fp = 0;
#endif
	mapStart = 0;
	mapLen = 0;
	fileStream = 0;
	useMemoryMapping = true;
	loadEntireDataSetHint = loadEntireDataSetHint_;
}

/*
 * Initialize the object to use std::ifstream to access the file.
 */
DataSet::DataSet(const std::string& fileName_, const affymetrix_calvin_io::DataSetHeader& header_, std::ifstream& ifs, bool loadEntireDataSetHint_)
{
	fileName = fileName_;
	header = header_;

	mappedData = 0;
	data = 0;
	isOpen = false;

#ifdef _MSC_VER
	fileMapHandle = 0;
#else
	fp = 0;
#endif
	mapStart = 0;
	mapLen = 0;
	fileStream = &ifs;
	useMemoryMapping = false;
	loadEntireDataSetHint = loadEntireDataSetHint_;
}

/*

 * Clean up.
 */
DataSet::~DataSet()
{
	Close();
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

	if (useMemoryMapping)
		isOpen = OpenMM();
	else
	{
		ReadDataSetUsingStream();
		isOpen = true;
	}
	return isOpen;
}

/*
 * Open the file using memory-mapping
 */
bool DataSet::OpenMM()
{
#ifdef _MSC_VER
	if (MapDataWin32(header.GetDataStartFilePos(), header.GetDataSize()) == false)
		return false;
#else
	// Open the file
	fp = fopen(fileName.c_str(), "r");
	if (fp == NULL)
	{
		return false;
	}

	if (MapDataPosix(header.GetDataStartFilePos(), header.GetDataSize()) == false)
		return false;
#endif

	return true;
}

/*
 * Reads the DataSet data from the file into a memory buffer.
 */
void DataSet::ReadDataSetUsingStream()
{
	if(loadEntireDataSetHint == false)
		return;

	mapLen = header.GetDataSize();
	mapStart = header.GetDataStartFilePos();

	data = new char[mapLen];
	fileStream->seekg(mapStart);
	fileStream->read(data, mapLen);
}

/*
 * Close the DataSet
 */
void DataSet::Close()
{
	if (useMemoryMapping)
		UnmapFile();
	else
		ClearStreamData();
}

#ifdef _MSC_VER

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
	if (Fs::fileExists(fileName))
	{
          int64_t fileLen = Fs::fileSize(fileName);
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
#ifdef _MSC_VER

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
 * Delete the buffer
 */
void DataSet::ClearStreamData()
{
	delete[] data;
	data = 0;
	mapStart = 0;
	mapLen = 0;
}

/*
 * Check the row, column and expected column type
 */
void DataSet::CheckRowColumnAndType(int32_t row, int32_t col, DataSetColumnTypes type)
{
	if (isOpen == false)
	{
		affymetrix_calvin_exceptions::DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (col < 0 || col >= header.GetColumnCnt())
	{
		affymetrix_calvin_exceptions::ColumnIndexOutOfBoundsException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	if (row < 0 || row >= header.GetRowCnt())
	{
		affymetrix_calvin_exceptions::RowIndexOutOfBoundsException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	// Check if the data type is expected

	if (header.GetColumnInfo(col).GetColumnType() != type)
	{
		affymetrix_calvin_exceptions::UnexpectedColumnTypeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * Detemine the address of data given row and col.  Ensure all requested data is mapped
 */
char* DataSet::FilePosition(int32_t rowStart, int32_t col, int32_t rowCount)
{
	if (isOpen == false)
	{
		affymetrix_calvin_exceptions::DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	// Limit row count
	if (rowCount > header.GetRowCnt())
		rowCount= header.GetRowCnt();

	if (useMemoryMapping == false && loadEntireDataSetHint == false)
	{
		return LoadDataAndReturnFilePosition(rowStart, col, rowCount);
	}

	// Byte offset in data set + byte offset of data set in file
	u_int32_t startByte = BytesPerRow()*rowStart + columnByteOffsets[col] + header.GetDataStartFilePos();

#ifdef _MSC_VER

	if (useMemoryMapping)
	{
		// Byte offset in data set + byte offset of data set in file
		u_int32_t endByte = BytesPerRow()*(rowStart+rowCount-1) + columnByteOffsets[col+1] + header.GetDataStartFilePos();	// as long as col is in bounds this is safe.

		// Remap the file if necessary
		if (startByte < mapStart || endByte > mapStart+mapLen)
		{
			if (startByte < mapStart)	// moving backwards through the data, attempt to find an optimum startByte.
			{
				u_int32_t reverseStartByte = 0;
				if (endByte > MaxViewSize)
					reverseStartByte = endByte - MaxViewSize;

				// Don't go above the DataSet data
				if (reverseStartByte < header.GetDataStartFilePos())
					reverseStartByte = header.GetDataStartFilePos();

				if (MapDataWin32(reverseStartByte, header.GetDataStartFilePos() + header.GetDataSize() - reverseStartByte) == false)
				{
					affymetrix_calvin_exceptions::DataSetRemapException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
					throw e;
				}
			}
			else	// forward
			{
				if (MapDataWin32(startByte, header.GetDataStartFilePos() + header.GetDataSize() - startByte) == false)
				{
					affymetrix_calvin_exceptions::DataSetRemapException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
					throw e;
				}
			}
		}
	}
#endif

	char*	filePosition = data + (startByte-mapStart);
	return filePosition;
}

/*
 *
 */
char* DataSet::LoadDataAndReturnFilePosition(int32_t rowStart, int32_t col, int32_t rowCount)
{
	// Delete the previous data
	ClearStreamData();

	mapLen = BytesPerRow()*rowCount;
	mapStart = BytesPerRow()*rowStart + columnByteOffsets[col] + header.GetDataStartFilePos();

	data = new char[mapLen];
	fileStream->seekg(mapStart);
	fileStream->read(data, mapLen);
	return data;
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
		accum += header.GetColumnInfo(col).GetSize();
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

template<typename T> void DataSet::GetDataT(int32_t col, int32_t startRow, int32_t count, T& values)
{
	int32_t endRow = ComputeEndRow(startRow, count);
	ClearAndSizeVector(values, endRow-startRow);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			// Get the data
			char* instr = FilePosition(row, col);
			AssignValue(row-startRow, values, instr);
		}
	}
	else
	{
		char* instr = FilePosition(startRow, col, count);
		int32_t recomputePositionRow = LastRowMapped();

		for (int32_t row = startRow; row < endRow; ++row)
		{
			if (row > recomputePositionRow)
			{
				instr = FilePosition(row, col, count-row);
				recomputePositionRow = LastRowMapped();
			}
			AssignValue(row-startRow, values, instr);
		}
	}
}

void DataSet::AssignValue(int32_t index, Uint8Vector& values, char*& instr)
{
	values[index] = FileInput::ReadUInt8(instr);
}

void DataSet::AssignValue(int32_t index, Int8Vector& values, char*& instr)
{
	values[index] = FileInput::ReadInt8(instr);
}

void DataSet::AssignValue(int32_t index, Uint16Vector& values, char*& instr)
{
	values[index] = FileInput::ReadUInt16(instr);
}

void DataSet::AssignValue(int32_t index, Int16Vector& values, char*& instr)
{
	values[index] = FileInput::ReadInt16(instr);
}

void DataSet::AssignValue(int32_t index, Uint32Vector& values, char*& instr)
{
	values[index] = FileInput::ReadUInt32(instr);
}

void DataSet::AssignValue(int32_t index, Int32Vector& values, char*& instr)
{
	values[index] = FileInput::ReadInt32(instr);
}

void DataSet::AssignValue(int32_t index, FloatVector& values, char*& instr)
{
	values[index] = FileInput::ReadFloat(instr);
}

void DataSet::AssignValue(int32_t index, StringVector& values, char*& instr)
{
	values[index] = FileInput::ReadString8(instr);
}

void DataSet::AssignValue(int32_t index, WStringVector& values, char*& instr)
{
	values[index] = FileInput::ReadString16(instr);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Uint8Vector& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Int8Vector& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Uint16Vector& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Int16Vector& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Uint32Vector& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, Int32Vector& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, FloatVector& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, std::vector<std::string>& values)
{
	GetDataT(col, startRow, count, values);
}

void DataSet::GetData(int32_t col, int32_t startRow, int32_t count, WStringVector& values)
{
	GetDataT(col, startRow, count, values);
}

template<typename T> int32_t DataSet::GetDataRawT(int32_t col, int32_t startRow, int32_t count, T* values)
{
	int32_t endRow = ComputeEndRow(startRow, count);

	if (header.GetColumnCnt() > 1)
	{
		for (int32_t row = startRow; row < endRow; ++row)
		{
			char* instr = FilePosition(row, col);
			AssignValue(row-startRow, values, instr);
		}
	}
	else // optimize
	{
		char* instr = FilePosition(startRow, col, count);
		int32_t recomputePositionRow = LastRowMapped();

		for (int32_t row = startRow; row < endRow; ++row)
		{
			if (row > recomputePositionRow)
			{
				instr = FilePosition(row, col, count-row);
				recomputePositionRow = LastRowMapped();
			}
			AssignValue(row-startRow, values, instr);
		}
	}
	return endRow-startRow;
}

void DataSet::AssignValue(int32_t index, u_int8_t* values, char*& instr)
{
	values[index] = FileInput::ReadUInt8(instr);
}

void DataSet::AssignValue(int32_t index, int8_t* values, char*& instr)
{
	values[index] = FileInput::ReadInt8(instr);
}

void DataSet::AssignValue(int32_t index, u_int16_t* values, char*& instr)
{
	values[index] = FileInput::ReadUInt16(instr);
}

void DataSet::AssignValue(int32_t index, int16_t* values, char*& instr)
{
	values[index] = FileInput::ReadInt16(instr);
}

void DataSet::AssignValue(int32_t index, u_int32_t* values, char*& instr)
{
	values[index] = FileInput::ReadUInt32(instr);
}

void DataSet::AssignValue(int32_t index, int32_t* values, char*& instr)
{
	values[index] = FileInput::ReadInt32(instr);
}

void DataSet::AssignValue(int32_t index, float* values, char*& instr)
{
	values[index] = FileInput::ReadFloat(instr);
}

void DataSet::AssignValue(int32_t index, std::string* values, char*& instr)
{
	values[index] = FileInput::ReadString8(instr);
}
void DataSet::AssignValue(int32_t index, std::wstring* values, char*& instr)
{
	values[index] = FileInput::ReadString16(instr);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int8_t* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, int8_t* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int16_t* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, int16_t* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int32_t* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, int32_t* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, float* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, std::string* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::GetDataRaw(int32_t col, int32_t startRow, int32_t count, std::wstring* values)
{
	return GetDataRawT(col, startRow, count, values);
}

int32_t DataSet::LastRowMapped()
{
	return (mapLen+(mapStart-header.GetDataStartFilePos()))/BytesPerRow() - 1;
}
