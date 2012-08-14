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


#include "calvin_files/parsers/src/GenericDataHeaderReader.h"
//
#include "calvin_files/data/src/GenericDataHeader.h"
#include "calvin_files/parsers/src/FileInput.h"
//
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
GenericDataHeaderReader::GenericDataHeaderReader(std::ifstream& fs)
: fileStream(fs)
{
}

/*
 * Reads the GenericDataHeader and all parent GenericDataHeaders.
 * Calls Read() with true to retain originqal intent.
 */
void GenericDataHeaderReader::Read(GenericDataHeader& gdh)
{
	// retain original calling logic.
	Read(gdh, true);
}

/*
 * Reads the GenericDataHeader and all parent GenericDataHeaders.
 * In call to AddNameValParam, doUniqueAdds is passed.
 */
void GenericDataHeaderReader::Read(GenericDataHeader& gdh, bool doUniqueAdds)
{
	// Read data type identifier
	gdh.SetFileTypeId(FileInput::ReadString8(fileStream));

	// Read file identifier
	gdh.SetFileId(FileInput::ReadString8(fileStream));

	// Read file creation time
	gdh.SetFileCreationTime(FileInput::ReadString16(fileStream));

	// Read locale
	gdh.SetLocale(FileInput::ReadString16(fileStream));

	// Read name value pairs
	u_int32_t paramCount = FileInput::ReadUInt32(fileStream);
	for (u_int32_t iparam = 0; iparam < paramCount; ++iparam)
	{
		const void* mimeValue = 0;
		std::wstring name = FileInput::ReadString16(fileStream);
		int32_t mimeSize = FileInput::ReadBlob(fileStream,mimeValue);
		std::wstring type = FileInput::ReadString16(fileStream);
		ParameterNameValueType nvt(name,(void*)mimeValue, mimeSize, type);
                // deleting a "const void*" generates a warning under gcc.
                // cast to a "char*" to quiet it.
		delete[] (char*)mimeValue;
		gdh.AddNameValParam(nvt,doUniqueAdds);
	}

	// Read number of generic data parent header
	u_int32_t numParents = FileInput::ReadUInt32(fileStream);

	// Read each parent header in turn - this needs to be recursive
	for (u_int32_t iparent = 0; iparent < numParents; ++iparent)
	{
		GenericDataHeader parentGDH;
		Read(parentGDH);
		gdh.AddParent(parentGDH);
	}
}
