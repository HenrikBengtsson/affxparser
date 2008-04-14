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

#include "OutputMessageStream.h"
#include <iostream>

using namespace std;

/*
 * Construct the object.
 */
OutputMessageStream::OutputMessageStream(int level, std::ostream *strm)
{
	verbosity = level;
	outstr = strm;
}

/*
 * Write the message if the level is high enough
 */
void OutputMessageStream::Write(int level, const std::string &msg)
{
	if (level <= verbosity)
		*outstr << msg;
}
