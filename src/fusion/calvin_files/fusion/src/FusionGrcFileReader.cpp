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


#include "calvin_files/fusion/src/FusionGrcFileReader.h"
//
#include "calvin_files/parsers/src/GridControlFileReader.h"
//
#include "file/GRCFileData.h"
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace affymetrix_grid_control;
using namespace affymetrix_calvin_exceptions;
using namespace affxgrc;
using namespace std;

/*
 * Clear class members if needed.
 */
FusionGrcFileReader::FusionGrcFileReader()
{
}

/*
 * Clear class members if needed.
 */
FusionGrcFileReader::~FusionGrcFileReader()
{
}

/*
 * Read the GRC file. First try reading it as a Calvin file
 * then as a GCOS GRC file.
 */
bool FusionGrcFileReader::Read(const string &fileName, GridControlData& data)
{
	GridControlFileReader calvinGrc;
	try
	{
		calvinGrc.Read(fileName, data);
		return true;
	}
	catch(InvalidFileTypeException e)
	{
		CGRCFileData gcosGrc;
		try
		{
			gcosGrc.SetFileName(fileName.c_str());
			if (gcosGrc.Read(data) == true)
				return true;
		}
		catch(...)
		{
		}
	}
	catch (...)
	{
	}
	return  false;
}

