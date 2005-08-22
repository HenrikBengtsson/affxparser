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

#ifndef _DataSetHeader_HEADER_
#define _DataSetHeader_HEADER_

#include <string>
#include <list>
#include "ParameterNameValueType.h"
#include "GenericDataHeader.h"
#include "AffyStlCollectionTypes.h"
#include "ColumnTypes.h"

#ifdef WIN32
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

using namespace affymetrix_calvin_parameter;

namespace affymetrix_calvin_io
{

class DataSetHeader
{
public:

	DataSetHeader();
	~DataSetHeader();

private:

	/*! total rows in the dataGroup */
	int32_t rowCount;
	/*! data dataGroup name */
	std::wstring name;
	/*! name/value pairs */
	ParameterNameValueTypeVector nameValParams;
	/*! column types */
	ColTypeVector columnTypes;
	/*! file position of the start of the data dataSet header*/
	u_int32_t headerStartFilePos;
	/*! file position of the start of the data */
	u_int32_t dataStartFilePos;

public:

	/*!  */
	void Clear();
	/*!  */
	int32_t GetDataSize() const;
	/*!  */
	int32_t GetRowSize() const;
	/*!  */
	void SetName(const std::wstring &p);
	/*!  */
	std::wstring GetName() const;
	/*!  */
	int32_t GetNameValParamCnt() const;
	/*!  */
	void AddNameValParam(const ParameterNameValueType &p);
	/*!  */
	void GetNameValIterators(ParameterNameValueTypeConstIt &begin, ParameterNameValueTypeConstIt &end) const;
	/*! Finds a ParameterNameValueType by name in the nameValPairs collection
	 *	@param name The name of the NameValPair to find
	 *	@param result Reference to a ParameterNameValueType to fill with the found ParameterNameValueType.
	 *	@return true if the ParameterNameValueType was found
	 */
	bool FindNameValParam(const std::wstring& name, ParameterNameValueType& result) const;

	/*!  */
	void AddColumnType(const ColumnType& colType);
	/*!  */
	void AddIntColumnType();
	/*!  */
	void AddUIntColumnType();
	/*!  */
	void AddShortColumnType();
	/*!  */
	void AddUShortColumnType();
	/*!  */
	void AddByteColumnType();
	/*!  */
	void AddUByteColumnType();
	/*!  */
	void AddFloatColumnType();
	/*!
*	@param len Maximum number of char in string
*/
	void AddAsciiColumnType(int32_t len);
	/*!
	*	@param len Maximum number of wchar_t in string
	*/
	void AddUnicodeColumnType(int32_t len);
	/*!  */
	ColumnType GetColumnType(int32_t index) const;
	/*!  */
	int32_t GetRowCnt() const;
	/*!  */
	void SetRowCnt(int32_t p);
	/*!  */
	int32_t GetColumnCnt() const;

	/*! Set the file position of the start of the DataSet header.
	 *	The value set here is not written to the file.
	 */
	void SetHeaderStartFilePos(u_int32_t pos) { headerStartFilePos = pos; }
	/*! Get the file position of the start of the DataSet header. */
	u_int32_t GetHeaderStartFilePos() const { return headerStartFilePos; }
	/*! 
	/*! Set the file position of the start of the DataSet data.
	*  The value set here is not written to the file.
	*/
	void SetDataStartFilePos(u_int32_t pos) { dataStartFilePos  = pos; }
	/*! Get the file position of the start of the DataSet data. */
	u_int32_t GetDataStartFilePos() const { return dataStartFilePos; }

protected:
	/*! Finds a ParameterNameValueType by name in the nameValPairs collection
	 *	@param nv The ParameterNameValueType to find
	 *	@return An iterator referencing the NameValPair if it exists, otherwise it returns nameValPairs.end()
	 */
	ParameterNameValueTypeConstIt FindNameValParam(const ParameterNameValueType& p) const;

private:

};

/*! vector of DataSetHeaders */
typedef std::vector<DataSetHeader> DataSetHdrVector;
/*! constant iterator of DataSetHeaders */
typedef std::vector<DataSetHeader>::iterator DataSetHdrIt;

}

#endif // _DataSetHeader_HEADER_
