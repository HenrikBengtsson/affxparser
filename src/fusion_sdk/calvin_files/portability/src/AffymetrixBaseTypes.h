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

#ifndef _AffymetrixBaseTypes_HEADER_
#define _AffymetrixBaseTypes_HEADER_

/*! \file AffymetrixBaseTypes.h This file provides type definitions for atom types.
 */ 

/* Define the type for the Windows platform. */
#ifdef WIN32

#ifndef __MINGW32__

/*! An 8 bit signed number */
typedef	char int8_t;

/*! A 16 bit signed number */
typedef	short int int16_t;

/*! A 32 bit signed number */
typedef	int int32_t;

/*! A 64 bit signed number */
typedef long long int int64_t;

/*! An 8 bit unsigned number */
typedef	unsigned char u_int8_t;

/*! A 16 bit unsigned number */
typedef	unsigned short int u_int16_t;

/*! A 32 bit unsigned number */
typedef	unsigned int u_int32_t;

/*! A 64 bit unsigned number */
typedef unsigned long long int u_int64_t;

#else

/*! An 8 bit unsigned number */
typedef unsigned char u_int8_t;

/*! A 16 bit unsigned number */
typedef unsigned short int u_int16_t;

/*! A 32 bit unsigned number */
typedef unsigned int u_int32_t;

/*! A 64 bit unsigned number */
typedef unsigned long long int u_int64_t;

#endif

#else

/* Use the sys/types.h for non Windows platforms */
#include <sys/types.h>

#endif

#endif // _AffymetrixBaseTypes_HEADER_
