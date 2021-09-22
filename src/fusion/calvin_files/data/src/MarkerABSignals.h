////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008 Affymetrix, Inc.
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

#ifndef _Marker_AB_Signals_HEADER_
#define _Marker_AB_Signals_HEADER_

/*! \file MarkerABSignals.h This file provides types to hold marker AB peak results.
 */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//

namespace affymetrix_calvin_data
{

/*! Holds the marker AB signal results */
typedef struct _MarkerABSignals
{
	/*! The index to the probe set ids in the main data set. */
	u_int32_t index;

	/* additional metric */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} MarkerABSignals;

}

#endif
