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


#ifndef _DataSetHeaderUpdater_HEADER_
#define _DataSetHeaderUpdater_HEADER_

/*! \file DataSetHeaderUpdater.h This file defines a class that updates the data set header of an existing file.
 */

#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/parameter/src/ParameterNameValueType.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{
/*! This class updates the data set header of an existing file with new information. */
class DataSetHeaderUpdater
{

public:

	/*! Constructor */
	DataSetHeaderUpdater(std::ofstream& fileStream, DataSetHeader &hdr);

	/*! Destructor */
	~DataSetHeaderUpdater();

	/*! Update the parameter list.
	 *	Can only update types that have the same name and type and value size in the source and target
	 */
    bool UpdateParameter(affymetrix_calvin_parameter::ParameterNameValueType &nvt);

		void UpdateNextDataSetPosition(u_int32_t position);

private:
	/*! Open output filestream */
	std::ofstream* os;

    /*! The data set header */
    DataSetHeader* setHdr;

    u_int32_t filePos;

		void SeekNextDataSetPosition();

};

}

#endif // _DataSetHeaderUpdater_HEADER_
