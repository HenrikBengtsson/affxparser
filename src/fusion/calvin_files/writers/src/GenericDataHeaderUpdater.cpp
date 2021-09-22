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


#include "calvin_files/writers/src/GenericDataHeaderUpdater.h"
//
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/writers/src/FileHeaderUpdater.h"
#include "calvin_files/writers/src/FileOutput.h"
//

using namespace std;
using namespace affymetrix_calvin_io;

#define WCHAR_T_SIZE 2

/*
 * Constructor
 */

GenericDataHeaderUpdater::GenericDataHeaderUpdater()
{
	os = 0;
	updateHdr = 0;
	currentHdr = 0;
}

/*
 * Destructor
 */
GenericDataHeaderUpdater::~GenericDataHeaderUpdater()
{
}

/*
 * Update the file with the new GenericDataHeader information.
 */
bool GenericDataHeaderUpdater::Update(std::ofstream& fileStream, GenericDataHeader& updateHeader, GenericDataHeader& currentHeader)
{
	os = &fileStream;
	updateHdr = &updateHeader;
	currentHdr = &currentHeader;

	bool result = UpdateFileId();
	UpdateParameters();
	return result;
}

/*
 * Update the FileId.  Only update the FileId if update and current fileIDs are the same size.
 */
bool GenericDataHeaderUpdater::UpdateFileId()
{
	// Check if we can update the file Id; make sure the existing file Id is the same length as the new one
	if (currentHdr->GetFileId().length() == updateHdr->GetFileId().length() && updateHdr->GetFileId().length() != 0)
	{
		// Move the file stream position to the location of the file ID.
		int32_t offset = GetFileHeaderSize() + FIELD_LEN_SIZE + (int32_t)currentHdr->GetFileTypeId().length();
		os->seekp(offset, std::ios::beg);

		// Write the new ID
		FileOutput::WriteString8(*os, updateHdr->GetFileId());

		return true;
	}
	else
		return false;
}

/*
 * Update the parameter list.
 */
void GenericDataHeaderUpdater::UpdateParameters()
{
	int32_t filePos = GetFileHeaderSize() + GetBytesFromGenericDataHdrStartToParameterList();

	ParameterNameValueTypeIt begin, end;
	currentHdr->GetNameValIterators(begin, end);
	for (ParameterNameValueTypeIt ii = begin; ii != end; ++ii)
	{
		// Add name length to file offset
		filePos += FIELD_LEN_SIZE + (int32_t)ii->GetName().length()*WCHAR_T_SIZE;

		affymetrix_calvin_parameter::ParameterNameValueType nvt;
		if (updateHdr->FindNameValParam(ii->GetName(), nvt))
		{
			// Check that the types match and the values are the same size
			if (ii->GetParameterType() == nvt.GetParameterType()  && 
					ii->GetMIMEValue().Size() >= nvt.GetMIMEValue().Size())
			{
				// Safe to update - move to filePos
				os->seekp(filePos, std::ios::beg);

				// Write value
				MIMEValue mv = nvt.GetMIMEValue();
				//existing value size
				u_int32_t size = ii->GetMIMEValue().Size();
				//new value size
				u_int32_t sz;
				const void* ptr = mv.GetValue(sz);
				//write the value with reserved size
				FileOutput::WriteBlob(*os, ptr, sz, size);
			}
		}

		// Add Value length to offset
		filePos += FIELD_LEN_SIZE + ii->GetMIMEValue().Size();

		// Add type length to offset
		filePos += FIELD_LEN_SIZE + (int32_t)ii->GetMIMEType().size()*WCHAR_T_SIZE;
	}
}

/*
 * Get the size of the FileHeader in bytes.
 */
int32_t GenericDataHeaderUpdater::GetFileHeaderSize()
{
	return 2*sizeof(u_int8_t) + sizeof(u_int32_t) + sizeof(int32_t);
}

/*
 * Get the size of the GenericDataHeader up to the start of the parameter list.
 */
int32_t GenericDataHeaderUpdater::GetBytesFromGenericDataHdrStartToParameterList()
{
	return FIELD_LEN_SIZE + (int32_t)currentHdr->GetFileTypeId().length() +
		FIELD_LEN_SIZE + (int32_t)currentHdr->GetFileId().length() +
		FIELD_LEN_SIZE + (int32_t)currentHdr->GetFileCreationTime().length()*WCHAR_T_SIZE +
		FIELD_LEN_SIZE + (int32_t)currentHdr->GetLocale().length()*WCHAR_T_SIZE +
		FIELD_LEN_SIZE;
}
