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


#ifndef _GenericDataTypes_HEADER_
#define _GenericDataTypes_HEADER_

/*! \file GenericDataTypes.h Defines constants for each specific data file type.
 */

#include "calvin_files/utils/src/AffymetrixGuid.h"
//

namespace affymetrix_calvin_data
{
/*! Defines an identifier for the scan acquisition data file. */
#define SCAN_ACQUISITION_DATA_TYPE affymetrix_calvin_utilities::AffymetrixGuidType("affymetrix-calvin-scan-acquisition")

/*! Defines an identifier for the scan acquisition data file. */
#define MULTI_SCAN_ACQUISITION_DATA_TYPE affymetrix_calvin_utilities::AffymetrixGuidType("affymetrix-calvin-multi-scan-acquisition")

/*! Defines an identifier for the intensity data file. */
#define INTENSITY_DATA_TYPE affymetrix_calvin_utilities::AffymetrixGuidType("affymetrix-calvin-intensity")

}

#endif // _GenericDataTypes_HEADER_
