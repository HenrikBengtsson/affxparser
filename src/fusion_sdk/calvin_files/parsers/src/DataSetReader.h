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


#ifndef _DataSetReader_HEADER_
#define _DataSetReader_HEADER_

/*! \file DataSetReader.h This file provides methods to read the data of a DataSet.
 */

#include "calvin_files/data/src/DataSetHeader.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! Class to read a DataSet */
class DataSetReader
{

public:

	/*! Constructor
	 *	@param s Reference to an open ifstream
	 *	@param d Reference to a DataSetHeader that has been initialized
	 */
	DataSetReader(std::ifstream& s, DataSetHeader& d);

	/*! Destructor */
	~DataSetReader();

public:

	/*! Get the DataSet name
	 *	@return DataSet name
	 */
	std::wstring GetDataSetName() const;

	/*! Get the DataSetHeader
	 *	@return DataSetHeader
	 */
	const DataSetHeader& GetDataSetHeader(){ return dataSetHdr; }

	/*! Reads a byte buffer from the DataSet.  File stream position is incremented
	 *	@param buffer Pointer to the buffer.
	 *  @param count Size of the buffer.
	 *  @return The number of bytes read.
	 */
	int32_t ReadBuffer(char* buffer, int32_t count);

	/*! Read int8_t from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(int8_t& value);

	/*! Read u_int8_t from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(u_int8_t& value);

	/*! Read int16_t from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(int16_t& value);

	/*! Read u_int16_t from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(u_int16_t& value);

	/*! Read int32_t from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(int32_t& value);

	/*! Read u_int32_t from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(u_int32_t& value);

	/*! Read float from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(float& value);

	/*! Read string from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(std::string& value);

	/*! Read wstring from the DataSet.  File stream position is incremented
	 *	@param value Variable reference to receive the value
	 */
	void Read(std::wstring& value);

private:

	/*! Open file stream */
	std::ifstream& fileStream;
	/*! Data dataSet header that has been initialized */
	DataSetHeader& dataSetHdr;

};

/*! typedef of a vector of DataSetReaders */
typedef std::vector<DataSetReader> DataSetReaderVector;
/*! typedef of a constant iterator of DataSetReaders */
typedef std::vector<DataSetReader>::iterator DataSetReaderIt;

}

#endif // _DataSetReader_HEADER_
