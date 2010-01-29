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


#ifndef _DataGroupHeader_HEADER_
#define _DataGroupHeader_HEADER_

#include "calvin_files/data/src/DataSetHeader.h"
//
#include <cstring>
#include <list>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class DataGroupHeader
{
public:
	
	DataGroupHeader();
	DataGroupHeader(const std::wstring &n);
	~DataGroupHeader();

private:

	/*! data dataGroup name */
	std::wstring name;
	/*! file position of the 1st data dataSet */
	u_int32_t dataSetPos;
	/*! file position of the next dataGroup */
	u_int32_t nextGrpPos;
	/*! file position of the start of the data group header */
	u_int32_t headerStartFilePos;
	/*! data dataSets in this dataGroup */
	DataSetHdrVector dataSetHdrs;

public:

	/*!  */
	void Clear();
	/*!  */
	void SetName(const std::wstring &p);
	/*!  */
	std::wstring GetName() const;
	/*! Get the data set count */
	int32_t GetDataSetCnt() const;
	/*!  */
	void AddDataSetHdr(const DataSetHeader &p);

	/*!Replace a data set header with a new data set header of the same name.
	 * @param new data set header with the same name as the header it replaces.
	*/
	void ReplaceDataSetHdr(const DataSetHeader &p);

	/*!  */
	DataSetHeader& GetDataSet(int32_t index);
	/*!  */
	const DataSetHeader& GetDataSetConst(int32_t index) const;
	/*!  */
	void GetDataSetIterators(DataSetHdrIt &begin, DataSetHdrIt &end);

	/*! Set the file position of the start of the DataSet header.
	 *	The value set here is not written to the file.
	 */
	void SetHeaderStartFilePos(u_int32_t pos) { headerStartFilePos = pos; }

	/*! Get the file position of the start of the DataSet header. */
	u_int32_t GetHeaderStartFilePos() const { return headerStartFilePos; }

/*! Set the file position of the DataSet header.
*  The value set here is not necessarily the value written to the file.
*/
	void SetDataSetPos(u_int32_t pos) { dataSetPos  = pos; }
	/*! Get the file position of the DataSet header. */
	u_int32_t GetDataSetPos() const { return dataSetPos; }
	/*! Set the file position of the next DataGroup header. */
	void SetNextGroupPos(u_int32_t pos) { nextGrpPos  = pos; }
	/*! Get the file position of the next DataGroup header. */
	u_int32_t GetNextGroupPos() const { return nextGrpPos; }
	/*!
	 */
	affymetrix_calvin_io::DataSetHeader* FindDataSetHeader(const std::wstring& dataSetName);
};

/*! vector of DataGroupHeaders */
typedef std::vector<DataGroupHeader> DataGroupHdrVector;
/*! constant iterator of DataGroupHeaders */
typedef std::vector<DataGroupHeader>::iterator DataGroupHdrIt;
/*! constant iterator of DataGroupHeaders */
typedef std::vector<DataGroupHeader>::const_iterator DataGroupHdrConstIt;

}

#endif // _DataGroupHeader_HEADER_
