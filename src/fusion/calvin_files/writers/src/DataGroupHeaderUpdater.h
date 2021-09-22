////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008 Affymetrix, Inc.
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


#ifndef _DataGroupHeaderUpdater_HEADER_
#define _DataGroupHeaderUpdater_HEADER_

/*! \file DataGroupHeaderUpdater.h This file defines a class that updates the data group header of an existing file.
 */

#include "calvin_files/data/src/DataGroupHeader.h"
#include "calvin_files/writers/src/DataSetHeaderUpdater.h"
#include "calvin_files/writers/src/DataSetWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{
/*! This class updates the data set header of an existing file with new information. */
class DataGroupHeaderUpdater
{

public:

	/*! Constructor */
	DataGroupHeaderUpdater(std::ofstream& fileStream, DataGroupHeader &hdr);

	/*! Destructor */
	~DataGroupHeaderUpdater();

	void UpdateNextDataGroupOffset(u_int32_t offset);

	void UpdateLeadDataSetOffset(u_int32_t offset);

	void UpdateDataSetCount(u_int32_t count);

	/*! Append a new data set in this data group.
	 * @param setHdr The new data set that will be appended.
	 */
	void AppendDataSetHeader(DataSetHeader& setHdr);

	/*! Replace a data set in this data group with a new one. 
	 * @param targetName The name of the data set to be replaced.
	 * @param newHdr The new data set that will be written into the file.
	 */
	void ReplaceDataSetHeader(std::wstring targetName, DataSetHeader& newHdr);

private:
	/*! Open output filestream */
	std::ofstream* os;

	/*! The data set header */
	DataGroupHeader* grpHdr;

	void SeekNextDataGrpPosition();

	void SeekLeadDataSetPosition();

	void SeekDataSetCntPosition();

};

}

#endif // _DataGroupHeaderUpdater_HEADER_
