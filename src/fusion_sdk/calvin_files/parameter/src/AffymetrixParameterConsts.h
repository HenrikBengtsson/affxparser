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

#ifndef _AffymetrixParameterConsts_HEADER_
#define _AffymetrixParameterConsts_HEADER_

/*! \file AffymetrixParameterConsts.h This file defines constant names for static attributes.
 */

namespace affymetrix_calvin_parameter
{
/*! Defines the static attribute name for the probe array type of the physical array. */
#define ARRAY_TYPE_PARAM_NAME L"affymetrix-array-type"

/*! Defines the static attribute name for the barcode of the physical array. */
#define ARRAY_BARCODE_PARAM_NAME L"affymetrix-array-barcode"

/*! Defines the static attribute name for the array lot number of the physical array. */
#define ARRAY_LOT_PARAM_NAME L"affymetrix-array-lot"

/*! Defines the static attribute name for the expiration date of the physical array. */
#define ARRAY_EXPIRE_DATE_PARAM_NAME L"affymetrix-array-expiration-date"

/*! Defines the static attribute name for the barcode of the array plate. */
#define PLATE_BARCODE_PARAM_NAME L"affymetrix-plate-barcode"

/*! Defines the static attribute name for the plate type of the array plate. */
#define PLATE_TYPE_PARAM_NAME L"affymetrix-plate-type"

/*! Defines the static attribute name for the row location of the array plate well. */
#define PLATE_WELL_ROW_PARAM_NAME L"affymetrix-plate-well-row"

/*! Defines the static attribute name for the column location of the array plate well. */
#define PLATE_WELL_COL_PARAM_NAME L"affymetrix-plate-well-col"

/*! Defines the static attribute name for the algorithm name. */
#define ALGORITHM_NAME_PARAM_NAME L"affymetrix-algorithm-name"

/*! Defines the static attribute prefix for algorithm parameter names */
#define ALGORITHM_PARAM_NAME_PREFIX L"affymetrix-algorithm-param-"

/*! CDF Data Type Expression */
#define AFFY_EXPR_PS "affymetrix-expression-probesets"

/*! CDF Data Type Genotyping */
#define AFFY_GENO_PS "affymetrix-genotyping-probesets"

/*! CDF Data Type Tag */
#define AFFY_TAG_PS "affymetrix-tag-probesets"

/*! CDF Data Type Resequencing */
#define AFFY_RESEQ_PS "affymetrix-resequencing-probesets"

/*! CDF Data Type Control */
#define AFFY_CNTRL_PS "affymetrix-control-probesets"

/*! Defines US English locale. */
#define US_ENGLISH_LOCALE L"US English"
}

#endif
