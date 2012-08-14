////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011 Affymetrix, Inc.
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
//
// affy/sdk/portability/apt-no-warnings.h ---
//
// $Id$
//

/// @file include this file to disable some of the warnings
///       we have in APT and havent gotten around to fixing yet.

// @todo Including this file should be regarded as a bug,
//       but better than missing a warning hidden in the rest.

#ifndef _APT_NO_WARNINGS_H_
#define _APT_NO_WARNINGS_H_

#if defined(_MSC_VER)

// to turn these warnings back on, use
// #pragma warning( LVL : NUMBER )
// where "LVL" is the level at which it should be reported.

// turn off warnings about signed and unsigned comparisons.
#pragma warning( disable: 4018 )

// dont complain about "unsafe" functions.
// for this to work, this must be defined BEFORE including the standard libaries.
// #define _CRT_SECURE_NO_WARNINGS 1
#pragma warning( disable: 4996 )

//#pragma warning( 1 : 4996 )
#endif

#endif // _APT_NO_WARNINGS_H_
