/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "FusionCHPGenericData.h"
#include "FileException.h"
#include "GenericFileReader.h"


using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;

/*! Used to register the CHP reader. */
FusionCHPGenericData::Reg FusionCHPGenericData::reg;


/*
 * Convert to the CHP pointer.
 */
FusionCHPGenericData * FusionCHPGenericData::FromBase(FusionCHPData *chip)
{
	if (chip == NULL)
		return NULL;
	if (chip->FileTypeIdentifier() == "")
		return NULL;
	return (FusionCHPGenericData *)chip;
}

FusionCHPGenericData::FusionCHPGenericData()
{
}

FusionCHPGenericData::~FusionCHPGenericData()
{
	Clear();
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

