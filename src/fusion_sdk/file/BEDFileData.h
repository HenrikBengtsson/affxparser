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

/*! \file BEDFileData.h This file defines a class to read BED files. */

#ifndef _BEDFileData_HEADER_
#define _BEDFileData_HEADER_

#include "file/IntervalEntry.h"
#include "file/TagValuePair.h"
//
#include <cstring>
#include <list>
#include <string>
//

namespace affxbed
{

/*! The first word of the browser line. */
#define BROWSER_LINE_START "browser position"

/*! The first word of the track line. */
#define TRACK_LINE_START "track name="

/*! The default track name. */
#define DEFAULT_TRACK_NAME "\"AFFX track\""


/*! This type holds the parameters and intervals for a given group of intervals. */
typedef struct _IntervalGroup
{
	/*! The parameters for the interval group. */
	TagValuePairTypeList parameters;

	/*! The intervals. */
	IntervalEntryList intervals;

	/*! Clears the structure */
	void Clear();

} IntervalGroup;

/*! An STL list of sequence data items. */
typedef std::list<IntervalGroup> IntervalGroupList;

/*! An STL constant list iterator. */
typedef IntervalGroupList::iterator IntervalGroupListIt;

/*! Stores data associated with a BED file. */
class BEDFileData
{
protected:

	/*! The file name. */
	std::string fileName;

	/*! The browser (first line of the BED file). */
	std::string browser;

	/*! The track for the bed file (second line of the BED file). */
	std::string track;

	/*! The intervals. */
	IntervalGroupList intervalGroups;

public:
	/*! Constructor */
	BEDFileData();

	/*! Destructor */
	~BEDFileData();

	/*! Clears the data in the class. */
	void Clear();

	/*! The file name.
	 * @return The file name.
	 */
	std::string &FileName() { return fileName; }

	/*! The browser (first line of the BED file).
	 * @return The browser line
	 */
	std::string &Browser() { return browser; }

	/*! The track for the bed file (second line of the BED file).
	 @ return The track line.
	 */
	std::string &Track() { return track; }

	/*! Format the browser string.
	 * @param seq The sequence name.
	 * @param start The start position.
	 * @param stop The stop position.
	 */
	void FormatBrowser(const char *seq, int start, int stop);

	/*! Format the track string.
	 * @param name The track name.
	 * @param desc The description.
	 */
	void FormatTrack(const char *name="AFFX track", const char *desc="");

	/*! The intervals.
	 * @return The intervals
	 */
	IntervalGroupList &IntervalGroups() { return intervalGroups; }

	/*! Reads the entries from the file.
	 * @return True if successful.
	 */
	bool Read();
};

}

#endif
