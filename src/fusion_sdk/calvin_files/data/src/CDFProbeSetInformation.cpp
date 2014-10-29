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


#include "calvin_files/data/src/CDFProbeSetInformation.h"
//
#include "calvin_files/data/src/CDFData.h"
#include "calvin_files/data/src/CDFProbeGroupInformation.h"
#include "calvin_files/data/src/DataSet.h"
//

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
CDFProbeSetInformation::CDFProbeSetInformation()
{
	dataGroup = 0;
	type = Expression;
	direction = 0;
	groupCnt = 0;
	probeSetNumber = 0;
	cellsPerList = 0;
	listCnt = 0;
	cellCnt = 0;
}

/*
 * Destructor
 */
CDFProbeSetInformation::~CDFProbeSetInformation()
{
	delete dataGroup;
}

void CDFProbeSetInformation::SetDataGroup(DataGroup& dg)
{
	if (dataGroup)
		delete dataGroup;

	dataGroup = new DataGroup(dg);
	const DataGroupHeader& dgh = dataGroup->Header();

	type = Expression;
	direction = 0;
	groupCnt = 0;
	probeSetNumber = 0;
	cellsPerList = 0;
	listCnt = 0;
	cellCnt = 0;

	// Get the first DataSetHeader
	const DataSetHeader& dsh = dgh.GetDataSetConst(0);

	ParameterNameValueType nvt;

	// UnitType
	if (dsh.FindNameValParam(CDF_UNIT_TYPE, nvt))
		type = (CDFDataTypeIds)nvt.GetValueUInt8();
	
	// Direction
	if (dsh.FindNameValParam(CDF_DIRECTION, nvt))
		direction = nvt.GetValueUInt8();

	// ProbeGroup count
	groupCnt = dgh.GetDataSetCnt();

	// ProbeSetNumber
	if (dsh.FindNameValParam(CDF_PROBE_SET_NUMBER, nvt))
		probeSetNumber = nvt.GetValueUInt32();

	// CellsPerList
	if (dsh.FindNameValParam(CDF_CELLS_PER_ATOM, nvt))
		cellsPerList = nvt.GetValueUInt8();

	// Sum up the totals
	for (int32_t group = 0; group < (int32_t)groupCnt; ++group)
	{
		if (dsh.FindNameValParam(CDF_ATOMS, nvt))
		{
			listCnt += nvt.GetValueUInt32();
		}
		if (dsh.FindNameValParam(CDF_CELLS, nvt))
		{
			cellCnt += nvt.GetValueUInt32();
		}
	}
}

void CDFProbeSetInformation::GetGroupInformation(u_int32_t groupIdx, CDFProbeGroupInformation& info)
{
        // WAS if (groupIdx < 0 || ...) BUT 'groupIdx < 0'
        // is always false because 'groupIdx' is unsigned.
	if (groupIdx >= groupCnt)
		return;

	DataSet* ds = dataGroup->DataSet(groupIdx);
	info.SetDataSet(ds);
}

