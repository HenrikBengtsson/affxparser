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
#ifndef _CHPBackgroundZone_HEADER_
#define _CHPBackgroundZone_HEADER_

#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/Coords.h"
//
#include <cstring>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class stores a zone's background value */
class CHPBackgroundZone
{

private:
	/*! The X coordinate of the center of the zone. */
	float centerX;

	/*! The Y coordinate of the center of the zone. */
	float centerY;

	/*! The zone's background value */
	float background;

	/*! The smoothing factor used to calculate the zone backgrounds */
	float smoothFactor;

public:

	CHPBackgroundZone();
	CHPBackgroundZone(float x, float y, float bg, float smooth);
	~CHPBackgroundZone();

	void Clear();

	/*! Assignment operator
	 * @param zn The zone to copy
	 * @return The new zone object
	 */
	CHPBackgroundZone operator=(CHPBackgroundZone zn);

	float GetCenterX() const;

	float GetCenterY() const;

	float GetBackground() const;

	float GetSmoothFactor() const;

	void SetCenterX(float p);

	void SetCenterY(float p);

	void SetBackground(float p);

	void SetSmoothFactor(float p);
};

/*! An STL list of zones */
typedef std::vector<CHPBackgroundZone> CHPBackgroundZoneVector;

/*! iterator of CHPBackgroundZones */
typedef std::vector<CHPBackgroundZone>::iterator CHPBackgroundZoneVectorIt;

}

#endif // _CHPBackgroundZone_HEADER_
