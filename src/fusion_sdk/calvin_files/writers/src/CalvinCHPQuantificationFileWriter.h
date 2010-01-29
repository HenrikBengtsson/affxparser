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
#ifndef _CHPQuantificationFileWriter_HEADER_
#define _CHPQuantificationFileWriter_HEADER_

/*! \file CalvinCHPQuantificationFileWriter.h Contains classes to write a quant CHP file. */

#include "calvin_files/data/src/CHPQuantificationData.h"
#include "calvin_files/data/src/ProbeSetQuantificationData.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

namespace affymetrix_calvin_io
{

/*! A class to write a quant CHP file. */
class CHPQuantificationFileWriter
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
	 * @param p Pointer to the quant CHP data object.
	 */
	CHPQuantificationFileWriter(CHPQuantificationData& p);

	/*! Destructor */
	~CHPQuantificationFileWriter();

	/*! Write an entry.
	 * @param p A pointer to a quant entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetQuantificationData & p);

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

#endif // _CHPQuantificationFileWriter_HEADER_
