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


#include "calvin_files/data/src/CDFProbeInformation.h"
//

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
CDFProbeInformation::CDFProbeInformation()
{
	xCoord = 0;
	yCoord = 0;
	listIndex = 0;
	indexPos = 0;
	baseProbe = 0;
	baseTarget = 0;
	probeLength = 0;
	probeGrouping = 0;
}


void CDFProbeInformation::CopyFromDataSet(DataSet* dataSet, int32_t row)
{
	dataSet->GetData(row, XCoordCol, xCoord);
	dataSet->GetData(row, YCoordCol, yCoord);
	dataSet->GetData(row, ListIndexCol, listIndex);
	dataSet->GetData(row, IndexPosCol, indexPos);
	dataSet->GetData(row, BaseProbeCol, baseProbe);
	dataSet->GetData(row, BaseTargetCol, baseTarget);
//	dataSet->GetData(row, ProbeLengthCol, probeLength);
//	dataSet->GetData(row, ProbeGroupingCol, probeGrouping);
}

