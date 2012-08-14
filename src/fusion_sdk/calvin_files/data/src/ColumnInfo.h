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


#ifndef _ColumnInfo_HEADER_
#define _ColumnInfo_HEADER_

/*! \file ColumnInfo.h This file defines column information class of various types.
 */

//
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
#include <vector>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! Data dataSet column data types */
enum DataSetColumnTypes {
	ByteColType,
	UByteColType,
	ShortColType,
	UShortColType,
	IntColType,
	UIntColType,
	FloatColType,
	ASCIICharColType,
	UnicodeCharColType
};

/*! Base class for the varous columns */
class ColumnInfo
{
private:
	/*! Name of the column. */
	std::wstring name;
	/*! Type of data in this column */
	DataSetColumnTypes type;
	/*! size of an individual element in bytes */
	int32_t size;
	/*! number of elements in column */
	int32_t len;
	/*! overhead size in bytes */
	int32_t overhead;

protected:
	/*! Constructor - used only by dervied types
	 *	@param name_ Name of the column.
	 *	@param type_ Type of data in the column.
	 *	@param size_ Size of each element of in the column.
	 *	@param len_ Number of elements of type in the column.
	 *	@param overhead_ Number of extra bytes in the column
	 */
	ColumnInfo(const std::wstring& name_, DataSetColumnTypes type_, int32_t size_, int32_t len_, int32_t overhead_);

public:

	/*! Constructor - used by the file read operation
	 *	@param name_ Name of the column.
	 *	@param type_ Type of data in the column.
	 *	@param totalSize Total size of the colum in bytes.
	 */
	ColumnInfo(const std::wstring& name_, DataSetColumnTypes type_, int32_t totalSize);

	/*! Equiality operator
	 *	@param p object to compare against
	 */
	bool operator==(const ColumnInfo &p) const { return (name == p.name && type == p.type && size == p.size); }

	/*! Inequiality operator
	 *	@param p object to compare against
	 */
	bool operator!=(const ColumnInfo &p) const { return (name != p.name || type != p.type || size != p.size); }

	/*! Returns the type of the data in the column
	 *	@return The type of the data in the column
	 */
	DataSetColumnTypes GetColumnType() const { return type; }

	/*! Returns the total size of the column in bytes.
	 *	@return Size in bytes of the column
	 */
	int32_t GetSize() const { return size*len + overhead; }

	/*! Returns the number of elements of type in the column
	 *	@return Number of elements of type in the column
	 */
	int32_t GetLength() const { return len; }

	/*! Get the column name.
	 *	@return The column name.
	 */
	std::wstring GetName() { return name; }
};

/*! Byte column */
class ByteColumn : public ColumnInfo
{
public:

	ByteColumn(const std::wstring& name_) : ColumnInfo(name_, ByteColType, sizeof(int8_t), 1, 0) {}
};

/*! Unsigned byte column */
class UByteColumn : public ColumnInfo
{
public:

	UByteColumn(const std::wstring& name_) : ColumnInfo(name_, UByteColType, sizeof(u_int8_t), 1, 0) {}
};

/*! Short column */
class ShortColumn : public ColumnInfo
{
public:

	ShortColumn(const std::wstring& name_) : ColumnInfo(name_, ShortColType, sizeof(int16_t), 1, 0) {}
};

/*! Unsigned short column */
class UShortColumn : public ColumnInfo
{
public:

	UShortColumn(const std::wstring& name_) : ColumnInfo(name_, UShortColType, sizeof(u_int16_t), 1, 0) {}
};

/*! Int (int32_t) column */
class IntColumn : public ColumnInfo
{
public:

	IntColumn(const std::wstring& name_) : ColumnInfo(name_, IntColType, sizeof(int32_t), 1, 0) {}
};

/*! Unsigned int (u_int32_t) column */
class UIntColumn : public ColumnInfo
{
public:

	UIntColumn(const std::wstring& name_) : ColumnInfo(name_, UIntColType, sizeof(u_int32_t), 1, 0) {}
};

/*! Float column */
class FloatColumn : public ColumnInfo
{
public:

	FloatColumn(const std::wstring& name_) : ColumnInfo(name_, FloatColType, sizeof(float), 1, 0) {}
};

/*! ASCII string column */
class ASCIIColumn : public ColumnInfo
{
public:

	ASCIIColumn(const std::wstring& name_, int32_t maxLn) : ColumnInfo(name_, ASCIICharColType, sizeof(int8_t), maxLn, 4) {}
};

/* Unicode string column */
class UnicodeColumn : public ColumnInfo
{
public:

	UnicodeColumn(const std::wstring& name_, int32_t maxLn) : ColumnInfo(name_, UnicodeCharColType, sizeof(int16_t), maxLn, 4) {}
};

/*! vector of column info */
typedef std::vector<ColumnInfo> ColInfoVector;

/*! iterator of column info */
typedef std::vector<ColumnInfo>::const_iterator ColumnInfoConstIt;

}
#endif // _ColumnInfo_HEADER_
