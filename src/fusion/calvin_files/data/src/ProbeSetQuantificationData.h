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


#ifndef _ProbeSetQuantificationData_HEADER_
#define _ProbeSetQuantificationData_HEADER_

/*! \file ProbeSetQuantificationData.h This file provides types to hold quantification results.
 */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_data
{

/*! Stores data for a quantification of a probe set. */
typedef struct _ProbeSetQuantificationData
{
	/*! The name of the probe set. */
	std::string name;

    /*! The probe set id. */
    int32_t id;

	/*! The quantification associated to the name. */
	float quantification;

} ProbeSetQuantificationData;

}

#endif

