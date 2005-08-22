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

#ifndef _ColumnTypes_HEADER_
#define _ColumnTypes_HEADER_

#ifdef WIN32
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


class ColumnType
{
private:
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
	 *	@param type_ Type of data in the column.
	 *	@param size_ Size of each element of in the column.
	 *	@param len_ Number of elements of type in the column.
	 *	@param overhead_ Number of extra bytes in the column
	 */
	ColumnType(DataSetColumnTypes type_, int32_t size_, int32_t len_, int32_t overhead_)
		: type(type_), size(size_), len(len_), overhead(overhead_)
	{
	}

public:
		
	/*! Constructor - used by the file read operation
	 *	@param type_ Type of data in the column.
	 *	@param totalSize Total size of the colum in bytes.
	 */
	ColumnType(DataSetColumnTypes type_, int32_t totalSize)
		: type(type_), size(totalSize), len(1), overhead(0)
	{ 
		if (type == ASCIICharColType || type == UnicodeCharColType)
		{
			overhead = 4;
			if (type == UnicodeCharColType)
			{
				size = sizeof(int16_t);
				len = (totalSize-overhead)/sizeof(int16_t);
			}
			else if (type == ASCIICharColType)
			{
				size = sizeof(int8_t);
				len = (totalSize-overhead)/sizeof(int8_t);
			}
		}
	}

	bool operator==(const ColumnType &p) const { return (type == p.type && size == p.size); }

	bool operator!=(const ColumnType &p) const { return (type != p.type || size != p.size); }

	/*! Returns the type of the data in the column
	 *	@param Returns the type of the data in the column
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
};

class ByteColumnType : public ColumnType
{
public:

	ByteColumnType() : ColumnType(ByteColType, sizeof(int8_t), 1, 0) {}
};

class UByteColumnType : public ColumnType
{
public:

	UByteColumnType() : ColumnType(UByteColType, sizeof(u_int8_t), 1, 0) {}
};

class ShortColumnType : public ColumnType
{
public:

	ShortColumnType() : ColumnType(ShortColType, sizeof(int16_t), 1, 0) {}
};

class UShortColumnType : public ColumnType
{
public:

	UShortColumnType() : ColumnType(UShortColType, sizeof(u_int16_t), 1, 0) {}
};

class IntColumnType : public ColumnType
{
public:

	IntColumnType() : ColumnType(IntColType, sizeof(int32_t), 1, 0) {}
};

class UIntColumnType : public ColumnType
{
public:

	UIntColumnType() : ColumnType(UIntColType, sizeof(u_int32_t), 1, 0) {}
};

class FloatColumnType : public ColumnType
{
public:

	FloatColumnType() : ColumnType(FloatColType, sizeof(float), 1, 0) {}
};

class ASCIIColumnType : public ColumnType
{
public:

	ASCIIColumnType(int32_t maxLn) : ColumnType(ASCIICharColType, sizeof(int8_t), maxLn, 4) {}
};

class UnicodeColumnType : public ColumnType
{
public:

	UnicodeColumnType(int32_t maxLn) : ColumnType(UnicodeCharColType, sizeof(int16_t), maxLn, 4) {}
};

/*! vector of column types */
typedef std::vector<ColumnType> ColTypeVector;

/*! iterator of column types */
typedef std::vector<ColumnType>::const_iterator ColTypeConstIt;

}
#endif // _ColumnTypes_HEADER_
