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
// * On unix, <stdint.h> defines a number of api calls 
//   with an operating-system specific calling syntax.
//
// * A user can include this file in place of <unistd.h>
//   or they can test for _WIN32 and include one file or the other.
//   It will work either way.
//
//   #ifdef _WIN32
//   #include "affy-system-api.h"
//   #else
//   #include <unistd.h>
//   #endif
//
/////////////////////////////////////////////////////////////////

#ifndef _AFFY_SYSTEM_API_H
#define _AFFY_SYSTEM_API_H 1

/*! @file affy-system-api.h This file defines operating system API functions for consistency across platforms.
 */

#ifdef _MSC_VER

//#pragma warning(disable: 4996) // ignore deprecated functions warning
#include <process.h>
#define snprintf _snprintf
#define getpid _getpid
#define gethostid() 0

#else

#include <unistd.h>

#endif /* _MSC_VER */

#endif /* affy-system-api.h */
