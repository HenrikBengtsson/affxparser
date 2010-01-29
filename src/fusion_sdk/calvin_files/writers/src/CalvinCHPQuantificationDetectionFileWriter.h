////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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
#ifndef _CHPQuantificationDetectionFileWriter_HEADER_
#define _CHPQuantificationDetectionFileWriter_HEADER_

/*! \file CalvinCHPQuantificationDetectionFileWriter.h Contains classes to write a quant/detection CHP file. */

#include "calvin_files/data/src/CHPQuantificationDetectionData.h"
#include "calvin_files/data/src/ProbeSetQuantificationDetectionData.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

namespace affymetrix_calvin_io
{

/*! A class to write a quant/detection CHP file. */
class CHPQuantificationDetectionFileWriter
{
private:
	/*! The file writer. */
	GenericFileWriter* writer;

	/*! The data set writer. */
	DataSetWriter* dataSetWriter;

	/*! The file position of the entry for each data set. */
	int32_t entryPos;

	/*! The maximum probe set name length. */
	int32_t maxProbeSetName;

public:

	/*! Constructor
	 * @param p Pointer to the quant/detection CHP data object.
	 */
	CHPQuantificationDetectionFileWriter(CHPQuantificationDetectionData& p);

	/*! Destructor */
	~CHPQuantificationDetectionFileWriter();

	/*! Write an entry.
	 * @param p A pointer to a quant/detection entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetQuantificationDetectionData & p);

	/*! Seeks to the data set.
	 */
	void SeekToDataSet();

private:

	/*! Writes the headers to the file. */
	void WriteHeaders();

	/*! Sets the file positions.
	 * @return The current file position.
	 */
	int32_t SetFilePositions();
};

}

#endif // _CHPQuantificationDetectionFileWriter_HEADER_
