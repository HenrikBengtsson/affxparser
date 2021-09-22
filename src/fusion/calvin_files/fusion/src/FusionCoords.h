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


#ifndef _FusionCoords_HEADER_
#define _FusionCoords_HEADER_

/*! \file FusionCoords.h This file defines the fusion coordinate classes.
 */

#include <cstring>
#include <list>
#include <string>
#include <vector>
//

namespace affymetrix_fusion_io
{

	/*! Defines a floating-point location */
	struct FPoint
	{
		/*! The x coordinate */
		float x;

		/*! The y coordinate */
		float y;

		/*! Equality test */
		bool operator==(const FPoint& lhs)const { return (x==lhs.x && y==lhs.y); }
		/*! Inequality test */
		bool operator!=(const FPoint& lhs)const { return !(*this==lhs); }
	};

	typedef std::vector<FPoint> FPointVector;

	/*! Defines a floating point rectangle */
	struct FGridCoords
	{
		/* Empty constructor */
		FGridCoords() { upperleft.x = 0.0f, upperleft.y = 0.0f; upperright = lowerright = lowerleft = upperleft; }

		/*! Tests if the rectangle is empty */
		bool IsEmpty() const
		{
			if (upperleft == upperright && lowerleft == lowerright && upperleft == lowerleft)
				return true;
			return false;
		}

		/*! The upper left coordinate */
		FPoint upperleft;

		/*! The upper right coordinate */
		FPoint upperright;

		/*! The lower right coordinate */
		FPoint lowerright;

		/*! The lower left coordinate */
		FPoint lowerleft;
	};

}

#endif	// _FusionCoords_HEADER_
