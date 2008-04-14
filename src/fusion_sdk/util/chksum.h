////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
////////////////////////////////////////////////////////////////

#ifndef _checksum_HEADER
#define _checksum_HEADER

/**
 * @file  chksum.h
 * @brief This file provides functions for computing a checksum.
 */

#include <stdlib.h>
//
#include "portability/affy-base-types.h"

namespace affxutil
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
	static uint16_t OnesComplementCheckSum(void *addr, size_t size);
};

};

#endif
