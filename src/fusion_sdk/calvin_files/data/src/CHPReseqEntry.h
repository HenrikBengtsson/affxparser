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
#ifndef _CHPReseqEntry_HEADER_
#define _CHPReseqEntry_HEADER_

/*! \file CHPReseqEntry.h Defines a class to store the resequencing analyis results. */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_io
{


/*! The force call was made due to no signal threshold. */
#define CC_NO_SIGNAL_THR_FORCE_CALL 'N'

/*! The force call was made due to weak signal threshold. */
#define CC_WEAK_SIGNAL_THR_FORCE_CALL 'W'

/*! The force call was made due to saturation level. */
#define CC_SATURATION_LEVEL_FORCE_CALL 'S'

/*! The force call was made due to quality score threshold. */
#define CC_QUALITY_SCORE_THR_FORCE_CALL 'Q'

/*! The force call was made due to failed both trace and sequence profiles. */
#define CC_TRACE_AND_SEQUENCE_PROFILES_FORCE_CALL 'F'

/*! The force call was made due to base reliability threshold. */
#define CC_RELIABILITY_THR_FORCE_CALL 'B'


/*! A structure to hold a force call, its position and reason.
 *
 * A force call is the call the algorithm would have made if the thresholds
 * were not applied.
 */
typedef struct _CHPReseqForceCall
{
	/*! The position (index) of the call. */
	int position;

	/*! The call at the given position. */
	char call;

	/*! The reason for the call. */
	char reason;

} CHPReseqForceCall;

/*! A structure to hold a base call at a given position (index). */
typedef struct _CHPReseqOrigCall
{
	/*! The position (index) of the call. */
	int position;

	/*! The call at the given position. */
	char call;

} CHPReseqOrigCall;

/*! A structure to hold the base call and score. */
typedef struct _CHPReseqEntry
{
	/*! The call. */
	char call;

	/*! The score for the call. */
	float score;

} CHPReseqEntry;

}

#endif
