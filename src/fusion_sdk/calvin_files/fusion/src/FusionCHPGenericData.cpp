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


#include "calvin_files/fusion/src/FusionCHPGenericData.h"
//
#include "calvin_files/parsers/src/FileException.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
//


using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;

/*! Used to register the CHP reader. */
FusionCHPGenericData::Reg FusionCHPGenericData::reg;

/*! The class name. */
static AffymetrixGuidType ObjectName = "FusionCHPGenericData";

/*
 * Convert to the CHP pointer.
 */
FusionCHPGenericData * FusionCHPGenericData::FromBase(FusionCHPData *chip)
{
	if (chip != NULL && chip->GetObjectName() == ObjectName)
		return (FusionCHPGenericData *)chip;
	return NULL;
}

AffymetrixGuidType FusionCHPGenericData::GetObjectName()
{
	return ObjectName;
}

FusionCHPGenericData::FusionCHPGenericData()
{
}

FusionCHPGenericData::~FusionCHPGenericData()
{
	Clear();
}

/*
 * Get the id of the file (only valid for Command Console "calvin" files)
 */
AffymetrixGuidType FusionCHPGenericData::FileId()
{
	return genericData.FileIdentifier();
}

/*
 * Returns the GenericData object associated with a Calvin file, NULL for GCOS files.
 */
GenericData *FusionCHPGenericData::GetGenericData()
{
	return &genericData;
}

bool FusionCHPGenericData::Read()
{
	GenericFileReader reader;
	try
	{
		reader.SetFilename(filename);
		reader.Open(genericData);
		return true;
	}
	catch(...)
	{
		return false;
	}
}

bool FusionCHPGenericData::ReadHeader()
{
	return Read();
}

void FusionCHPGenericData::Clear()
{
	genericData.Clear();
}

