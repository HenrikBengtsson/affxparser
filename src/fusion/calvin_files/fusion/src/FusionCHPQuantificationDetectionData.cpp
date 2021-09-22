////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#include "calvin_files/fusion/src/FusionCHPQuantificationDetectionData.h"
//
#include "calvin_files/parsers/src/CHPQuantificationDetectionFileReader.h"
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_io;

/*! Used to register the CHP reader. */
FusionCHPQuantificationDetectionData::Reg FusionCHPQuantificationDetectionData::reg;

/*! The class name. */
static AffymetrixGuidType ObjectName = "FusionCHPQuantificationDetectionData";

/*
 * Construct the class.
 */
FusionCHPQuantificationDetectionData::FusionCHPQuantificationDetectionData()
{
}

/*
 * Clean up any used memory.
 */
FusionCHPQuantificationDetectionData::~FusionCHPQuantificationDetectionData()
{
	chpData.Clear();
}

/*
 * Just call read.
 */
bool FusionCHPQuantificationDetectionData::ReadHeader()
{
	return Read();
}

/*
 * Read the file.
 */
bool FusionCHPQuantificationDetectionData::Read()
{
	CHPQuantificationDetectionFileReader reader;
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
FusionCHPQuantificationDetectionData * FusionCHPQuantificationDetectionData::FromBase(FusionCHPData *chip)
{
	if (chip != NULL && chip->GetObjectName() == ObjectName)
		return (FusionCHPQuantificationDetectionData *)chip;
	return NULL;
}

AffymetrixGuidType FusionCHPQuantificationDetectionData::GetObjectName()
{
	return ObjectName;
}

/*
 * Get the id of the file (only valid for Command Console "calvin" files)
 */
AffymetrixGuidType FusionCHPQuantificationDetectionData::FileId()
{
	return chpData.GetGenericData().FileIdentifier();
}

/*
 * Returns the GenericData object associated with a Calvin file, NULL for GCOS files.
 */
GenericData *FusionCHPQuantificationDetectionData::GetGenericData()
{
	return &chpData.GetGenericData();
}
