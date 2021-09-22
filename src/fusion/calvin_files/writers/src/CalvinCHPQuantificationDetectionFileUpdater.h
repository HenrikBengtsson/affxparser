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


/*! \file CalvinCHPQuantificationDetectionFileUpdater.h Provides interfaces to update data in a "Calvin" binary "quantification/detection" data file. */

#ifndef _CalvinCHPQuantificationDetectionFileUpdater_HEADER_
#define _CalvinCHPQuantificationDetectionFileUpdater_HEADER_

#include "calvin_files/writers/src/DataSetUpdater.h"
//

namespace affymetrix_calvin_io
{

/*! Provides interfaces to update data in a "Calvin" binary "quantification/detection" data file.
 * This class assumes that the file has been completely written and that
 * only existing data groups/sets/rows/cols are being modified.
 */
class CalvinCHPQuantificationDetectionFileUpdater : public DataSetUpdater
{
public:
	/*! Constructor */
	CalvinCHPQuantificationDetectionFileUpdater();

	/*! Destructor */
	~CalvinCHPQuantificationDetectionFileUpdater();

	/*! Update the quantification value for the given row
	 * @param row The row index.
	 * @param quantification The new quantification value.
	 */
	void UpdateQuantification(int row, float quantification);

	/*! Update the detection value for the given row
	 * @param row The row index.
	 * @param pvalue The new p-value.
	 */
	void UpdateDetection(int row, float pvalue);
};

}

#endif // _CalvinCHPQuantificationDetectionFileUpdater_HEADER_
