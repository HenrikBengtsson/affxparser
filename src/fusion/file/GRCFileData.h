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

#ifndef _GRCFileData_HEADER_
#define _GRCFileData_HEADER_

/*! \file GRCFileData.h This file provides reading capaibilities for GRC files.
 */

//////////////////////////////////////////////////////////////////////

#include "file/GridControlData.h"
//
#include "portability/affy-base-types.h"
//
#include <cstring>
#include <fstream>
#include <list>
#include <string>
//

//////////////////////////////////////////////////////////////////////

namespace affxgrc
{

/*! The current version of the GRC file */
#define GRC_VERSION 1

/*! The magic number of the GRC file */
#define GRC_MAGIC 129

//////////////////////////////////////////////////////////////////////

/*! This class holds the coordinates of those probes used by the GCAST algorithm. */
class CGRCFileData
{
public:
	/*! Constructor */
	CGRCFileData();

	/*! Destructor */
	~CGRCFileData();

protected:
	/*! The name of the GRC file */
	std::string m_strFileName;

	/*! The version number of the file */
	int32_t m_Version;

	/*! Checks if the file is of a release format
	 *
	 * @return True if release format (not early access)
	 */
	bool IsReleaseFormat();

	/*! Reads a pre-release (early access) format GRC file.
	 *
	 * @param grid The grid control data to fill from the GRC file.
	 * @return True if successful
	 */
	bool ReadPreReleaseFormat(affymetrix_grid_control::GridControlData &grid);

	/*! Reads a release format GRC file.
	 *
	 * @param grid The grid control data to fill from the GRC file.
	 * @return True if successful
	 */
	bool ReadReleaseFormat(affymetrix_grid_control::GridControlData &grid);

public:
	/*! Sets the file name.
	 * @param name The full path to the GRC file.
	 */
	void SetFileName (const char *name) { m_strFileName = name; }

	/*! Gets the file name.
	 * @return The full path to the GRC file.
	 */
	const std::string &GetFileName() const { return m_strFileName; }

	/*! Reads the contents of the file.
	 * @param grid The grid control data to fill from the GRC file.
	 * @return True if successful.
	 */
	bool Read(affymetrix_grid_control::GridControlData &grid);

	/*! Checks for the existance of a file.
	 * @return True if the file exists.
	 */
	bool Exists();
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif
