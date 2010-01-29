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

/*! \file BEDFileWriter.h This file defines a class to write BED files. */

#ifndef _BEDFileWriter_HEADER_
#define _BEDFileWriter_HEADER_

#include "file/BEDFileData.h"
//

namespace affxbed
{

/*! Provies write capabilities for BED files. */
class BEDFileWriter : public BEDFileData
{
public:
	/*! Constructor */
	BEDFileWriter();

	/*! Destructor */
	~BEDFileWriter();

	/*! Writes the entries to a file.
	 * @return True if successful.
     */
	bool Write();
};

}

#endif
