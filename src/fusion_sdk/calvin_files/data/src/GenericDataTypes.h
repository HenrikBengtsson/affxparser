/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#ifndef _GenericDataTypes_HEADER_
#define _GenericDataTypes_HEADER_

/*! \file GenericDataTypes.h Defines constants for each specific data file type.
 */

#include "AffymetrixGuid.h"

namespace affymetrix_calvin_data
{
/*! Defines an identifier for the scan acquisition data file. */
static const affymetrix_calvin_utilities::AffymetrixGuidType SCAN_ACQUISITION_DATA_TYPE = "affymetrix-calvin-scan-acquisition";

/*! Defines an identifier for the intensity data file. */
static const affymetrix_calvin_utilities::AffymetrixGuidType INTENSITY_DATA_TYPE = "affymetrix-calvin-intensity";

/*! Defines an identifier for the expression results data file. */
static const affymetrix_calvin_utilities::AffymetrixGuidType EXPRESSION_RESULTS_DATA_TYPE = "affymetrix-calvin-expression";

/*! Defines an identifier for the genotyping results data file. */
static const affymetrix_calvin_utilities::AffymetrixGuidType GENOTYPING_RESULTS_DATA_TYPE = "affymetrix-calvin-genotyping";

/*! Defines an identifier for the resequencing results data file. */
static const affymetrix_calvin_utilities::AffymetrixGuidType RESEQUENCING_RESULTS_DATA_TYPE = "affymetrix-calvin-resequencing";

/* Defines an identifier for the tag/universal results data file. */
static const affymetrix_calvin_utilities::AffymetrixGuidType TAG_RESULTS_DATA_TYPE = "affymetrix-calvin-tag";
}

#endif // _GenericDataTypes_HEADER_
