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


#ifndef _GridControlFileReader_HEADER_
#define _GridControlFileReader_HEADER_

/*! \file GridControlFileReader.h This file provides methods to read a GRC data file.
 */

#include "calvin_files/data/src/DataException.h"
#include "calvin_files/parsers/src/FileException.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include "file/GridControlData.h"
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
	
/*! The identifier for a template file. */
#define GRD_FILE_TYPE_IDENTIFIER "affymetrix-calvin-grid-control"

/*! The name of the dataSet which contains the B1 probes. */
#define GRD_FILE_B1_SET_NAME L"B1 Probes"

/*! The name of the dataSet which contains the B2 probes. */
#define GRD_FILE_B2_SET_NAME L"B2 Probes"

/*! The name of the dataSet which contains the non-synthesized features. */
#define GRD_FILE_NS_SET_NAME L"Non-Synthesized Features"

/*! The name of the data dataGroup which holds the probe cooridnates. */
#define GRD_FILE_COORDINATE_GROUP_NAME L"Grid Control Probes"

/*! The name of the rows parameter. */
#define GRD_ROWS_PARAMETER_NAME L"Rows"

/*! The name of the columns parameter. */
#define GRD_COLUMNS_PARAMETER_NAME L"Columns"

/*! This class reads a GRC data file. It is an interpreter class.*/
class GridControlFileReader
{
public:
	/*! Constructor */
	GridControlFileReader();

	/*! Destructor */
	~GridControlFileReader();

public:
	/*! Reads the contents of a GRC file.
	 *
	 * @param fileName The name of the file to read.
	 * @param data A reference to an object that will receive information from the file.
	 * @return True if successfully read.
	 */
	void Read(const std::string &fileName, affymetrix_grid_control::GridControlData& data);
};

}

#endif // _GridControlFileReader_HEADER_
