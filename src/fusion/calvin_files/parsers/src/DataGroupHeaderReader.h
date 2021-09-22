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


#ifndef _DataGroupHeaderReader_HEADER_
#define _DataGroupHeaderReader_HEADER_

/*! \file DataGroupHeaderReader.h This file provides methods to read the DataGroupHeaders from a file.
 */

#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/parsers/src/FileException.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <cstring>
#include <fstream>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // dont show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{
/*! This class reads the all the DataGroupHeader information from a file into a FileHeader object.*/
class DataGroupHeaderReader
{
public:
	/*! Constructor
	 */
	DataGroupHeaderReader();

public:
	/*! Reads all the DataGroupHeaders in a file and the minimum information for each DataSetHeader in every DataGroup.
	 *	@param fs Open fstream positioned at the start of the first DataGroupHeader in the file.
	 *	@param fh FileHeader object to fill.
	 *	@param dataGroupCnt Number of DataGroup in the file.
	 */
	void ReadAllMinimumInfo(std::ifstream& fs, FileHeader& fh, u_int32_t dataGroupCnt);

	/*! Reads all the DataGroupHeaders in a file and all information for each DataSetHeader in every DataGroup.
	 *	@param fileStream Open fstream positioned at the start of the first DataGroupHeader in the file.
	 *	@param fh FileHeader object to fill.
	 *	@param dataGroupCnt Number of DataGroup in the file.
	 */
	void ReadAll(std::ifstream& fileStream, FileHeader& fh, u_int32_t dataGroupCnt);

	/*! Reads the DataGroupHeader and the minimum information for all DataSetHeaders associated with this DataGroupHeader
	 *	from the file.
	 *	@param fileStream Open fstream positioned at the start of the first DataGroupHeader in the file.
	 *	@param dch DataGroupHeader object to fill.
	 *	@return The file position of the next data group
	 */
	u_int32_t ReadMinimumInfo(std::ifstream& fileStream, DataGroupHeader& dch);

	/*! Read the DataGroupHeader and all DataSetHeaders associated with this DataGroupHeader
	 *	from the file.
	 *	@param fileStream Open fstream positioned at the start of the first DataGroupHeader in the file.
	 *	@param dch DataGroupHeader object to fill.
	 *	@return The file position of the next data group
	 */
	u_int32_t Read(std::ifstream& fileStream, DataGroupHeader& dch);

	/*! Reads the DataGroupHeader from the file.  Doesn't read all DataSetHeader information.
	 *	@param fileStream Open fstream positioned at the start of a DataGroupHeader in the file.
	 *	@param dch DataGroupHeader object to fill with the header information.
	 *	@return The number of DataSetHeaders associated with the current DataGroupHeader.
	 */
	u_int32_t ReadHeader(std::ifstream& fileStream, DataGroupHeader& dch);

protected:
	/*! Read the file position of the start of the DataGroup.
	 *	@param fileStream Open fstream positioned at the start of the DataGroupHeader.
	 *	@param dsh Reference to the DataGroupHeader object to fill.
	 */
	void ReadDataGroupStartFilePos(std::ifstream& fileStream, DataGroupHeader& grpHdr);
	/*! Reads the file position of the next DataGroup.
	 *	@param fileStream Open fstream positioned at the start of the file position of a DataGroupHeader.
	 *	@param dch DataGroupHeader object in which to write the file position.
	 */
	void ReadNextDataGroupFilePos(std::ifstream& fileStream, DataGroupHeader& dch);
	
	/*! Reads the file position of the first DataSet associated with the current DataGroup.
	 *	@param fileStream Open fstream positioned at the start of the file position of the first DataSetHeader in the DataGroup.
	 *	@param dch DataGroupHeader object to which to add the DataSetHeader information.
	 */
	void ReadFirstDataSetFilePos(std::ifstream& fileStream, DataGroupHeader& dch);

	/*! Reads the number of DataSets associated with the current DataGroup.
	 *	@param fileStream Open fstream positioned at the start of the DataGroupHeader count.
	 *	@param dch DataGroupHeader object in which to add the DataSet count.
	 */
	u_int32_t ReadDataSetCnt(std::ifstream& fileStream, DataGroupHeader& dch);

	/*! Reads the number of DataGroup name.
	 *	@param fileStream Open fstream positioned at the start of the DataGroupHeader name.
	 *	@param dch DataGroupHeader object to which to add the DataGroup name.
	 */
	void ReadDataGroupName(std::ifstream& fileStream, DataGroupHeader& dch);

protected:
};

}

#endif // _DataGroupHeaderReader_HEADER_
