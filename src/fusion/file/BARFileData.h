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

#if !defined(AFX_BARFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
#define AFX_BARFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_

/*! \file BARFileData.h This file provides BAR file reading capabilities.
 */

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

#include "file/TagValuePair.h"
//
#include <cstring>
#include <fstream>
#include <list>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affxbar
{

//////////////////////////////////////////////////////////////////////

/*! The possible data types stored in a BAR file. */
typedef enum _GDACFILES_BAR_DATA_TYPE
{
	/*! Double precision (64 bit) floating point. */
	BAR_DATA_DOUBLE,

	/*! Single precision (32 bit) floating point. */
	BAR_DATA_FLOAT,

	/*! 32 bit signed integer. */
	BAR_DATA_INTEGER,

	/*! 16 bit signed integer. */
	BAR_DATA_SHORT,

	/*! 8 bit signed integer. */
	BAR_DATA_CHAR,

	/*! 32 bit unsigned integer. */
	BAR_DATA_UINTEGER,

	/*! 16 bit unsigned integer. */
	BAR_DATA_USHORT,

	/*! 8 bit unsigned integer. */
	BAR_DATA_UCHAR

} GDACFILES_BAR_DATA_TYPE;

/*! An STL vector of data types. */
typedef std::vector<GDACFILES_BAR_DATA_TYPE> GDACFILES_BAR_DATA_TYPE_VECTOR;

//////////////////////////////////////////////////////////////////////

/*! This union is used to store data associated with a single value. */
typedef union _BarSequenceResultData
{
	/*! Double precision (64 bit) floating point. */
	double dValue;

	/*! Single precision (32 bit) floating point. */
	float fValue;

	/*! 32 bit signed integer. */
	int iValue;

	/*! 16 bit signed integer. */
	short sValue;

	/*! 8 bit signed integer. */
	char cValue;

	/*! 32 bit unsigned integer. */
	unsigned int uiValue;

	/*! 16 bit unsigned integer. */
	unsigned short usValue;

	/*! 8 bit unsigned integer. */
	unsigned char ucValue;
} BarSequenceResultData;

//////////////////////////////////////////////////////////////////////

/*! This class provides storage for results associated with a sequence. */
class CGDACSequenceResultItem
{
public:
	/*! Constructor */
	CGDACSequenceResultItem();

	/*! Destructor */
	~CGDACSequenceResultItem();

protected:
	/*! The name of the sequence. */
	std::string m_Name;

	/*! The version associated to the sequence. */
	std::string m_Version;

	/*! The group name for the sequence. */
	std::string m_GroupName;

	/*! The number of data points associated with the sequence. */
	int m_NumberDataPoints;

	/*! The number of columns of data for each data point. */
	int m_NumberColumns;

	/*! The number of name/value parameters. */
	int m_NumberParameters;

	/*! A pointer to the BAR file's column types vector. */
	GDACFILES_BAR_DATA_TYPE_VECTOR *m_pColumnTypes;

	/*! The parameter name/value array. */
	TagValuePairTypeVector m_Parameters;

	/*! A pointer to the data. */
	BarSequenceResultData **m_ppData;

	/*! A pointer to the data pointer.
	 * This is used when making copies of the object. The data pointer is not duplicated,
	 * just a pointer to the data object is made. */
	BarSequenceResultData ***m_pppData;

	/*! Flag indicating if the file is memory mapped. */
	bool m_bMapped;

	/*! Pointer to the memory mapped file. */
	char *m_lpData;

	/*! The file position of the start of the data. */
	int m_DataStartPosition;

	/*! Makes a copy of the object. The results are not copied to the new object, just
	 * the pointer back to the originating object.
	 * @param orig The object to copy.
	 */
	void MakeShallowCopy(CGDACSequenceResultItem &orig);

public:
	/*! Gets the sequence name.
	 * @return The sequence name.
	 */
	std::string GetName() const { return m_Name; }

	/*! Gets the version number.
	 * @return The version number.
	 */
	std::string GetVersion() const { return m_Version; }

	/*! Gets the group name.
	 * @return The group name.
	 */
	std::string GetGroupName() const { return m_GroupName; }

	/*! Returns the full name (name, group, version).
	 * @return The full name.
	 */
	std::string GetFullName() const;

	/*! Gets the number of data points (rows).
	 * @return The number of data points (rows).
	 */
	int GetNumberDataPoints() const { return m_NumberDataPoints; }

	/*! Gets the number of columns.
	 * @return The number of columns.
	 */
	int GetNumberColumns() const { return m_NumberColumns; }

	/*! Gets the type of data for a column.
	 * @param index The column index.
	 * @return The type of data at the given column.
	 */
	GDACFILES_BAR_DATA_TYPE GetColumnType(int index) const { return (*m_pColumnTypes)[index]; }

	/*! Gets the data for a given position.
	 * @param iData The row index.
	 * @param iCol The column index.
	 * @param data The returned data.
	 */
	void GetData(int iData, int iCol, BarSequenceResultData &data );

	/*! Gets the number of parameters.
	 * @return The number of parameters.
	 */
	int GetNumberParameters() const { return m_NumberParameters; }

	/*! Gets the parameter at the given index.
	 * @param index The index to the parameter array.
	 * @return The parameter.
	 */
	TagValuePairType &GetParameter(int index) { return m_Parameters[index]; }

	/*! Sets the sequence name.
	 * @param name The sequence name.
	 */
	void SetName(const char *name) { m_Name = name; }

	/*! Sets the version.
	 * @param ver The version.
	 */
	void SetVersion(const char *ver) { m_Version = ver; }

	/*! Sets the group name.
	 * @param group The group name.
	 */
	void SetGroupName(const char *group) { m_GroupName = group; }

	/*! Sets the number of data points and allocate memory for the results.
	 * @param n The number of data points (rows).
	 */
	void SetNumberDataPoints(int n);

	/*! Sets the data point (when writing a file).
	 * @param nIndex The row index.
	 * @param colIndex The column index.
	 * @param data The value of the data point.
	 */
	void SetDataPoint(int nIndex, int colIndex, BarSequenceResultData &data);

	/*! Adds a parameter to the list.
	 * @param tag The name of the parameter.
	 * @param value The value of the parameter.*
	 */
	void AddParameter(std::string tag, std::string value);

	/*! Friend to the top level class. */
	friend class CBARFileData;
};

////////////////////////////////////////////////////////////////////

/*! The class provides storage and reading capabilities for BAR files. */
class CBARFileData
{
public:
	/*! Constructor */
	CBARFileData();

	/*! Destructor */
	~CBARFileData();

protected:
	/*! The full path to the BAR file. */
	std::string m_FileName;

	/*! The version number of the file. */
	float	m_Version;

	/*! The number of sequences in the file. */
	int m_NumberSequences;

	/*! The number of columns of data in each sequence entry. */
	int m_NumberColumns;

	/*! The number of name/value paramters. */
	int m_NumberParameters;

	/*! The start position of the data. */
	int m_DataStartPosition;

	/*! The name/value parameters. */
	TagValuePairTypeVector m_Parameters;

	/*! The types of data stored in each column. */
	GDACFILES_BAR_DATA_TYPE_VECTOR m_ColumnTypes;

	/*! The sequence results. */
	std::vector<CGDACSequenceResultItem> m_Results;

	/*! A string to hold error messages. */
	std::string m_strError;

	/*! Reads the file.
	 * @param bReadHeaderOnly Flag to indicate if the header should only be read.
	 * @return True if successful.
	 */
	bool ReadFile(bool bReadHeaderOnly=false);

	/*! Reads the file header
	 * @return True if successful.
	 */
	bool ReadHeaderSection();

	/*! Reads the file data (after the header).
	 * @return True if successful.
	 */
	bool ReadDataSection();

	/*! Gets the size of a row of data.
	 * @return The number of bytes per data row.
	 */
	int GetDataRowSize();

	/*! Pointer to a memory mapped file. */
	void  *m_lpFileMap;

	/*! Pointer to the data in the memory mapped file. */
	char  *m_lpData;
#ifdef _MSC_VER

	/*! Windows handle to the file. */
	HANDLE m_hFileMap;

	/*! Windows handle to the file. */
	HANDLE m_hFile;
#else
	/*! File pointer for memory mapping. */
	FILE *m_fp;

	/*! The size of the file. */
	int m_MapLen;
#endif

	/*! Flag indicating if the file is open. */
	bool m_bFileOpen;

	/*! Flag indicating if the file is memory mapped. */
	bool m_bFileMapped;

public:
	/*! Sets the full path to the BAR file.
	 * @param name The file name.
	 */
	void SetFileName (const char *name) { m_FileName = name; }

	/*! Gets the full path of the file.
	 * @return The full path of the file.
	 */
	std::string GetFileName() const { return m_FileName; }

	/*! Reads the contents of the file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Reads the header of the file.
	 * @return True if successful.
	 */
	bool ReadHeader();

	/*! Checks if the file exists.
	 * @return True if exists.
	 */
	bool Exists();

	/*! Closes and deallocate memory. */
	void Close();

	/*! Gets an error string for file read errors.
	 * @return An error string for file read errors.
	 */
	std::string GetError() const { return m_strError; }

	/*! Gets the file version.
	 * @return The file version.
	 */
	float GetVersion() const { return m_Version; }

	/*! Gets the number of sequence stored in the file.
	 * @return The number of sequence stored in the file.
	 */
	int GetNumberSequences() const { return m_NumberSequences; }

	/*! Gets the number of columns of data per row.
	 * @return The number of columns of data per row.
	 */
	int	GetNumberColumns() const { return m_NumberColumns; }

	/*! Gets the number of name/value parameters.
	 * @return The number of name/value parameters.
	 */
	int GetNumberParameters() const { return m_NumberParameters; }

	/*! Gets the name/value parameter.
	 * @param index The index to the parameter of interest.
	 * @return The name/value parameter given the index.
	 */
	TagValuePairType &GetParameter(int index) { return m_Parameters[index]; }

	/*! Gets the type of data stored.
	 * @param index The column index.
	 * @return The type of data stored in the column.
	 */
	GDACFILES_BAR_DATA_TYPE &GetColumnTypes(int index) { return m_ColumnTypes[index]; }

	/*! Gets the sequence data.
	 * @param index The sequence index.
	 * @param seq The sequence data.
	 */
	void GetResults(int index, CGDACSequenceResultItem &seq);

	/*! Adds a parameter to the list.
	 * @param tag The parameter name.
	 * @param value The parameter value.
	 */
	void AddAlgorithmParameter(const char *tag, const char *value);

	/*! Adds a column for writing a BAR file.
	 * @param ctype The type of data to store at the column.
	 */
	void AddColumn(GDACFILES_BAR_DATA_TYPE ctype);

	/*! Sets the number of sequences to store in the BAR file.
	 * @param n The number of sequences.
	 */
	void SetNumberSequences(int n);

	/*! Retrieves a pointer to the sequences results. This should only be used when 
	 * performing write operations. Use the GetResults functions when reading the file.
	 * @param index The sequence index.
	 * @return A pointer to the sequence results.
	 */
	CGDACSequenceResultItem *GetResultsPtr(int index) { return &m_Results[index]; }
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BARFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
