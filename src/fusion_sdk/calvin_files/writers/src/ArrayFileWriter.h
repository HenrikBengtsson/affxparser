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

#ifndef _ArrayFileWriter_HEADER_
#define _ArrayFileWriter_HEADER_

/*! \file ArrayFileWriter.h This file provides interfaces to write an array file.
 */

#include "FileWriteException.h"
#include "AffymetrixBaseTypes.h"
#include "ArrayData.h"
#include <string>
#include <fstream>

namespace affymetrix_calvin_io
{

/*! This class provides interfaces to write an array file. */
class ArrayFileWriter
{
public:
	/*! Constructor */
	ArrayFileWriter();

	/*! Destructor */
	~ArrayFileWriter();

protected:

	/*! An identifier to the type of data stored in the file */
	affymetrix_calvin_utilities::AffymetrixGuidType dataTypeIdentifier;

public:
	/*! Writes the array object to an array file.
	 *
	 * @param fileName The name of the array file to write.
	 * @param arrayData The array data to write to the file.
	 * @return True if the file was successfully written.
	 */
	bool Write(const std::string &fileName, affymetrix_calvin_array::ArrayData &arrayData);
};

};

#endif // _ArrayFileWriter_HEADER_
