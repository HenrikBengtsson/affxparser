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


#ifndef _ChromosomeMultiDataSummaryData_HEADER_
#define _ChromosomeMultiDataSummaryData_HEADER_

/*! \file ChromosomeMultiDataSummaryData.h This file provides types to hold chromosome multi data results.
 */

#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
#include <vector>
//

namespace affymetrix_calvin_data
{

typedef struct _ChromosomeMultiDataSummaryData
{
    /*! The chromosome number. */
    u_int8_t chr;

	/*! The chromosome number for display. */
	std::string display;

	/*! The starting index */
	u_int32_t startIndex;

	/*! The number of markers */
	u_int32_t markerCount;

    /*! The minimum signal */
    float minSignal;

    /*! The maximum signal */
    float maxSignal;

    /*! The median copy number state */
    float medianCnState;

    /*! The hom frequency */
    float homFrequency;

    /*! The het frequency */
    float hetFrequency;

	/*! Other metrics. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ChromosomeMultiDataSummaryData;

}

#endif

