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

/*! \file CalvinCHPSignalFileUpdater.h Provides interfaces to update data in a "Calvin" binary "signal" data file. */

#ifndef _CalvinCHPSignalFileUpdater_HEADER_
#define _CalvinCHPSignalFileUpdater_HEADER_

#include "DataSetUpdater.h"

namespace affymetrix_calvin_io
{

/*! Provides interfaces to update data in a "Calvin" binary "signal" data file.
 * This class assumes that the file has been completely written and that
 * only existing data groups/sets/rows/cols are being modified.
 */
class CalvinCHPSignalFileUpdater : public DataSetUpdater
{
public:
	/*! Constructor */
	CalvinCHPSignalFileUpdater();

	/*! Destructor */
	~CalvinCHPSignalFileUpdater();

	/*! Update the signal value for the given row
	 * @param row The row index.
	 * @param signal The new signal value.
	 */
	void UpdateSignal(int row, float signal);
};

}

#endif // _CalvinCHPSignalFileUpdater_HEADER_
