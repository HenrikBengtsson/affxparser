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

/* \file Progress.h Provides a base class for displaying progress. */

#ifndef _Progress_HEADER_
#define _Progress_HEADER_

/*! Provides a base class definition for streaming back messages to the parent process. */
class Progress
{
public:
	/*! Sets the upper limit of the range of the progress display.
	 * @param level - level of verbosity desired.
	 * @param mx The maximum number of steps in the task.
	 * @param inc The number of steps performed before the progress display is updated.
	 */
	virtual void SetStepProperties(int level, int mx, int inc) = 0;
  virtual ~Progress() { }

	/*! Steps the progress ahead by one count.
	 * @param level - What level of verbosity this message should be printed at.
	 */
	virtual void Step(int level) = 0;
};

#endif
