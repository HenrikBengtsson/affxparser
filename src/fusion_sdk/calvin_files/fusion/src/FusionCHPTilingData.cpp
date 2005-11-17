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

#include "FusionCHPTilingData.h"
#include "CHPTilingFileReader.h"

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_io;

/*! Used to register the CHP reader. */
FusionCHPTilingData::Reg FusionCHPTilingData::reg;

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
	if (chip)
	{
		const AffymetrixGuidTypeList &ids = chip->FileTypeIdentifiers();
		AffymetrixGuidTypeList::const_iterator it;
		for (it=ids.begin(); it!=ids.end(); ++it)
		{
			if (*it == CHP_TILING_TYPE)
			{
				return (FusionCHPTilingData *)chip;
			}
		}
	}
	return NULL;
}
