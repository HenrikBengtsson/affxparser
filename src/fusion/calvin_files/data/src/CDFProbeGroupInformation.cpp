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


#include "calvin_files/data/src/CDFProbeGroupInformation.h"
//
#include "calvin_files/data/src/CDFData.h"
#include "calvin_files/data/src/CDFProbeInformation.h"
#include "calvin_files/data/src/DataSet.h"
//

using namespace affymetrix_calvin_io;

CDFProbeGroupInformation::CDFProbeGroupInformation()
{
	dataSet = 0;
}

CDFProbeGroupInformation::~CDFProbeGroupInformation()
{
	if (dataSet)
	{
		dataSet->Close();
		dataSet->Delete();
	}
}

void CDFProbeGroupInformation::SetDataSet(DataSet* ds)
{
	if (dataSet)
	{
		dataSet->Close();
		dataSet->Delete();
	}

	dataSet = ds;
	dataSet->Open();

	const DataSetHeader& dsh = ds->Header();

	type = Expression;
	direction = 0;
	listCnt = 0;
	cellCnt = 0;
	wobbleSituation = 0;
	alleleCode = 0;
	channel = 0;
	repType = 0;
	probeSetNumber = 0;
	cellsPerList = 0;

	ParameterNameValueType nvt;

	// UnitType
	if (dsh.FindNameValParam(CDF_UNIT_TYPE, nvt))
		type = (CDFDataTypeIds)nvt.GetValueUInt8();
	
	// Direction
	if (dsh.FindNameValParam(CDF_DIRECTION, nvt))
		direction = nvt.GetValueUInt8();

	// ProbeSetNumber
	if (dsh.FindNameValParam(CDF_PROBE_SET_NUMBER, nvt))
		probeSetNumber = nvt.GetValueUInt32();

	// CellsPerList (cells per atom)
	if (dsh.FindNameValParam(CDF_CELLS_PER_ATOM, nvt))
		cellsPerList = nvt.GetValueUInt8();

	// Wobble situation 
//	if (dsh.FindNameValParam(CDF_WOBBLE_SITUATION, nvt))
//		wobbleSituation = nvt.GetValueUInt16();

	// Allele code
//	if (dsh.FindNameValParam(CDF_ALLELE, nvt))
//		alleleCode = nvt.GetValueUInt16();

	// Channel
//	if (dsh.FindNameValParam(CDF_CHANNEL, nvt))
//		channel = nvt.GetValueUInt8();

	// Probe replication type
//	if (dsh.FindNameValParam(CDF_REP_TYPE, nvt))
//		repType = nvt.GetValueUInt8();

	// List (atom)
	if (dsh.FindNameValParam(CDF_ATOMS, nvt))
		listCnt = nvt.GetValueUInt32();

	// Cells
	if (dsh.FindNameValParam(CDF_CELLS, nvt))
		cellCnt = nvt.GetValueUInt32();
}

/*
 * Get information for the probe by index
 */
void CDFProbeGroupInformation::GetCell(int32_t cell_index, CDFProbeInformation& info)
{
	if (cell_index < 0 || cell_index >= dataSet->Rows())
		return;

	info.CopyFromDataSet(dataSet, cell_index);
}
