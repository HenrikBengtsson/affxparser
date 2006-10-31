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


#include "FileHeaderUpdater.h"
#include "GenericDataHeaderUpdater.h"

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
FileHeaderUpdater::FileHeaderUpdater()
{
}

/*
 * Destructor
 */
FileHeaderUpdater::~FileHeaderUpdater()
{
}

/*
 * Update the file with new FileHeader information
 */
bool FileHeaderUpdater::Update(std::ofstream& os, FileHeader& updateHdr, FileHeader& currentHdr)
{
	// There is nothing at this time to update in the FileHeader
	// Move on to the GenericDataHeader

	GenericDataHeaderUpdater genUpdater;
	return genUpdater.Update(os, *updateHdr.GetGenericDataHdr(), *currentHdr.GetGenericDataHdr());
}

