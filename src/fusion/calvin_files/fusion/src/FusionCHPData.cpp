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


#include "calvin_files/fusion/src/FusionCHPData.h"
//
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/utils/src/FileUtils.h"
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_io;

/*! The head of the CHP data objects. */
FusionCHPDataReg *FusionCHPDataReg::m_Head = NULL;

/*
 * Store the ids
 */
void FusionCHPDataReg::SetFileTypeIds(const AffymetrixGuidTypeList &fileTypeIds)
{
	fileTypeIdentifiers = fileTypeIds;
}

/*
 * Set the members. The head is the pointer to this class.
 */
FusionCHPDataReg::FusionCHPDataReg() : m_Next(m_Head)
{
	m_Head = this;
}

FusionCHPDataReg::~FusionCHPDataReg()
{
	fileTypeIdentifiers.clear();
}

/*
 * Read the header and return the file type id. If the file fails to read
 * it must be an old CHP file, just return a blank for the file type id.
 */
FusionCHPData *FusionCHPDataReg::CreateObject(const AffymetrixGuidType &id)
{
	// Find the matching CHP data object.
	for (FusionCHPDataReg *p=m_Head; p != NULL; p = p->m_Next)
	{
		AffymetrixGuidTypeList::iterator it;
		for (it=p->fileTypeIdentifiers.begin(); it!=p->fileTypeIdentifiers.end(); ++it)
		{
			if (*it == id)
			{
				FusionCHPData *chp = p->MakeObject();
				chp->fileTypeIdentifiers = p->fileTypeIdentifiers;
				chp->fileTypeIdentifier = id;
				if (chp)
				{
					return chp;
				}
			}
		}
	}

	// Go back now and find the generic reader (the one with no file type identifiers)
	for (FusionCHPDataReg *p=m_Head; p != NULL; p = p->m_Next)
	{
		if (p->fileTypeIdentifiers.size() == 0)
		{
			FusionCHPData *chp = p->MakeObject();
			chp->fileTypeIdentifier = id;
			return chp;
		}
	}

	return NULL;
}

/*
 * Read the guid from the file.
 */
bool ReadGuidFromFile(const std::string &fileName, AffymetrixGuidType &guid)
{
	if (FileUtils::Exists(fileName.c_str()) == false)
		return false;

	GenericData data;
	GenericFileReader reader;
	try
	{
		reader.SetFilename(fileName);
		reader.ReadHeader(data);
		guid = data.Header().GetGenericDataHdr()->GetFileTypeId();
		return true;
	}
	catch (...)
	{
		guid = "";
		return true;
	}
}

/*
 * Read the file type ID from the header of the file.
 * Now find the data object with the matching id.
 * Read the file and return the pointer.
 */
FusionCHPData *FusionCHPDataReg::ReadHeader(const std::string &fileName)
{
	AffymetrixGuidType fileTypeId;
	if (ReadGuidFromFile(fileName, fileTypeId) == false)
		return NULL;

	FusionCHPData *chp = FusionCHPDataReg::CreateObject(fileTypeId);
	if (chp)
	{
		chp->SetFileName(fileName.c_str());
		if (chp->ReadHeader() == false)
		{
			delete chp;
			chp = NULL;
		}
	}
	return chp;
}

/*
 * Read the file type ID from the header of the file.
 * Now find the data object with the matching id.
 * Read the file and return the pointer.
 */
FusionCHPData *FusionCHPDataReg::Read(const std::string &fileName)
{
	AffymetrixGuidType fileTypeId;
	if (ReadGuidFromFile(fileName, fileTypeId) == false)
		return NULL;
	FusionCHPData *chp = CreateObject(fileTypeId);
	if (chp)
	{
		chp->SetFileName(fileName.c_str());
		if (chp->Read() == false)
		{
			delete chp;
			chp = NULL;
		}
	}
	return chp;
}
