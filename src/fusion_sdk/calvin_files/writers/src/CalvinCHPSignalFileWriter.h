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
#ifndef _CHPSignalFileWriter_HEADER_
#define _CHPSignalFileWriter_HEADER_

/*! \file CalvinCHPSignalFileWriter.h Contains classes to write a Signal CHP file. */

#include <fstream>
#include "CHPSignalData.h"
#include "GenericFileWriter.h"
#include "ProbeSetSignalData.h"

namespace affymetrix_calvin_io
{

/*! A class to write a Signal CHP file. */
class CHPSignalFileWriter
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
	 * @param p Pointer to the Signal CHP data object.
	 */
	CHPSignalFileWriter(CHPSignalData& p);

	/*! Destructor */
	~CHPSignalFileWriter();

	/*! Write an entry.
	 * @param p A pointer to a Signal entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetSignalData & p);

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

#endif // _CHPSignalFileWriter_HEADER_
