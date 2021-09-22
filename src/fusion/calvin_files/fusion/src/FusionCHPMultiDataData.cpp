////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 Affymetrix, Inc.
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


#include "calvin_files/fusion/src/FusionCHPMultiDataData.h"
//
#include "calvin_files/parsers/src/CHPMultiDataFileReader.h"
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_io;

/*! Used to register the CHP reader. */
FusionCHPMultiDataData::Reg FusionCHPMultiDataData::reg;

/*! The class name. */
static AffymetrixGuidType ObjectName = "FusionCHPMultiDataData";

/*
 * Construct the class.
 */
FusionCHPMultiDataData::FusionCHPMultiDataData()
{
}

/*
 * Clean up any used memory.
 */
FusionCHPMultiDataData::~FusionCHPMultiDataData()
{
	Clear();
}

/*
 * Just call read.
 */
bool FusionCHPMultiDataData::ReadHeader()
{
	return Read();
}

/*
 * Read the file.
 */
bool FusionCHPMultiDataData::Read()
{
	CHPMultiDataFileReader reader;
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
FusionCHPMultiDataData * FusionCHPMultiDataData::FromBase(FusionCHPData *chip)
{
	if (chip != NULL && chip->GetObjectName() == ObjectName)
		return (FusionCHPMultiDataData *)chip;
	return NULL;
}

AffymetrixGuidType FusionCHPMultiDataData::GetObjectName()
{
	return ObjectName;
}
