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

#include "R_affx_constants.h"

/*! Microsoft Visual Studio C++ doesn't implement std::min() and std::max()
  due to possible conflict with min() and max() defined in windows.h. */
#ifdef _MSC_VER   // vc++ version
#include <windows.h>
#define Max(a,b) max(a,b)
#define Min(a,b) min(a,b)
#else            // assume other people implement the standard
#include <algorithm>
#define Max(a,b) std::max(a,b)
#define Min(a,b) std::min(a,b)
#endif           // _MSC_VER

#ifdef WIN64
#include <stdint.h>
#endif

/* "WIN32" should always be defined by MS C++  */
/* It shouldnt be defined on any other platform */
#ifndef WIN32

/* we are on a unix system -- use the native <stdint.h>... */
/* ...except on a sun which doesnt have it. (but they are defined) */
#ifndef sun
#include <stdint.h>
#endif

#else

#ifndef __MINGW32__

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

#else

/*! A 16 bit unsigned integer. */
typedef	unsigned short int     uint16_t;

/*! A 32 bit unsigned integer. */
typedef	unsigned int           uint32_t;

#endif /* __MINGW32__ */

#endif /* WIN32 */

//////////

// I didnt see a standard, so lets pretend that everything
// is linux and use their defs

// These set up the numeric values to be used.
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

// now we set BYTE_ORDER for the platform if it hasnt been set already
#ifndef BYTE_ORDER
// Big guys first...
#ifdef __ppc__
#define BYTE_ORDER BIG_ENDIAN
#endif
#ifdef __ppc64__
#define BYTE_ORDER BIG_ENDIAN
#endif
#ifdef __sparc__
#define BYTE_ORDER BIG_ENDIAN
#endif
// ...then the little ones.
#ifdef __i386__
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#ifdef __x86_64__
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#ifdef WIN32
#define BYTE_ORDER LITTLE_ENDIAN
#endif
// BYTE_ORDER
#endif

// Check that one of the cases above set BYTE_ORDER.
// Otherwise some compiled code might be BIG_ENDIAN and some might be LITTLE_ENDIAN,
// depending on how the test is written.
#ifndef BYTE_ORDER
#error "BYTE_ORDER is not set"
#endif

//////////

/* This is also in "AffymetrixBaseTypes.h"
 * Be sure to only get it once. (keep that def there in sync.)
 */
/// @todo remove the duplicate def in "AffymetrixBaseTypes.h"
#ifndef _AFFY_TYPE_PUNNED_
#define _AFFY_TYPE_PUNNED_

/* Use a union to allow type-punning without having to
 * use "-fnostrict-alias" on the entire file.
 * See the GCC info page about -fstrict-alias for the details.
 * We used to do "*(uint32_t)&floatvar" but that would break
 * on some versions of gcc.
 */

union type_punned {
  float v_float;
  int v_int32;
  unsigned int v_uint32;
};
#endif


#endif /* affy-base-types.h */
