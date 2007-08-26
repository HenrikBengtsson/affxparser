////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#ifndef _ProbeSetMultiDataData_HEADER_
#define _ProbeSetMultiDataData_HEADER_

/*! \file ProbeSetMultiDataData.h This file provides types to hold MultiData results.
 */

#include <vector>
#include <string>
#include "AffymetrixBaseTypes.h"
#include "ParameterNameValueType.h"

namespace affymetrix_calvin_data
{

/*! Stores data for a genotype result of a probe set. */
typedef struct _ProbeSetMultiDataGenotypeData
{
	/*! The name of the probe set. */
	std::string name;

    /*! The call. */
    u_int8_t call;

	/*! The confidence of the call. */
	float confidence;

	/*! Other metrics associated with the call. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ProbeSetMultiDataGenotypeData;

/*! Stores data for an expression result of a probe set. */
typedef struct _ProbeSetMultiDataExpressionData
{
	/*! The name of the probe set. */
	std::string name;

    /*! The quantification of the probe set. */
	float quantification;

	/*! Other metrics associated with the call. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ProbeSetMultiDataExpressionData;

}

#endif

