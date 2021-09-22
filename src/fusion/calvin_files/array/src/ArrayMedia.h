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


#ifndef _ArrayMedia_HEADER_
#define _ArrayMedia_HEADER_

/*! \file ArrayMedia.h This file a type for defining the media type of an array. */

#include <cstring>
#include <string>
//

namespace affymetrix_calvin_array
{

/*! The type of media of the array. */
typedef enum _ArrayMedia
{
	CartridgeMedia,		/*! A GeneChip cartridge. */
	PlateOrStripMedia	/*! A 96 well plate or peg strip. */
} ArrayMedia;

/*! Converts the media type to a string.
 * @param media The media type
 * @return The string representation.
 */
std::wstring MediaToString(ArrayMedia media);

/*! Converts a string to media type.
 * @param media The string representation.
 * @return The media
 */
ArrayMedia MediaFromString(const std::wstring &media);

};

#endif // _ArrayMedia_HEADER_
