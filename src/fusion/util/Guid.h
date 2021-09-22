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

#ifndef _UTIL_GUID_H_
#define _UTIL_GUID_H_

/// @file Guid.h This file provides functions for creating globally unique identifiers.

//
#include "portability/apt-win-dll.h"
//
#include <cstring>
#include <list>
#include <string>

///
/// RFC4122 v4  Guid => 09fdee5a-b19e-4101-0668-0840519ad701
/// Classic APT Guid => 0000007361-1232759575-1336980168-0774759233-0407464966
///
/// The format of the guids affxutil::GenerateNewGuid() will make.
/// Comment it out for the old format.
#define AFFY_GUID_FORMAT_RFC4122 1

/// The env var to read the seed to srand from on unix.
/// This is handy for debugging.
#define AFFY_DEBUG_SRAND_SEED_ENVVAR "APT_DEBUG_SRAND_SEED"


namespace affxutil
{
  /// @brief     Ensure that srand has been seeded.
  ///            Safe to call multiple times.
  void ensure_srand_seeded();
  
  /// @brief     Seed srand with data from the system env
  ///            Or from 'APT_DEBUG_SRAND_SEED' on unix.
  ///            It will always set the srand seed when called.
  void affy_seed_srand();

/*! The GUID type */
typedef std::string GuidType;

/*! An STL list of guid types */
typedef std::list<GuidType> GuidTypeList;

/*! This class provides functions for creating globally unique identifiers. */
/// @todo: Why do we need GuidType and this class?
class APTLIB_API Guid
{
public:
	/*! Constructor */
	Guid();

	/*! Destructor */
	~Guid();

	/*! Generates a new GUID.
	 *
	 * @return The new GUID.
	 */
	static GuidType GenerateNewGuid();
};

};

#endif // _UTIL_GUID_H_
