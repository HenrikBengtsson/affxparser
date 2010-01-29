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


#ifndef _AffymetrixGuid_HEADER_
#define _AffymetrixGuid_HEADER_

/*! \file AffymetrixGuid.h This file provides functions for creating globally unique identifiers.
 */

#include <cstring>
#include <list>
#include <string>
//

namespace affymetrix_calvin_utilities
{

/*! The GUID type */
typedef std::string AffymetrixGuidType;

/*! An STL list of guid types */
typedef std::list<AffymetrixGuidType> AffymetrixGuidTypeList;

/*! This class provides functions for creating globally unique identifiers. */
class AffymetrixGuid
{
public:
	/*! Constructor */
	AffymetrixGuid();

	/*! Destructor */
	~AffymetrixGuid();

	/*! Generates a new GUID.
	 *
	 * @return The new GUID.
	 */
	static AffymetrixGuidType GenerateNewGuid();
};

};

#endif // _AffymetrixGuid_HEADER_
