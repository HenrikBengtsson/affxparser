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

#ifndef _DataSetHeaderReader_HEADER_
#define _DataSetHeaderReader_HEADER_

/*! \file DataSetHeaderReader.h This file provides methods to read the DataSetHeader from a file.
 */

#include "FileException.h"
#include "AffymetrixBaseTypes.h"
#include "AffymetrixGuid.h"
#include "GenericData.h"
#include <string>
#include <fstream>

#ifdef WIN32
#pragma warning(disable: 4290) // dont show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{
/*! This class reads the all DataSetHeader information associated with a DataGroup from a file. */
class DataSetHeaderReader
{
public:
	/*! Constructor	 */
	DataSetHeaderReader();

public:
	/*! Reads the minimum DataSetHeader information for all DataSets associated with a DataGroup.
	 *	@param fileStream Open fstream positioned at the start of the first DataSetHeader in a DataGroup.
	 *	@param dch DataGroupHeader object to which to add the DataSetHeader information.
	 *	@param dataSetCnt Number of DataSets in the DataGroup.
	 */
	void ReadAllMinimumInfo(std::ifstream& fileStream, DataGroupHeader& dch, u_int32_t dataSetCnt);

	/*! Reads the complete DataSetHeader information for all DataSets associated with a DataGroup.
	 *	@param fileStream Open fstream positioned at the start of the first DataSetHeader in a DataGroup.
	 *	@param dch DataGroupHeader object to which to add the DataSetHeader information.
	 *	@param dataSetCnt Number of DataSets in the DataGroup.
	 */
	void ReadAll(std::ifstream& fileStream, DataGroupHeader& dch, u_int32_t dataSetCnt);

	/*! Reads the minimum DataSetHeader information.
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader.
	 *	@param dph Reference to the DataSetHeader object to fill.
	 *	@return The file position of the next DataSet.
	 */
	u_int32_t ReadMinimumInfo(std::ifstream& fileStream, DataSetHeader& dph);

	/*! Reads the complete DataSetHeader information.
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader.
	 *	@param dph Reference to the DataSetHeader object to fill.
	 *	@return The file position of the next DataSet.
	 */
	u_int32_t Read(std::ifstream& fileStream, DataSetHeader& dph);

protected:
};

}

#endif // _DataSetHeaderReader_HEADER_