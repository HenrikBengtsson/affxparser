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

#ifndef _GRDFILEWRITER_H_
#define _GRDFILEWRITER_H_

/*! \file GRDFileWriter.h This file provides GRD file writing capabilities.
 */

#include "file/GRDFileData.h"
//
#include <fstream>
//

namespace affxgrd
{

/*! This class provides methods to write a GRD file. */
class CGRDFileWriter : public CGRDFileData
{
public:

	/*! Creates the file set in SetFileName and writes the header. 
	 *	@return Returns true if the file was successfully created and the header was successfully written.
	 */
	bool CreateNewFile();

	/*! Writes a center coordinate to an newly created GRD file.  May only be called after a successful call to CreateNewFile.
	 *	@return Returns true if the center coordinate was successfully written.
	 */
	bool WriteCenter(const FCOORD& center);

	/*! Closes the newly created file.
	 *	@return Returns true if the new file was successfully closed.
	 */
	bool CloseNewFile();

protected:

	/*! Writes the header to the open file.
	 *	@return Returns true if the header was sucessfully written.
	 */
	bool WriteHeader();

	/*! Writes the fixed section of the file header to the open file.
	 *	@return Returns true if the fixed header was successfully written.
	 */
	bool WriteFixedHeader();

	/*! Writes the tag header of the file header to the open file.
	 *	@return Returns true if the tag header was successfully written.
	 */
	bool WriteTagHeader();

	/*! Writes the optimized subgrid coordinates header of the file header to the open file.
	 *	@return Returns true if the optimized subgrid coordinates header was successfully written.
	 */
	bool WriteOptimumCorners();

protected:

	/*! stream for writing files */
	std::ofstream m_outstr;
};

}

#endif
