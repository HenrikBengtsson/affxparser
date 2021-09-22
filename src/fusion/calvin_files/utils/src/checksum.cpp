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


#include "calvin_files/utils/src/checksum.h"
//

using namespace affymetrix_calvin_utilities;

/*
 * Store the one's complement of the checksum in the header of the data.
 * To validate the data, compute the one's complement of the data. The
 * resulting checksum should be zero.
 */
u_int16_t CheckSum::OnesComplementCheckSum(void *addr, size_t size)
{
	u_int16_t *pdata = (u_int16_t *)addr;
	u_int32_t sum = 0;
	u_int16_t checksum;

	// Compute Internet Checksum for "size" bytes
	// beginning at location "addr".
	while( size > 1 )
	{
		sum += *pdata++;
		size -= 2;
	}

	// Add left-over byte, if any
	if( size > 0 )
		sum += * (unsigned char *) pdata;

	// Fold 32-bit sum to 16 bits
	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);

	// Compute the one's complement of the checksum.
	checksum = (u_int16_t) ~sum;
	return checksum;
}

