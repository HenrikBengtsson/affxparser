/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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

#include "CalvinCHPSignalFileUpdater.h"

using namespace affymetrix_calvin_io;
using namespace std;

/*! The data set index. */
#define SIGNAL_DATA_SET 0

/*! The data group index. */
#define SIGNAL_DATA_GROUP 0

/*! The signal column. */
#define SIGNAL_COLUMN 1

/*
 * Initialize any needed members.
 */
CalvinCHPSignalFileUpdater::CalvinCHPSignalFileUpdater() : DataSetUpdater()
{
}

/*
 * Clean up.
 */
CalvinCHPSignalFileUpdater::~CalvinCHPSignalFileUpdater()
{
}

/*
 * Update the value for the given row.
 */
void CalvinCHPSignalFileUpdater::UpdateSignal(int row, float signal)
{
	Update(SIGNAL_DATA_GROUP, SIGNAL_DATA_SET, row, SIGNAL_COLUMN, signal);
}
