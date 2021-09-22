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


#ifndef _FileHeader_HEADER_
#define _FileHeader_HEADER_

/*! \file FileHeader.h This file defines a class to act as a data container for the generic file header
 */

#include "calvin_files/data/src/DataGroupHeader.h"
#include "calvin_files/data/src/GenericDataHeader.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffyStlCollectionTypes.h"
//
#include <cstring>
#include <string>
#include <vector>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

static const u_int8_t MAGIC_NUM = 59;
static const u_int8_t CALVINIOVERSION = 1;

/*! This class defines a data container for the generic file header */
class FileHeader
{
public:
	
	FileHeader();
	~FileHeader() { Clear(); }

private:

	/*! filename */
	std::string filename;
	u_int8_t magic;
	u_int8_t version;
	DataGroupHdrVector dataGroupHdrs;
	GenericDataHeader genericHdr;
	/*! Number of data dataGroups in the file */
	int32_t numDataGroups;
	/*! Position of the first DataGroup. */
	u_int32_t firstDataGroupFilePos;

public:

	void Clear();
	/*!  */
	void SetFilename(const std::string &p);
	/*!  */
	std::string GetFilename() const;
	u_int8_t GetMagicNumber() const;
	u_int8_t GetVersion() const;
	/*! Get the number of DataGroupHeaders added */
	int32_t GetDataGroupCnt() const;
	void AddDataGroupHdr(const DataGroupHeader &p);
	/*! Get a DataGroupHeader by index.  Max index < GetDataGroupCnt */
	DataGroupHeader& GetDataGroup(int32_t index);
	const DataGroupHeader& GetDataGroupConst(int32_t index) const;
	void GetDataGroupIts(DataGroupHdrIt &begin, DataGroupHdrIt &end);
	void SetGenericDataHdr(const GenericDataHeader &p);
	GenericDataHeader* GetGenericDataHdr();
	/*! Finds a DataGroupHeader by name.
	 *	@param name The name of the DataGroup
	 *  @return A pointer to the DataGroupHeader.  If not found, the return is 0.
	 */
	DataGroupHeader* FindDataGroupHeader(const std::wstring& name);
	/*! Get the number of DataGroups in a file.*/
	int32_t GetNumDataGroups() const { return numDataGroups; }
	/*! Set the number of DataGroups.  Set when reading a file */
	void SetNumDataGroups(int32_t value) { numDataGroups = value; }
	/*! Get the file position to the first DataGroup */
	u_int32_t GetFirstDataGroupFilePos() { return firstDataGroupFilePos; }
	/*! Set the file postion to the first DataGroup. Method should be protected.  It is set when the object is being read. */
	void SetFirstDataGroupFilePos(u_int32_t value) { firstDataGroupFilePos = value; }
};

}

#endif // _FileHeader_HEADER_
