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

#include "util/DotProgress.h"
//

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
