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


#ifndef _FamilialMultiDataData_HEADER_
#define _FamilialMultiDataData_HEADER_

/*! \file FamilialMultiDataData.h This file provides types to store results for a familial file.
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

/*! Stores the segment overlap from a familial file. */
typedef struct _FamilialSegmentOverlap
{
    /*! The type of segment; the name of the data set in which the segment appears in the CYCHP file. */
    std::string segmentType;

    /*! The key identifying the sample from the Samples data set. */
    u_int32_t referenceSampleKey;

    /*! The ID of the segment of the reference sample. */
    std::string referenceSegmentID;

    /*! The key identifying the sample from the Samples data set. */
    u_int32_t familialSampleKey;

    /*! The ID of the segment of the compare sample. */
    std::string familialSegmentID;

} FamilialSegmentOverlap;

/*! Stores information about the sample for a familial file. */
typedef struct _FamilialSample
{
	/*! Local arbitrary unique sample identifier used within the file. */
	u_int32_t sampleKey;

	/*! The identifier of the ARR file associated with the sample. If no ARR file was used in generating the associated CYCHP files, this value will be the empty string. */
	std::string arrID;

	/*! The identifier of the CYCHP file containing the sample data. */
	std::string chpID;

	/*! The filename (not the complete path) of the CYCHP file containing the sample data. */
	std::wstring chpFilename;

	/*! The role of the identified sample, such as “index”, “mother”, or “father”. */
	std::string role;

	/*! The call of whether the assigned role is correct. */
	bool roleValidity;

	/*! The confidence that the assigned role is correct */
	float roleConfidence;

} FamilialSample;

}

#endif
