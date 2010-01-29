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


#include "calvin_files/parsers/src/CDFFileReader.h"
//
#include "calvin_files/parsers/src/GenericFileReader.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_exceptions;

/*
 * Constructor
 */
CDFFileReader::CDFFileReader()
{
}

/*
 * Destructor
 */
CDFFileReader::~CDFFileReader()
{
}

/*
 */
void CDFFileReader::Read(CDFData& data, ReadMode mode)
{
	data.Clear();
	//data.GetGenericData().UseMemoryMapping(false);
	GenericFileReader reader;
	if (fileName.empty())
		fileName = data.GetFilename();
	reader.SetFilename(fileName);
	reader.ReadHeader(data.GetGenericData(), GenericFileReader::ReadNoDataGroupHeader);

	switch(mode)
	{
	case ReadSequential:
		data.PrepareForSequentialAccess();
		break;
	case ReadByProbeSetNumber:
		data.PrepareForAccessByProbeSetIndex();
		break;
	case ReadByProbeSetName:
		data.PrepareForAccessByProbeSetName();
		break;
	default:
		data.PrepareForSequentialAccess();
		break;
	}
}

