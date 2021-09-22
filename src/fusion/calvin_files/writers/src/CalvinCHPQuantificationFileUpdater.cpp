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


#include "calvin_files/writers/src/CalvinCHPQuantificationFileUpdater.h"
//

using namespace affymetrix_calvin_io;
using namespace std;

/*! The data set index. */
#define QUANTIFICATION_DATA_SET 0

/*! The data group index. */
#define QUANTIFICATION_DATA_GROUP 0

/*! The quantification column. */
#define QUANTIFICATION_COLUMN 1

/*
 * Initialize any needed members.
 */
CalvinCHPQuantificationFileUpdater::CalvinCHPQuantificationFileUpdater() : DataSetUpdater()
{
}

/*
 * Clean up.
 */
CalvinCHPQuantificationFileUpdater::~CalvinCHPQuantificationFileUpdater()
{
}

/*
 * Update the value for the given row.
 */
void CalvinCHPQuantificationFileUpdater::UpdateQuantification(int row, float quantification)
{
	Update(QUANTIFICATION_DATA_GROUP, QUANTIFICATION_DATA_SET, row, QUANTIFICATION_COLUMN, quantification);
}
