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

#ifndef _Guid_HEADER_
#define _Guid_HEADER_

/*! \file Guid.h This file provides functions for creating globally unique identifiers.
 */

#include <string>
#include <list>

namespace affxutil
{

/*! The GUID type */
typedef std::string GuidType;

/*! An STL list of guid types */
typedef std::list<GuidType> GuidTypeList;

/*! This class provides functions for creating globally unique identifiers. */
class Guid
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

#endif // _Guid_HEADER_
