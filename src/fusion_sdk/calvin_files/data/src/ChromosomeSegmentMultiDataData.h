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


#ifndef _ChromosomeSegmentMultiDataData_HEADER_
#define _ChromosomeSegmentMultiDataData_HEADER_

/*! \file ChromosomeSegmentMultiDataData.h This file provides types to store results for a segment.
 */

//
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
//

namespace affymetrix_calvin_data
{

typedef struct _ChromosomeSegmentData
{
    /*! The segment id */
    u_int32_t segmentId;

    /*! The chromosome number. */
    u_int8_t chr;

    /*! The start position on the chromosome. */
    u_int32_t startPosition;

    /*! The stop position on the chromosome. */
    u_int32_t stopPosition;

    /*! The number of markers in the segment. */
    int markerCount;

    /*! The mean marker distance. */
    u_int32_t meanMarkerDistance;

	/*! Other metrics associated with the segment. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ChromosomeSegmentData;

typedef struct _ChromosomeSegmentDataEx
{
    /*! The segment id */
    u_int32_t segmentId;

	/*! The reference sample key. */
	u_int32_t referenceSampleKey;

	/*! The familial sample key. */
	u_int32_t familialSampleKey;

    /*! The chromosome number. */
    u_int8_t chr;

    /*! The start position on the chromosome. */
    u_int32_t startPosition;

    /*! The stop position on the chromosome. */
    u_int32_t stopPosition;

    /*! The segment call */
    u_int8_t call;

    /*! The confidence in the call */
    float confidence;

    /*! The number of markers in the segment. */
    int markerCount;

	/*! The homozygosity */
	float homozygosity;

	/*! The heterozygosity */
	float heterozygosity;

	/*! Other metrics associated with the segment. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ChromosomeSegmentDataEx;

}

#endif
