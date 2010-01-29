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
#ifndef _CHPTilingFileWriter_HEADER_
#define _CHPTilingFileWriter_HEADER_

/*! \file CalvinCHPTilingFileWriter.h Contains classes to write a tiling CHP file. */

#include "calvin_files/data/src/CHPTilingData.h"
#include "calvin_files/data/src/CHPTilingEntry.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

namespace affymetrix_calvin_io
{

/*! A class to write a tiling CHP file. */
class CHPTilingFileWriter
{
private:
	/*! The file writer. */
	GenericFileWriter* writer;

	/*! The data set writer. */
	DataSetWriter* dataSetWriter;

	/*! The file position of the entry for each data set. */
	int32_t *entryPos;

public:

	/*! Constructor
	 * @param p Pointer to the tiling CHP data object.
	 */
	CHPTilingFileWriter(CHPTilingData& p);

	/*! Destructor */
	~CHPTilingFileWriter();

	/*! Write a tiling entry.
	 * @param p A pointer to a tiling entry.
	 */
	void WriteTilingEntry(const CHPTilingEntry& p);

	/*! Seeks to the data set.
	 * @param index The data set index.
	 */
	void SeekToDataSet(int index);

private:

	/*! The current data set index. */
	int dataSetIndex;

	/*! Writes the headers to the file. */
	void WriteHeaders();

	/*! Sets the file positions.
	 * @return The current file position.
	 */
	int32_t SetFilePositions();
};

}

#endif // _CHPTilingFileWriter_HEADER_
