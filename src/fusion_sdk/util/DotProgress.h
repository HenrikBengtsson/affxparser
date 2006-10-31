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

/* \file DotProgress.h Provides a base class for displaying progress. */

#ifndef _DotProgress_HEADER_
#define _DotProgress_HEADER_

#include "Progress.h"
#include <ostream>
#include <iostream>

/*! Provides a class for showing progress with dots written to an output stream. */
class DotProgress : public Progress
{
private:
	/*! Where to stream the messages. */
	std::ostream *outstr;

	/*! What level of messages is wanted, larger num == more msgs */
	int verbosity;

	/*! How often do we print a dot when dot() is called? */
	int dotMod;
	
	/*! How many times has dot() been called? */
	int dotCount;

public:

	/*! Constructor
	 * @param strm Where to stream the dots
	 */
	DotProgress(std::ostream *strm = &std::cout);
  ~DotProgress() { }

	/*! Sets the upper limit of the range of the progress display.
	 * @param level - level of verbosity desired.
	 * @param mx The maximum number of steps in the task.
	 * @param inc The number of steps performed before the progress display is updated.
	 */
	void SetStepProperties(int level, int mx, int inc);

	/*! Steps the progress ahead by one count.
	 * @param level - What level of verbosity this message should be printed at.
	 */
	void Step(int level);
};

#endif
