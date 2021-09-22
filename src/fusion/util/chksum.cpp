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

#include "util/chksum.h"

using namespace affxutil;

/*
 * Store the one's complement of the checksum in the header of the data.
 * To validate the data, compute the one's complement of the data. The
 * resulting checksum should be zero.
 */
uint16_t CheckSum::OnesComplementCheckSum(void *addr, size_t size)
{
	uint16_t *pdata = (uint16_t *)addr;
	uint32_t sum = 0;
	uint16_t checksum;

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
	checksum = (uint16_t) ~sum;
	return checksum;
}
