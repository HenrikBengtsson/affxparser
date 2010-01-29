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


#ifndef _Subgrids_HEADER_
#define _Subgrids_HEADER_

#include "calvin_files/utils/src/Coords.h"
//
#include <vector>
//

/*! \file Subgrids.h This file defines a subgrid.
*/


namespace affymetrix_calvin_utilities
{

/*! Defines a subgrid */
class Subgrid
{
public:
	int32_t row;	// ???
	int32_t col;	// ???
	FGridCoords pixelgrid;
	GridCoords cellgrid;
	//bool operator==( Subgrid& rhs );
};

typedef std::vector<Subgrid> SubgridVector;

}

#endif
