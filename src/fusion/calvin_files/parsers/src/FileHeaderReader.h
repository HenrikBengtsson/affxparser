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


#ifndef _FileHeaderReader_HEADER_
#define _FileHeaderReader_HEADER_

/*! \file FileHeaderReader.h This file provides methods to read a generic file header.
 */

#include "calvin_files/data/src/FileHeader.h"
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
	/*! This class reads the FileHeader of the generic data file. */
class FileHeaderReader
{
public:
	/*! Constructor
	 *	@param fs Open fstream positioned at the start of the file.
	 *	@param fh FileHeader object to fill.
	 */
	FileHeaderReader(std::ifstream& fs, FileHeader &fh);

	/*! Reads the FileHeader. 
	 * @exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 * @exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	 */
	void Read();

	/*! Gets the number of DataGroups in the file.
	 *	@return Number of DataGroups.
	 */
	u_int32_t GetDataGroupCnt() const { return dataGroupCnt; }

	/*! Gets the file position of the first DataGroup header. */
	u_int32_t GetFirstDataGroupFilePos() const { return firstDataGroupFilePos; }

protected:
	/*! Reads the magic number from the file. 
	 * @exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	*/
	void ReadMagicNumber();

	/*! Reads the generic file version number from the file.
	 * @exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 */
	void ReadVersion();

	/*! Reads the DataGroup count from the file. */
	void ReadDataGroupCnt();

	/*! Reads the file position of the first DataGroup. */
	void ReadFirstDataGroupFilePos();

	/*! Reads the GenericDataHeader from the file. */
	void ReadGenericDataHdr();

protected:
	/*! A reference to the file stream. */
	std::ifstream& fileStream;

	/*! FileHeader object to fill. */
	FileHeader& header;

	/*! Number of DataGroups. */
	u_int32_t dataGroupCnt;

	/*! Position of the first DataGroup. */
	u_int32_t firstDataGroupFilePos;
};

}

#endif	//_FileHeaderReader_HEADER_
