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


#ifndef _Cyto_GenotypeCall_HEADER_
#define _Cyto_GenotypeCall_HEADER_

/*! \file CytoGenotypeCallMultiDataType.h This file provides types to hold genotyping results for cytgo arrays.
 */

#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//

namespace affymetrix_calvin_data
{

/*! Holds the marker AB signal results */
typedef struct _CytoGenotypeCallData
{
	/*! The index relative to the probeset in the CopyNumber 
	data set of the ProbeSet data group (0 based). */
	u_int32_t index;

	 /*! The call. */
    u_int8_t call;

	/*! The cpnfidence */
    float confidence;

	 /*! The forced call. */
    u_int8_t forcedCall;

	/*! The A signal */
    float aSignal;

	/*! The B signal */
    float bSignal;

	/*! The signal strength */
    float signalStrength;
	
	/*! The contrast */
    float contrast; 

	/*! The extra metrics. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} CytoGenotypeCallData;

}

#endif
