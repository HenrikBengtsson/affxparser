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

#ifndef _Coords_HEADER_
#define _Coords_HEADER_

#include "AffymetrixBaseTypes.h"
#include <vector>

/*! \file Coords.h This file defines array coordinate types.
*/

namespace affymetrix_calvin_utilities
{

/*! Defines a convention for a rectangle mapped onto a region */
enum RectanglePositions{
	UpperLeft = 0,		/*! The upper left corner. */
	UpperRight,			/*! The upper right corner. */
	LowerRight,			/*! The lower right corner. */
	LowerLeft			/*! The lower left corner. */
};

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

/*! Defines a region with floating point coordinates */
struct FRegion
{
	/*! Clears the region. */
	void Clear(){ pts.clear(); }

	/*! A vector of points */
	std::vector<FPoint> pts;
};

/*! Defines a floating point rectangle */
struct FRect
{
	/* Empty constructor */
	FRect() { upperleft.x = 0.0f, upperleft.y = 0.0f; upperright = lowerright = lowerleft = upperleft; }

	/*! Cast constructor from FRegion */
	FRect(const FRegion& r)
	{
		upperleft.x = 0.0f, upperleft.y = 0.0f; upperright = lowerright = lowerleft = upperleft;
		if (r.pts.size()>=4)
		{
		upperleft = r.pts.at(UpperLeft); upperright = r.pts.at(UpperRight);
		lowerright = r.pts.at(LowerRight); lowerleft = r.pts.at(LowerLeft);
		}
	}

	/*! Cast operator to FRegion */
	operator FRegion()
	{
		FRegion r;
		r.pts.resize(4);
		r.pts[UpperLeft] = upperleft; r.pts[UpperRight] = upperright; r.pts[LowerRight] = lowerright; r.pts[LowerLeft] = lowerleft;
		return r;
	}

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

/*! Defines an integral location */
struct Point
{
	/*! The X coordinate */
	int32_t x;

	/*! The Y coodinate */
	int32_t y;

	/*! Equality test */
	bool operator==(const Point& lhs)const { return (x==lhs.x && y==lhs.y); }
	/*! Inequality test */
	bool operator!=(const Point& lhs)const { return !(*this==lhs); }
};

/*! Defines an integral location */
struct PointU16_t
{
	/*! The X coordinate */
	u_int16_t x;

	/*! The Y coodinate */
	u_int16_t y;
};

/*! Defines an integral region */
struct Region
{
	/*! Clears the region. */
	void Clear(){ pts.clear(); }

	/*! A vector of points. */
	std::vector<Point> pts;
};

/*! Defines an integral rectagle */
struct Rect
{
	/*! Empty constructor */
	Rect() { upperleft.x = 0; upperleft.y = 0; upperright = lowerright = lowerleft = upperleft; }

	/*! Cast constructor from a Region */
	Rect(const Region& r)
	{
		upperleft.x = 0; upperleft.y = 0; upperright = lowerright = lowerleft = upperleft;
		if (r.pts.size()>=4)
		{
			upperleft = r.pts.at(UpperLeft); upperright = r.pts.at(UpperRight);
			lowerright = r.pts.at(LowerRight); lowerleft = r.pts.at(LowerLeft);
		}
	}

	/*! Cast operator to Region */
	operator Region()
	{
		Region r;
		r.pts.resize(4);
		r.pts[UpperLeft] = upperleft; r.pts[UpperRight] = upperright; r.pts[LowerRight] = lowerright; r.pts[LowerLeft] = lowerleft;
		return r;
	}

	/*! Tests if the rectangle is empty */
	bool IsEmpty() const
	{
		if (upperleft == upperright && lowerleft == lowerright && upperleft == lowerleft)
			return true;
		return false;
	}

	/*! The upper left cooridnate */
	Point upperleft;

	/*! The upper right coordinate */
	Point upperright;

	/*! The lower right coordinate */
	Point lowerright;

	/*! The lower left coordinate */
	Point lowerleft;
};

/*! An STL vector or floating point regions. */
typedef std::vector<FRegion> FRegionVector;

/*! An STL vector of regions. */
typedef std::vector<Region> RegionVector;

}

#endif
