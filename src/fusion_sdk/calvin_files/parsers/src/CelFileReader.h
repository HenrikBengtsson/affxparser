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


#ifndef _CelFileReader_HEADER_
#define _CelFileReader_HEADER_

/*! \file CelFileReader.h This file provides methods to read a CEL data file.
 */

#include "calvin_files/data/src/CELData.h"
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
	/*! This class reads a CEL data file. It is an interpreter class.*/
class CelFileReader
{
public:
	/*! Constructor */
	CelFileReader();

	/*! Destructor */
	~CelFileReader();

public:
	/*! Gets the name of the input file.
	 *
	 * @return The name of the file to read.
	 */
	const std::string GetFilename() const { return fileName; }

	/*! Sets the name of the input file.
	 *
	 * @param name The name of the file to read.
	 */
	void SetFilename(const std::string &name) { fileName = name; }

	/*! Reads the file header of the generic file and reads all the DataSetHeader information.
	 *
	 * @param data A reference to a GenericData object that will receive information from the file.
	 * @exception affymetrix_calvin_exceptions::FileNotFoundException The file does not exist.
	 * @exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 * @exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	 */
	void Read(CelFileData& data);

protected:
	/*! Name of the file to read */
	std::string fileName;

};

}

#endif // _CelFileReader_HEADER_
