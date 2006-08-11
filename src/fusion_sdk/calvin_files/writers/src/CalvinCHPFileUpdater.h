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

/*! \file CalvinCHPFileUpdater.h Provides interfaces to update data in a "Calvin" binary "signal" data file. */

#ifndef _CalvinCHPFileUpdater_HEADER_
#define _CalvinCHPFileUpdater_HEADER_

#include <fstream>
#include "DataSetUpdater.h"

namespace affymetrix_calvin_io
{

/*! Provides interfaces to update data in a "Calvin" binary "signal" data file.
 * This class assumes that the file has been completely written and that
 * only existing data groups/sets/rows/cols are being modified.
 */
class CalvinCHPFileUpdater : public DataSetUpdater
{
public:
	/*! Constructor */
	CalvinCHPFileUpdater();

	/*! Destructor */
	~CalvinCHPFileUpdater();

	/*! Open CHP signal file and initialize it.
	 * @param fileName The name of the CHP signal file.
	 */
	void OpenCHPFile(const char *fileName);

	/*! Seek to appropriate file position and update expression signal
	 * @param row The row index.
	 * @param quantification The new quantification value.
	 */
	void UpdateExpressionQuantification(int row, float quantification);

	/*! Seek to appropriate file position and update genotype entry
	 * @param row The row index.
	 * @param call CHP call representation.
	 * @param confidence CHP confidence value.
	 * @param RAS1 CHP RAS1 value.
	 * @param RAS2 CHP RAS2 value.
	 * @param aaCall CHP aaCall value.
	 * @param abCall CHP abCall value.
	 * @param bbCall CHP bbCall value.
	 * @param noCall CHP noCall value.
	 */
	void UpdateGenotypeEntry(int row, u_int8_t call, float confidence, float RAS1, float RAS2, float aaCall, float abCall, float bbCall, float noCall);

	/*! Close CHP signal file. */
	void CloseCHPFile();

private:
	// CHP signal file
	std::ofstream *m_CHPFile;
};

}

#endif // _CalvinCHPFileUpdater_HEADER_
