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


#ifndef _AffymetrixFusionGrcFile_HEADER_
#define _AffymetrixFusionGrcFile_HEADER_

/*! \file FusionGrcFileReader.h This file provides fusion classes for reading GRC files. */

#include "file/GridControlData.h"
//

namespace affymetrix_fusion_io
{

/*! This class provides GRC file reading capabilities.
 *
 * This class will read either the GCOS or Calvin format GRC files.
 */
class FusionGrcFileReader 
{

public:
	/*! Constructor */
    FusionGrcFileReader();

	/*! Destructor */
    ~FusionGrcFileReader();

	/*! Reads the contents of a GRC file.
	 * @param fileName The name of the file to read.
	 * @param data A reference to an object that will receive information from the file.
	 * @return True if successful.
	 */
	bool Read(const std::string &fileName, affymetrix_grid_control::GridControlData& data);

};

};
#endif // _AffymetrixFusionGrcFile_HEADER_
