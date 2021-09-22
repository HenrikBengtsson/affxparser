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


#include "calvin_files/array/src/ArrayMedia.h"
//

using namespace affymetrix_calvin_array;

/*! The GeneChip cartridge. */
#define MEDIA_TYPE_CARTRIDGE std::wstring(L"Cartridge")

/*! A 96 well plate or strip. */
#define MEDIA_TYPE_PLATE_OR_STRIP std::wstring(L"PlateOrStrip")

/*
 * Convert the media type to a string.
 */
std::wstring affymetrix_calvin_array::MediaToString(ArrayMedia media)
{
	switch (media)
	{
	case CartridgeMedia:
		return MEDIA_TYPE_CARTRIDGE;
		break;

	case PlateOrStripMedia:
		return MEDIA_TYPE_PLATE_OR_STRIP;
		break;

	default:
		return L"";
		break;
	}
}

/*
 * Convert the string to media type.
 */
ArrayMedia affymetrix_calvin_array::MediaFromString(const std::wstring &media)
{
	if (media == MEDIA_TYPE_CARTRIDGE)
		return CartridgeMedia;
	
	else if (media == MEDIA_TYPE_PLATE_OR_STRIP)
		return PlateOrStripMedia;

	else
		return CartridgeMedia;
}
