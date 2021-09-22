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


#ifndef _CDFQCProbeSetInformation_HEADER_
#define _CDFQCProbeSetInformation_HEADER_

#ifdef _MSC_VER
#include <windows.h>
#endif

/*! \file CDFQCProbeSetInformation.h This file defines the CDFQCProbeSetInformation class.
 */

#include "calvin_files/data/src/DataSet.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_io
{

class CDFQCProbeInformation;

/*! Class that exposes the CDF QC probe set information */
class CDFQCProbeSetInformation
{
public:
	/*! Constructor */
	CDFQCProbeSetInformation();
	/*! Destructor */
	~CDFQCProbeSetInformation();

	/*! Get the QC type - DataSet name */
	std::wstring GetQCProbeSetType() const;
	/*! Get the number of cells in the probe set - number of rows in the DataSet */
	int32_t GetNumCells() const;
	/*! Get the QC probe information */
	void GetProbeInformation(int index, CDFQCProbeInformation& info);

protected:
	void SetDataSet(DataSet* ds);

protected:
	/*! DataSet pointer */
	DataSet* dataSet;

	friend class CDFData;
};

}
#endif // _CDFQCProbeSetInformation_HEADER_
