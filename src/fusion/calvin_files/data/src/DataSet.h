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


#ifndef _DataSet_HEADER_
#define _DataSet_HEADER_

#ifdef _MSC_VER
#include <windows.h>
#endif

#include "calvin_files/data/src/DataException.h"
#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffyStlCollectionTypes.h"
//
#include <cstring>
#include <fstream>
#include <string>
//


namespace affymetrix_calvin_io
{

// forward declare
class GenericData;

/*! This class provides methods to access the data of a DataSet. */
class DataSet
{
public:
	/*! Constructor. Use this constructor do access the data using memory-mapping.
	 *	On Windows, memory-mapping will be restricted to 200MB view of the DataSet data.
	 *	@param fileName The name of the generic file to access.
	 *	@param header The DataSetHeader of the DataSet to access.
	 *	@param handle A handle to the file mapping object
	 *	@param loadEntireDataSetHint Indicate if DataSet will attempt to read the entire DataSet data into a memory buffer.
	 */
	DataSet(const std::string& fileName, const affymetrix_calvin_io::DataSetHeader& header, void* handle, bool loadEntireDataSetHint=false);


	/*! Constructor. Use this constructor do access the data using std::ifstream.
	 *	With fstream access the entire DataSet data will be read into memory.
	 *	@param fileName The name of the generic file to access.
	 *	@param header The DataSetHeader of the DataSet to access.
	 *	@param ifs A reference to an open ifstream.
	 *	@param loadEntireDataSetHint Indicate if DataSet will attempt to read the entire DataSet data into a memory buffer.
	 */
	DataSet(const std::string& fileName, const affymetrix_calvin_io::DataSetHeader& header, std::ifstream& ifs, bool loadEntireDataSetHint=false);

public:
	/*! Method to release memory held by this object.  Closes object before deleting. */
	void Delete();

	/*! Method to open the DataSet to access the data.
	 *	@return true if successful
	 */
	bool Open();

	/*! Method to close the DataSet. */
	void Close();

	/*! Method to get a reference to the DataSetHeader
	 *	@return A reference to the DataSetHeader.
	 */
	const affymetrix_calvin_io::DataSetHeader& Header() { return header; }

	/*! Return the number of rows in the DataSet. */
	int32_t Rows() { return header.GetRowCnt(); }

	/*! Return the number of columns in the DataSet. */
	int32_t Cols() { return header.GetColumnCnt(); }

	/*! Determines if the DataSet is open
	 *	@return true if the DataSet is open
	 */
	bool IsOpen() { return (isOpen); }

	/*! Provides access to single data elements
	 *	@param row Row index.
	 *	@param col Column index.
	 *	@param value Reference to the data type to fill with the data.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not memory-mapped.
	 */
	void GetData(int32_t row, int32_t col, u_int8_t& value);

	void GetData(int32_t row, int32_t col, int8_t& value);

	void GetData(int32_t row, int32_t col, u_int16_t& value);

	void GetData(int32_t row, int32_t col, int16_t& value);

	void GetData(int32_t row, int32_t col, u_int32_t& value);

	void GetData(int32_t row, int32_t col, int32_t& value);

	void GetData(int32_t row, int32_t col, float& value);

	void GetData(int32_t row, int32_t col, std::string& value);

	void GetData(int32_t row, int32_t col, std::wstring& value);

	/*! Provides access to multiple data elements in the same column.
	 *	If count elements could not be read, it is not considered an error.  The vector
	 *	is filled with only the data that could be read.
	 *	@param col Column index.
	 *	@param startRow Row index of the data to be inserted into the vector at [0].
	 *	@param count Number of elements to retrieve. -1 indicates to read all
	 *	@param values Reference to the data type to fill with the data.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not memory-mapped.
	 */

	void GetData(int32_t col, int32_t startRow, int32_t count, Uint8Vector& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, Int8Vector& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, Uint16Vector& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, Int16Vector& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, Uint32Vector& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, Int32Vector& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, FloatVector& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, std::vector<std::string>& values);

	void GetData(int32_t col, int32_t startRow, int32_t count, WStringVector& values);

	/*! Provides access to multiple data elements in the same column.
	 *	The caller is responsible for allocating the storage to which count element values can be written.
	 *	If count elements could not be read, it is not considered an error.  The array
	 *	is filled with only the data that could be read.
	 *	@param col Column index.
	 *	@param startRow Row index of the data to be inserted into the vector at [0].
	 *	@param count Number of elements to retrieve. -1 indicates to read all
	 *	@param values Reference to the data type to fill with the data.
	 *	@return Number of elements read.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not memory-mapped.
	*/

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int8_t* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, int8_t* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int16_t* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, int16_t* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, u_int32_t* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, int32_t* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, float* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, std::string* values);

	int32_t GetDataRaw(int32_t col, int32_t startRow, int32_t count, std::wstring* values);

	/*! Check that the requested data matches the type of data in the column and that row and column are in bounds.
	 *	@param row Row index to check.
	 *	@param col Column index to check.
	 *	@param type Column type to check.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not memory-mapped.
	 *	@exception affymetrix_calvin_exceptions::ColumnIndexOutOfBoundsException The column index is out-of-bounds.
	 *	@exception affymetrix_calvin_exceptions::RowIndexOutOfBoundsException The row index is out-of-bounds.
	 *	@exception affymetrix_calvin_exceptions::UnexpectedColumnTypeException The column type does not match the type requested.
	 */
	void CheckRowColumnAndType(int32_t row, int32_t col, affymetrix_calvin_io::DataSetColumnTypes type);

//protected:

	/*! Return the bytes per row.
	 *	@return Bytes in a row.
	 */
	int32_t BytesPerRow() { return columnByteOffsets[header.GetColumnCnt()]; }

//protected:
	/*! Destructor. */
	~DataSet();

protected:
	/*! Open the DataSet using memory-mapping
	 *	@return True if the DataSet was successully mapped.
	 */
	bool OpenMM();

	/*! Read the DataSet data into a buffer using ifstream::read.
	 */
	void ReadDataSetUsingStream();

	/*! Close the memory mapped file. */
	void UnmapFile();

	/*! Delete the DataSet data read in by ifstream::read */
	void ClearStreamData();

	/*! Returns the address of a data element given a row and column.  Ensures that data from rowStart
	 *	to rowCount+rowStart are mapped unless that is larger than the mapped window.
	 *	@param rowStart Row index
	 *	@param col Column index
	 *	@param rowCount The number of rows to ensure are mapped starting at rowStart
	 *	@return Pointer to the data element at rowStart
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not open.
	 *	@exception affymetrix_calvin_exceptions::DataSetRemapException There was an error during a remap.
	 */
	char* FilePosition(int32_t rowStart, int32_t col, int32_t rowCount=1);

	/*! Returns the address of a data element given a row and column.  Ensures that data from rowStart
	 *	to rowCount+rowStart are copied from the file into a memory buffer.  The memory buffer will
	 *	remain intact until the next call to LoadDataAndReturnFilePosition.
	 *	@param rowStart Row index
	 *	@param col Column index
	 *	@param rowCount The number of rows to ensure are mapped starting at rowStart
	 *	@return Pointer to the data element at rowStart
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not open.
	 */
	char* LoadDataAndReturnFilePosition(int32_t rowStart, int32_t col, int32_t rowCount);

	/*! Updates the columnByteOffsets member. */
	void UpdateColumnByteOffsets();

	/*! Computes the index of the row after last to read.
	 *	@param startRow Row index of the data to be inserted into the vector at [0].
	 *	@param count Number of elements to be retrieved. -1 indicates read to the last element.
	 *	@return Index of row after the last row to read.
	 */
	int32_t ComputeEndRow(int32_t startRow, int32_t count);

	/*!	Clears and resizes the vector
	 *	@param values Reference to a vector to clear and resize.
	 *	@param size Target size of the vector
	 */
	template<typename T> void ClearAndSizeVector(std::vector<T>& values, u_int32_t size);

	/*! Template method to get data into a vector
	 *	@param col Column index.
	 *	@param startRow Row index of the data to be inserted into the vector at [0].
	 *	@param count Number of elements to retrieve. -1 indicates to read all
	 *	@param values Reference to the data type to fill with the data.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not memory-mapped.
	 */
	template<typename T> void GetDataT(int32_t col, int32_t startRow, int32_t count, T& values);

	/*! Template method to get data into an array
	 *	@param col Column index.
	 *	@param startRow Row index of the data to be inserted into the vector at [0].
	 *	@param count Number of elements to retrieve. -1 indicates to read all
	 *	@param values Reference to the data type to fill with the data.
	 *	@return Number of elements read.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotOpenException The file is not memory-mapped.
	 */
	template<typename T> int32_t GetDataRawT(int32_t col, int32_t startRow, int32_t count, T* values);


	/*! Returns the index of the last row mapped.
	 *	@return Index of the last row mapped.
	 */
	int32_t LastRowMapped();

	/*! Platform specific memory-mapping method */
#ifdef _MSC_VER

	bool MapDataWin32(u_int32_t start, u_int32_t bytes);

#else

	bool MapDataPosix(u_int32_t start, u_int32_t bytes);

#endif

	/*! Reads from the instr pointer into the vector at the index indicated.
	 *	@param index Index to the vector where to write the value.
	 *	@param values The vector into which to write the value.
	 *	@param instr A pointer to the data in the memory buffer.  The pointer is advanced by the method.
	 */
	void AssignValue(int32_t index, Uint8Vector& values, char*& instr);

	void AssignValue(int32_t index, Int8Vector& values, char*& instr);

	void AssignValue(int32_t index, Uint16Vector& values, char*& instr);

	void AssignValue(int32_t index, Int16Vector& values, char*& instr);

	void AssignValue(int32_t index, Uint32Vector& values, char*& instr);

	void AssignValue(int32_t index, Int32Vector& values, char*& instr);

	void AssignValue(int32_t index, FloatVector& values, char*& instr);

	void AssignValue(int32_t index, StringVector& values, char*& instr);

	void AssignValue(int32_t index, WStringVector& values, char*& instr);

	void AssignValue(int32_t index, u_int8_t* values, char*& instr);

	void AssignValue(int32_t index, int8_t* values, char*& instr);

	void AssignValue(int32_t index, u_int16_t* values, char*& instr);

	void AssignValue(int32_t index, int16_t* values, char*& instr);

	void AssignValue(int32_t index, u_int32_t* values, char*& instr);

	void AssignValue(int32_t index, int32_t* values, char*& instr);

	void AssignValue(int32_t index, float* values, char*& instr);

	void AssignValue(int32_t index, std::string* values, char*& instr);

	void AssignValue(int32_t index, std::wstring* values, char*& instr);

protected:
	/*! name of the file containing the data data set*.  */
	std::string fileName;

	/*! copy of the DataSetHeader */
	affymetrix_calvin_io::DataSetHeader header;

	/*! pointer to the mapped data, doesn't account for allocation granularity. */
	void*	mappedData;

	/*! pointer to the data.  In memory-mapping mode, the pointer has been adjusted for the allocation granularity. */
	char*	data;

	/*! Array of column byte offsets.  Updated when the file is opened.
	 *  There are columns + 1 elements
	 */
	Int32Vector columnByteOffsets;

#ifdef _MSC_VER

	/*! Handle returned by CreateFileMapping */
	HANDLE fileMapHandle;
	/*! Maximum size of the view to map */
	static const u_int32_t MaxViewSize = 200*1024*1024;	// 200MB

#else

	FILE *fp;

#endif

	/*! Indicates if the DataSet is open*/
	bool isOpen;
	/*! Byte offset to the start of the view */
	u_int32_t mapStart;
	/*! Number of bytes mapped to the view */
	u_int32_t mapLen;
	/*! A flag the indicates the data access mode.  True = access the data using memory-mapping.  False = access the data using std::ifstream */
	bool useMemoryMapping;
	/*! An open ifstream object */
	std::ifstream* fileStream;
	/*! Indicates whether to attempt to read all data into a memory buffer. */
	bool loadEntireDataSetHint;

};

}

#endif // _DataSet_HEADER_
