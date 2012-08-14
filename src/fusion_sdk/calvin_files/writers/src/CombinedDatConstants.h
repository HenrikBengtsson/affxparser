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


#ifndef _CombinedDatConstants_HDR_
#define _CombinedDatConstants_HDR_

#include "calvin_files/parameter/src/AffymetrixParameterConsts.h"

namespace affymetrix_calvin_io
{
/*! Defines the static attribute name for the probe array type of the physical array. */
#define GRID_STATUS_PARAM_NAME L"affymetrix-grid-status"

#define GRID_UPPER_LEFT_X L"affymetrix-grid-upper-left-x"
#define GRID_UPPER_LEFT_Y L"affymetrix-grid-upper-left-y"
#define GRID_UPPER_RIGHT_X L"affymetrix-grid-upper-right-x"
#define GRID_UPPER_RIGHT_Y L"affymetrix-grid-upper-right-y"
#define GRID_LOWER_LEFT_X L"affymetrix-grid-lower-left-x"
#define GRID_LOWER_LEFT_Y L"affymetrix-grid-lower-left-y"
#define GRID_LOWER_RIGHT_X L"affymetrix-grid-lower-right-x"
#define GRID_LOWER_RIGHT_Y L"affymetrix-grid-lower-right-y"

#define AFFY_WAVELENGTH			L"affymetrix-channel-wavelength"
#define AFFY_WAVELENGTH_NAME	L"affymetrix-channel-wavelength-name"
#define AFFY_FILTER_WAVELENGTH	L"affymetrix-filter-wavelength"

#define MULTI_CHANNEL			"affymetrix-calvin-multi-scan-acquisition"


/*! Defines the static attribute name for HTGlobal grid header */
#define HTGLOBAL_GRID_STATUS_PARAM_NAME L"GridStatus"

#define HTGLOBAL_GRID_UPPER_LEFT_X L"Upper left x"
#define HTGLOBAL_GRID_UPPER_LEFT_Y L"Upper left y"
#define HTGLOBAL_GRID_UPPER_RIGHT_X L"Upper right x"
#define HTGLOBAL_GRID_UPPER_RIGHT_Y L"Upper right y"
#define HTGLOBAL_GRID_LOWER_LEFT_X L"Lower left x"
#define HTGLOBAL_GRID_LOWER_LEFT_Y L"Lower left y"
#define HTGLOBAL_GRID_LOWER_RIGHT_X L"Lower right x"
#define HTGLOBAL_GRID_LOWER_RIGHT_Y L"Lower right y"

const int NumGridValues = 8;		// 4 corners * 2 co-ordinate = 8
static const std::wstring GridValueNames[NumGridValues] = 
{
	GRID_UPPER_LEFT_X,
	GRID_UPPER_LEFT_Y,
	GRID_UPPER_RIGHT_X,
	GRID_UPPER_RIGHT_Y,
	GRID_LOWER_RIGHT_X, 
	GRID_LOWER_RIGHT_Y, 
	GRID_LOWER_LEFT_X, 
	GRID_LOWER_LEFT_Y
};
static const std::wstring GridStatus = GRID_STATUS_PARAM_NAME;


static const std::wstring HTGridValueNames[NumGridValues] = 
{
	HTGLOBAL_GRID_UPPER_LEFT_X,
	HTGLOBAL_GRID_UPPER_LEFT_Y,
	HTGLOBAL_GRID_UPPER_RIGHT_X,
	HTGLOBAL_GRID_UPPER_RIGHT_Y,
	HTGLOBAL_GRID_LOWER_RIGHT_X, 
	HTGLOBAL_GRID_LOWER_RIGHT_Y, 
	HTGLOBAL_GRID_LOWER_LEFT_X, 
	HTGLOBAL_GRID_LOWER_LEFT_Y
};
static const std::wstring HTGridStatus = HTGLOBAL_GRID_STATUS_PARAM_NAME;

}

#endif
