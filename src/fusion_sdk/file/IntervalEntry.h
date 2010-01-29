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

/*! \file IntervalEntry.h This file defines types to store a sequence interval. */

#ifndef _IntervalEntry_HEADER_
#define _IntervalEntry_HEADER_

#include <cstring>
#include <list>
#include <string>
//

/*! A type to define a sequence interval. */
typedef struct _IntervalEntry
{
	/*! The sequence name. */
	std::string seq;

	/*! The probe set name. */
	std::string probeSetName;

	/*! The start position. */
	int start;

	/*! The stop position. */
	int stop;

	/*! The overlap score. */
	float overlap;

	/*! The strand, either "+" or "-". */
	char strand;

	/*! The size of the region. Assumes 0 based index for start and 1 based for stop.
	 * @return The size of the region.
	 */
	int size() const { return stop - start; }

} IntervalEntry;

/*! An STL list of interval entries. */
typedef std::list<IntervalEntry> IntervalEntryList;

/*! An STL list iterator of interval entries. */
typedef std::list<IntervalEntry>::iterator IntervalEntryListIt;

/*! An STL list iterator of interval entries. */
typedef std::list<IntervalEntry>::const_iterator IntervalEntryListConstIt;


#endif

