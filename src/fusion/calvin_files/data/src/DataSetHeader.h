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


#ifndef _DataSetHeader_HEADER_
#define _DataSetHeader_HEADER_

/*! \file DataSetHeader.h This file defines the data container for DataSetHeader information.
 */

#include "calvin_files/data/src/ColumnInfo.h"
#include "calvin_files/data/src/GenericDataHeader.h"
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/utils/src/AffyStlCollectionTypes.h"
//
#include <cstring>
#include <list>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

using namespace affymetrix_calvin_parameter;

namespace affymetrix_calvin_io
{

/*! The DataSetHeader information container class. */
class DataSetHeader
{
public:

	DataSetHeader();
	~DataSetHeader();

private:

	/*! total rows in the data set */
	int32_t rowCount;
	/*! data data set name */
	std::wstring name;
	/*! name/value pairs */
	ParameterNameValueTypeVector nameValParams;
	/*! column information */
	ColInfoVector columnTypes;
	/*! file position of the start of the dataSet header */
	u_int32_t headerStartFilePos;
	/*! file position of the start of the data */
	u_int32_t dataStartFilePos;
	/*! file position of the next dataSet header */
	u_int32_t nextSetFilePos;

public:

	/*!  */
	void Clear();

	void ClearNameValueParameters();

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

	void GetNameValIterators(ParameterNameValueTypeIt &begin, ParameterNameValueTypeIt &end);
	/*! Finds a ParameterNameValueType by name in the nameValPairs collection
	 *	@param name The name of the NameValPair to find
	 *	@param result Reference to a ParameterNameValueType to fill with the found ParameterNameValueType.
	 *	@return true if the ParameterNameValueType was found
	 */
	bool FindNameValParam(const std::wstring& name, ParameterNameValueType& result) const;

	/*!  */
	void AddColumn(const ColumnInfo& colInfo);
	/*!  */
	void AddIntColumn(const std::wstring& name);
	/*!  */
	void AddUIntColumn(const std::wstring& name);
	/*!  */
	void AddShortColumn(const std::wstring& name);
	/*!  */
	void AddUShortColumn(const std::wstring& name);
	/*!  */
	void AddByteColumn(const std::wstring& name);
	/*!  */
	void AddUByteColumn(const std::wstring& name);
	/*!  */
	void AddFloatColumn(const std::wstring& name);
	/*!
*	@param len Maximum number of char in string
*/
	void AddAsciiColumn(const std::wstring& name, int32_t len);
	/*!
	*   @param name The name of the column.
	*	@param len Maximum number of wchar_t in string
	*/
	void AddUnicodeColumn(const std::wstring& name, int32_t len);
	/*!  */
	ColumnInfo GetColumnInfo(int32_t index) const;
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

	/*! Set the file position of the start of the DataSet data.
	*  The value set here is not written to the file.
	*/
	void SetDataStartFilePos(u_int32_t pos) { dataStartFilePos  = pos; }
	/*! Get the file position of the start of the DataSet data. */
	u_int32_t GetDataStartFilePos() const { return dataStartFilePos; }

	/*! Set the file position of the next DataSet header. */
	void SetNextSetFilePos(u_int32_t pos) { nextSetFilePos = pos; }
	/*! Get the file position of the next DataSet header. */
	u_int32_t GetNextSetFilePos() const { return nextSetFilePos; }

protected:
	/*! Finds a ParameterNameValueType by name in the nameValPairs collection
	 *	@param p The ParameterNameValueType to find
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
