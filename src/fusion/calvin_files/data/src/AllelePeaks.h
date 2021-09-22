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


#ifndef _AllelePeaks_HEADER_
#define _AllelePeaks_HEADER_

/*! \file AllelePeaks.h This file provides types to hold allele peak results.
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

/*! Holds the allele peak results */
typedef struct _AllelePeaks
{
	/*! The name of the marker. */
	std::string name;

	/*! The chromosome value. */
	u_int8_t chr;

	/*! The physical position. */
	u_int32_t position;

	/*! The allele peaks. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> peaks;

} AllelePeaks;

}

#endif
