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
#ifndef _CHPUniversalEntry_HEADER_
#define _CHPUniversalEntry_HEADER_

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
#include <vector>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class stores a zone's background value */
class CHPUniversalEntry
{

private:

	float background;

public:

	CHPUniversalEntry();
	CHPUniversalEntry(float background);
	~CHPUniversalEntry();

	void Clear();

	/*! Assignment operator
	 * @param zn The zone to copy
	 * @return The new zone object
	 */
	CHPUniversalEntry operator=(CHPUniversalEntry zn);

	float GetBackground() const;

	void SetBackground(float p);
};

/*! An STL list of zones */
typedef std::vector<CHPUniversalEntry> CHPUniversalEntryVector;

/*! iterator of CHPUniversalEntrys */
typedef std::vector<CHPUniversalEntry>::iterator CHPUniversalEntryVectorIt;

}

#endif // _CHPUniversalEntry_HEADER_
