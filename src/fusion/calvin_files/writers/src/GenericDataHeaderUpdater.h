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


#ifndef _GenericDataHeaderUpdater_HEADER_
#define _GenericDataHeaderUpdater_HEADER_

/*! \file GenericDataHeaderUpdater.h This file defines a class that updates the GenericDataHdr of an existing file.
 */

#include "calvin_files/data/src/FileHeader.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{
/*! This class updates the GenericDataHeader of an existing file with new information. */
class GenericDataHeaderUpdater
{

public:
	static const int32_t FIELD_LEN_SIZE = sizeof(int32_t);

	/*! Constructor */
	GenericDataHeaderUpdater();

	/*! Destructor */
	~GenericDataHeaderUpdater();

	/*! Update the file with the new GenericDataHeader information.
	 *	The current implementation just updates the FileId.
	 *	@param fileStream An output stream open with std::ios::out|std::ios::binary|std::ios::in.
	 *	@param updateHeader A GenericDataHeader with information to update the file.
	 *	@param currentHdr The GenericDataHeader object with the information currently in the file.
	 *	@return Returns true if the FileId could be udpated.
	 */
	bool Update(std::ofstream& fileStream, GenericDataHeader& updateHeader, GenericDataHeader& currentHdr);
	
	/*! Update the FileId.  Only update the FileId if update and current fileIDs are the same size.
	 *	@return Returns true if successful.
	 */
	bool UpdateFileId();

	/*! Update the parameter list.
	 *	Can only update types that have the same name and type and value size in the source and target
	 */
	void UpdateParameters();

private:
	/*! Get the size of the FileHeader in bytes.
	 *	@return The size of the FileHeader.
	 */
	int32_t GetFileHeaderSize();
	/*! Get the size of the GenericDataHeader up to the start of the parameter list.
	 *	@return The size of the GenericDataHeader up to the start of the parameter list.
	 */
	int32_t GetBytesFromGenericDataHdrStartToParameterList();

private:
	/*! Open output filestream */
	std::ofstream* os;
	/*! A GenericDataHeader with information to use to update the target*/
	GenericDataHeader* updateHdr;
	/*! The GenericDataHeader with current information */
	GenericDataHeader* currentHdr;
};

}

#endif // _GenericDataHeaderUpdater_HEADER_
