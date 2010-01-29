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


#ifndef _MMDATFileWriter_HEADER_
#define _MMDATFileWriter_HEADER_

/*! \file MMDATFileWriter.h This file defines a class that writes a DAT file.  The pixel DataSet data
 *	is written to the file using Windows memory-mapping.
 */

#include "calvin_files/data/src/DATData.h"
#include "calvin_files/writers/src/DATFileWriter.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
#include "calvin_files/writers/src/MMDataSetDataWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class writes a DAT file.  The pixel DataSet data is written using Windows memory-mapping */
class MMDATFileWriter : public DATFileWriter
{

public:

	/*! Constructor
	 *  Creates the DAT file and writes header sections of the file.
	 *	@param p The DATData object to write to the file.
	 */
	MMDATFileWriter(DATData &p);
	/*! Destructor */
	~MMDATFileWriter();

public:

	/*! Opens the writer.
	 *	This opens the file and a memory-mapped view of the pixel DataSet.
	 *	It closes a previously opened file.
	 *	@return true if it succeeded.  Method may also throw exceptions.
	 *	@exception affymetrix_calvin_exceptions::FileCreateException
	 */
	bool Open();

	/*! Closes the writer.
	 *	This writes the pixel data to the file in network order and closes the file.
	 *  Once the writer is closed it should not be used to open and read the pixel data, overwritting existing pixel
	 *	data is OK.
	 *	@return true if it succeeded.  Method may also throw exceptions.
	 */
	bool Close();

public:
	/*! Maps the pixel data for access starting at firstRow and continuing for rowsToMap.
	 *  The number of pixels mapped is the number of row mapped times the number of pixels per row (DATData::GetCols).
	 *	@param firstRow First row of the DAT image to map for access.
	 *	@param rowsToMap Requested number of DAT image rows to map.  Use GetRowsMapped for number of rows actually mapped.
	 */
	void MapPixelData(int32_t firstRow, int32_t rowsToMap);
	/*! Get a pointer to the pixel data that has been mapped for access.
	 *	@return Pointer to the mapped data.
	 */
	u_int16_t* GetMappedPixelDataPtr();
	/*! Get the first DAT image row that has been mapped.
	 *	@return Index of the first DAT image row mapped for access.
	 */
	int32_t GetFirstPixelRowMapped();
	/*! Get the number of DAT image rows that have been mapped.
	 *  The number of pixels mapped is the number of row mapped times the number of pixels per row (DATData::GetCols).
	 *	@return The number of DAT image rows mapped for access.
	 */
	int32_t GetPixelRowsMapped();
	/*! Get the suggested maximum number of DAT image rows to map.
	 *	@return The suggested maximum number of DAT image rows to map
	 */
	int32_t GetMaxPixelRowsToMap();

private:
	virtual void WritePixels(const Uint16Vector &v) {}	// hide this method

	/*! Writes the pixel DataSet data in network byte order.
	 * Once this method is called this writer should not be used
	 * to read the pixel DataSet data.
	 */
	void HTONPixelDataSet();

private:
	/*! Memory-mapped DataSet data writer */
	MMDataSetDataWriter* pixelWriter;

	/*! DAT file name */
	std::string filename;

	/*! Pointer to the pixel DataSetHeader */
	affymetrix_calvin_io::DataSetHeader* dataSetHdr;

	/*! Number of rows of pixels in the DAT image */
	int32_t rows;

	/*! Number of columns of pixels in the DAT image */
	int32_t cols;
};

}

#endif // _MMDATFileWriter_HEADER_
