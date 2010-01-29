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


#ifndef _DataSetHeaderReader_HEADER_
#define _DataSetHeaderReader_HEADER_

/*! \file DataSetHeaderReader.h This file provides methods to read the DataSetHeader from a file.
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
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 *	@return The file position of the next DataSet.
	 */
	u_int32_t ReadMinimumInfo(std::ifstream& fileStream, DataSetHeader& dsh);

	/*! Reads the complete DataSetHeader information.
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 *	@return The file position of the next DataSet.
	 */
	u_int32_t Read(std::ifstream& fileStream, DataSetHeader& dsh);

protected:
	/*! Read the file position of the start of the DataSet.
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 */
	void ReadDataSetStartFilePos(std::ifstream& fileStream, DataSetHeader& dsh);

	/*! Read the file position to the start of the data.
	 *	@param fileStream Open fstream positioned at the start of the data file position.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 */
	void ReadDataFilePos(std::ifstream& fileStream, DataSetHeader& dsh);

	/*! Read the file position to the next DataSet.
	 *	@param fileStream Open fstream positioned at the start of the next DataSet file position.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 *	@return The file position of the next data set.
	 */
	u_int32_t ReadNextDataSetFilePos(std::ifstream& fileStream, DataSetHeader& dsh);

	/*! Read the DataSetHeader name.
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader name.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 */
	void ReadName(std::ifstream& fileStream, DataSetHeader& dsh);

	/*! Read the parameter list (name-value-type).
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader parameter list count.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 */
	void ReadParameters(std::ifstream& fileStream, DataSetHeader& dsh);

	/*! Read column information.
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader column count.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 */
	void ReadColumns(std::ifstream& fileStream, DataSetHeader& dsh);

	/*! Read the number of rows.
	 *	@param fileStream Open fstream positioned at the start of the DataSetHeader row count.
	 *	@param dsh Reference to the DataSetHeader object to fill.
	 */
	void ReadRowCount(std::ifstream& fileStream, DataSetHeader& dsh);

};

}

#endif // _DataSetHeaderReader_HEADER_
