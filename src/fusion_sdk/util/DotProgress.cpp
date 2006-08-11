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

#include "DotProgress.h"

using namespace std;

DotProgress::DotProgress(std::ostream *strm)
{
	outstr = strm;
}

void DotProgress::SetStepProperties(int level, int mx, int inc)
{
	verbosity = level;
	dotMod = inc;
	dotCount = dotMod;
}

void DotProgress::Step(int level)
{
	if (level <= verbosity)
	{
		--dotCount;
		if (dotCount <= 0)
		{
			dotCount = dotMod;

			outstr->put('.');
			outstr->flush();
		}
	}
}
