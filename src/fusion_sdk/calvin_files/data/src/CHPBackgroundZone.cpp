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


#include "calvin_files/data/src/CHPBackgroundZone.h"
//

using namespace affymetrix_calvin_io;

CHPBackgroundZone::CHPBackgroundZone() 
{
	Clear();
}

CHPBackgroundZone::CHPBackgroundZone(float x, float y, float bg, float smooth)
{
	centerX = x;
	centerY = y;
	background = bg;
	smoothFactor = smooth;
}

CHPBackgroundZone::~CHPBackgroundZone() {}

void CHPBackgroundZone::Clear() 
{
	centerX = 0.0;
	centerY = 0.0;
	background = 0.0;
	smoothFactor = 0.0;
}

CHPBackgroundZone CHPBackgroundZone::operator=(CHPBackgroundZone zn)
	{
		centerX = zn.GetCenterX(); 
		centerY = zn.GetCenterY(); 
		background = zn.GetBackground();
		smoothFactor = zn.GetSmoothFactor();
		return *this; 
	}

	float CHPBackgroundZone::GetCenterX() const
	{
		return centerX;
	}

	float CHPBackgroundZone::GetCenterY() const
	{
		return centerY;
	}

	float CHPBackgroundZone::GetBackground() const
	{
		return background;
	}

	float CHPBackgroundZone::GetSmoothFactor() const
	{
		return smoothFactor;
	}

	void CHPBackgroundZone::SetCenterX(float p)
	{
		centerX = p;
	}

	void CHPBackgroundZone::SetCenterY(float p)
	{
		centerY = p;
	}

	void CHPBackgroundZone::SetBackground(float p)
	{
		background = p;
	}

	void CHPBackgroundZone::SetSmoothFactor(float p)
	{
		smoothFactor = p;
	}

