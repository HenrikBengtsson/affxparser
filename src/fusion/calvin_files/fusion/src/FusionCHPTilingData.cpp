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


#include "calvin_files/fusion/src/FusionCHPTilingData.h"
//
#include "calvin_files/parsers/src/CHPTilingFileReader.h"
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_io;

/*! Used to register the CHP reader. */
FusionCHPTilingData::Reg FusionCHPTilingData::reg;

/*! The class name. */
static AffymetrixGuidType ObjectName = "FusionCHPTilingData";

/*
 * Construct the class.
 */
FusionCHPTilingData::FusionCHPTilingData()
{
}

/*
 * Clean up any used memory.
 */
FusionCHPTilingData::~FusionCHPTilingData()
{
	chpData.Clear();
}

/*
 * Just call read.
 */
bool FusionCHPTilingData::ReadHeader()
{
	return Read();
}

/*
 * Read the file.
 */
bool FusionCHPTilingData::Read()
{
	CHPTilingFileReader reader;
	reader.SetFilename(filename);
	try
	{
		reader.Read(chpData);
		return true;
	}
	catch(...)
	{
		return false;
	}
}

/*
 * Convert to the CHP pointer.
 */
FusionCHPTilingData * FusionCHPTilingData::FromBase(FusionCHPData *chip)
{
	if (chip != NULL && chip->GetObjectName() == ObjectName)
		return (FusionCHPTilingData *)chip;
	return NULL;
}

AffymetrixGuidType FusionCHPTilingData::GetObjectName()
{
	return ObjectName;
}

/*
 * Get the id of the file (only valid for Command Console "calvin" files)
 */
AffymetrixGuidType FusionCHPTilingData::FileId()
{
	return chpData.GetGenericData().FileIdentifier();
}

/*
 * Returns the GenericData object associated with a Calvin file, NULL for GCOS files.
 */
GenericData *FusionCHPTilingData::GetGenericData()
{
	return &chpData.GetGenericData();
}
