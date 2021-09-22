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


#ifndef _AffymetrixBaseTypes_HEADER_
#define _AffymetrixBaseTypes_HEADER_

/*! \file AffymetrixBaseTypes.h This file provides type definitions for atom types.
 */ 

#include "R_affx_constants.h"

#ifdef WIN64
#include <stdint.h>
#endif

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

#else // __MINGW32__

/*! An 8 bit unsigned number */
typedef unsigned char u_int8_t;

/*! A 16 bit unsigned number */
typedef unsigned short int u_int16_t;

/*! A 32 bit unsigned number */
typedef unsigned int u_int32_t;

/*! A 64 bit unsigned number */
typedef unsigned long long int u_int64_t;

#endif // __MINGW32__

#else // WIN32

/* Use the sys/types.h for non Windows platforms */
#include <sys/types.h>

#ifndef sun
#include <stdint.h>
#else // sun

/*! An 8 bit unsigned number */
typedef unsigned char u_int8_t;

/*! A 16 bit unsigned number */
typedef unsigned short int u_int16_t;

/*! A 32 bit unsigned number */
typedef unsigned int u_int32_t;

/*! A 64 bit unsigned number */
typedef unsigned long long int u_int64_t;

#endif // sun

#endif //WIN32

//////////


/* This is also in "affy-base-types.h"
 * Be sure to only get it once.  (keep them in sync!)
 * The documentation is there too.
 */
#ifndef _AFFY_TYPE_PUNNED_
#define _AFFY_TYPE_PUNNED_

union type_punned {
  float v_float;
  int v_int32;
  unsigned int v_uint32;
};
#endif

#endif // _AffymetrixBaseTypes_HEADER_
