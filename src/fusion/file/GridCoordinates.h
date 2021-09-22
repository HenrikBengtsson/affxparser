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

#ifndef _GridCoordinates_HEADER_
#define _GridCoordinates_HEADER_

/*! \file GridCoordinates.h Defines structures to hold the coordinates of a grid. */

#include "portability/affy-base-types.h"

//////////////////////////////////////////////////////////////////////

/*! A structure to hold a grid coordinate point. */
typedef struct _CoordinatePoint
{ 
	/*! The X cooridnate */
	int32_t x;

	/*! The Y coordinate */
	int32_t y;

} CoordinatePoint; 

/*! A structure to hold the grid coordinates. */
typedef struct _GridCoordinatesType
{
	/*! The upper left corner. */
    CoordinatePoint upperleft;

	/*! The upper right corner. */
    CoordinatePoint upperright;

	/*! The lower right corner. */
    CoordinatePoint lowerright;

	/*! The lower left corner. */
    CoordinatePoint lowerleft;

} GridCoordinatesType;

//////////////////////////////////////////////////////////////////////

#endif
