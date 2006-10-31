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


#ifndef _FileHeaderUpdater_HEADER_
#define _FileHeaderUpdater_HEADER_

/*! \file FileHeaderUpdater.h This file defines a class that updates the FileHeader of an existing file.
 */

#include <fstream>
#include "FileHeader.h"

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class updates the FileHeader of an existing file with new information. */
class FileHeaderUpdater
{

public:

	/*! Constructor */
	FileHeaderUpdater();
	/*! Destructor */
	~FileHeaderUpdater();

	/*! Update the file with new FileHeader information
	 *	@param os An output stream open with std::ios::out|std::ios::binary|std::ios::in.
	 *	@param updateHdr A FileHeader with information to update the file.
	 *	@param currentHdr A FileHeader with information current in the file.
	 *	@return Returns true if successful.
	 */
	bool Update(std::ofstream& os, FileHeader& updateHdr, FileHeader& currentHdr);

};

}

#endif // _FileHeaderUpdater_HEADER_
