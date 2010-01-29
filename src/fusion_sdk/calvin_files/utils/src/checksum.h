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


#ifndef _checksum_HEADER
#define _checksum_HEADER

/*! \file checksum.h This file provides functions for computing a checksum.
 */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <stddef.h>
//

namespace affymetrix_calvin_utilities
{

/*! A class to provide checksum functions */
class CheckSum
{
public:
	/*!
	* Computes a one's complement checksum.
	*
	* @param addr The memory address of the data.
	* @param size The number of bytes pointed to by the memory address.
	* @return The ones compliment checksum.
	*/
	static u_int16_t OnesComplementCheckSum(void *addr, size_t size);
};

};

#endif
