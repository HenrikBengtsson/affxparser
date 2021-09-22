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

/// @file This is used to put in the decls for windows DLLs
///       It expands to nothing when APT_LIB_EXPORTS is not set.

#ifndef _PORTABILITY_APT_WIN_DLL_H_
#define _PORTABILITY_APT_WIN_DLL_H_


#ifdef WIN32
#ifdef APT_LIB_EXPORTS // when building DLL, target project defines this macro
#define APTLIB_API __declspec(dllexport)
#elif defined(APT_LIB_IMPORTS) // when using DLL, client project defines this macro
#define APTLIB_API __declspec(dllimport)
#else // when building or using target static library, or whatever: define it as nothing
#define APTLIB_API
#endif // APT_EXPORT
#else
#define APTLIB_API
#endif // WIN32

#endif

