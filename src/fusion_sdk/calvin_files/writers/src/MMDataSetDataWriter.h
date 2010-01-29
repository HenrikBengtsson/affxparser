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


#ifndef _MMDataSetDataWriter_HEADER_
#define _MMDataSetDataWriter_HEADER_

/*! \file MMDataSetDataWriter.h This file defines a class that writes the data section of a DataSet using Windows memory-mapping.
 */

#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/data/src/MemMapFile.h"
#include "calvin_files/writers/src/DataSetHeaderWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class writes a DataSet data.  The DataSet data is written to the file using memory-mapping.
 *	This class assumes that the DataSetHeaderWriter has already written the DataSetHeader
 *	to the file or more accurately, that the file has already been created by the GenericFileWriter.
 */
class MMDataSetDataWriter
{

public:

	/*! Constructor
	 *	@param dsHdr The DataSetHeader object that is to be written to the file.
	 *	@param name DAT file name.
	 */
	MMDataSetDataWriter(DataSetHeader& dsHdr, const std::string& name);

	/*! Destructor */
	~MMDataSetDataWriter();

public:

	/*! Opens the writer.
	 *	It closes the previously opened view and opens a memory-mapped view of the first section of the DataSet.
	 *	@return true if it succeeded.  Method may also throw exceptions.
	 *	@exception affymetrix_calvin_exceptions::FileCreateException
	 */
	bool Open();

	/*! Closes the writer.
	 *	This closes the memory-mapped view.
	 *	@return true if it succeeded.  Method may also throw exceptions.
	 */
	bool Close();

	/*! Maps the pixel data for access starting at firstRow and continuing for rowsToMap.
	 *	@param firstRow First DataSet row to map for access.
	 *	@param rowsToMap Requested number of DataSet rows to map.  Use GetRowsMapped for number of rows actually mapped.
	 *	@return True if successful.
	 */
	bool MapData(int32_t firstRow, int32_t rowsToMap);
	/*! Get a pointer to the data that has been mapped for access.
	 *	@return Pointer to the mapped data.
	 */
	char* GetMappedDataPtr();
	/*! Get the first DataSet row that has been mapped.
	 *	@return Index of the first DataSet row mapped for access.
	 */
	int32_t GetFirstRowMapped();
	/*! Get the number of DataSet rows that have been mapped.
	 *	@return The number of DataSet rows mapped for access.
	 */
	int32_t GetRowsMapped();
	/*! Get the suggested maximum number of DataSet rows to map.
	 *	@return The suggested maximum number of DataSet rows to map.
	 */
	int32_t GetMaxRowsToMap();

protected:
	/*! Open a memory-mapped view of the DataSet data section.
	 *	@return True if successful.
	 */
	bool OpenData();

private:
	/*! Windows memory-mapped file access */
	MemMapFile mmfile;
	/*! DataSetHeader */
	DataSetHeader& dataSetHeader;
	/*! Name of the file to memory map */
	std::string filename;
	/*! Size of the data section of the DataSet */
	int32_t dataSetDataSize;

	/*! Bytes per DataSet row */
	int32_t bytesPerRow;
};

}
#endif // _MMDataSetDataWriter_HEADER_
