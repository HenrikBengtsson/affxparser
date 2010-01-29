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


#include "calvin_files/writers/src/CalvinCHPQuantificationDetectionFileUpdater.h"
//

using namespace affymetrix_calvin_io;
using namespace std;

/*! The data set index. */
#define QUANTIFICATION_DETECTION_DATA_SET 0

/*! The data group index. */
#define QUANTIFICATION_DETECTION_DATA_GROUP 0

/*! The quantification column. */
#define QUANTIFICATION_DETECTION_QUANTIFICATION_COLUMN 1

/*! The quantification column. */
#define QUANTIFICATION_DETECTION_DETECTION_COLUMN 2

/*
 * Initialize any needed members.
 */
CalvinCHPQuantificationDetectionFileUpdater::CalvinCHPQuantificationDetectionFileUpdater() : DataSetUpdater()
{
}

/*
 * Clean up.
 */
CalvinCHPQuantificationDetectionFileUpdater::~CalvinCHPQuantificationDetectionFileUpdater()
{
}

/*
 * Update the value for the given row.
 */
void CalvinCHPQuantificationDetectionFileUpdater::UpdateQuantification(int row, float quantification)
{
	Update(QUANTIFICATION_DETECTION_DATA_GROUP, QUANTIFICATION_DETECTION_DATA_SET, row, QUANTIFICATION_DETECTION_QUANTIFICATION_COLUMN, quantification);
}

/*
 * Update the value for the given row.
 */
void CalvinCHPQuantificationDetectionFileUpdater::UpdateDetection(int row, float detection)
{
	Update(QUANTIFICATION_DETECTION_DATA_GROUP, QUANTIFICATION_DETECTION_DATA_SET, row, QUANTIFICATION_DETECTION_DETECTION_COLUMN, detection);
}
