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


#ifndef _ArrayAuditActionTypes_HEADER_
#define _ArrayAuditActionTypes_HEADER_

/*! \file ArrayAuditActionTypes.h Defines constants for various array file audit actions.
 */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//

namespace affymetrix_calvin_array
{

/*! Initial creation of an array file. */
static const int32_t ARRAY_FILE_CREATEION_ACTION = 1;

/*! Attribute update action. */
static const int32_t ARRAY_FILE_ATTRIBUTE_UPDATE_ACTION = 2;

/*! Fluidics instrument control action. */
static const int32_t FLUIDICS_INSTRUMENT_CONTROL_ACTION = 3;

/*! Scanner instrument control action. */
static const int32_t SCAN_INSTRUMENT_CONTROL_ACTION = 4;

};

#endif // _ArrayAuditActionTypes_HEADER_
