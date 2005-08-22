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

/////////////////////////////////////////////////////////////////
//
// Commentary:
//
// * On unix, <stdint.h> defines a number of types which
//   are of a known bit size which a programmer can use when
//   the size of the type is important.  Normally in IO.
//
// * A user can include this file in place of <stdint.h>
//   or they can test for _WIN32 and include one file or the other.
//   It will work either way.
//
//   #ifdef _WIN32
//   #include "affy-base-types.h"
//   #else
//   #include <stdint.h>
//   #endif
//
// * You should not use "long" if you can help it.  
//   The size of that type changes depending on the cpu.
//
/////////////////////////////////////////////////////////////////

#ifndef _AFFY_BASE_TYPE_H
#define _AFFY_BASE_TYPE_H 1

/*! \file affy-base-types.h This file defines types for consistency across platforms.
 */

/* "WIN32" should always be defined by MS C++  */
/* It shouldnt be defined on any other platform */
#ifndef WIN32

/* we are on a unix system -- use the native <stdint.h>... */
/* ...except on a sun which doesnt have it. (but they are defined) */
#ifndef sun
#include <stdint.h>
#endif

#else

/* We are on windows -- define the types. */
#ifndef __int8_t_defined
#define __int8_t_defined 1

/*! An 8 bit signed integer */
typedef	char          int8_t;

/*! A 16 bit signed integer */
typedef	short int     int16_t;

/*! A 32 bit signed integer */
typedef	int           int32_t;

#ifndef __VC60__
/*! A 64 bit signed integer */
typedef long long int int64_t;
#endif /* __VC60__ */

/*! An 8 bit unsigned integer. */
typedef	unsigned char          uint8_t;

/*! A 16 bit unsigned integer. */
typedef	unsigned short int     uint16_t;

/*! A 32 bit unsigned integer. */
typedef	unsigned int           uint32_t;

#ifndef __VC60__
/*! A 64 bit unsigned integer. */
typedef unsigned long long int uint64_t;
#endif /* __VC60__ */

#endif /* __int8_t_defined */

#endif /* WIN32 */
#endif /* affy-base-types.h */
