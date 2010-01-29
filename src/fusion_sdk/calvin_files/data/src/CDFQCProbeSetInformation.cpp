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


#include "calvin_files/data/src/CDFQCProbeSetInformation.h"
//
#include "calvin_files/data/src/CDFData.h"
#include "calvin_files/data/src/CDFQCProbeInformation.h"
#include "calvin_files/data/src/DataSet.h"
//

using namespace affymetrix_calvin_io;

CDFQCProbeSetInformation::CDFQCProbeSetInformation()
{
	dataSet = 0;
}

CDFQCProbeSetInformation::~CDFQCProbeSetInformation()
{
	if (dataSet)
	{
		dataSet->Close();
		dataSet->Delete();
	}
}

/*
 * Get the QC type - DataSet name
 */
std::wstring CDFQCProbeSetInformation::GetQCProbeSetType() const
{
	return dataSet->Header().GetName();
}

/*
 * Get the number of cells in the probe set - number of rows in the DataSet
 */
int32_t CDFQCProbeSetInformation::GetNumCells() const
{
	if (dataSet == 0)
		return 0;
	return dataSet->Rows();
}

void CDFQCProbeSetInformation::SetDataSet(DataSet* ds)
{
	if (dataSet)
	{
		dataSet->Close();
		dataSet->Delete();
	}

	dataSet = ds;
	dataSet->Open();
}

/*
 * Get information for the probe by index
 */
void CDFQCProbeSetInformation::GetProbeInformation(int32_t cell_index, CDFQCProbeInformation& info)
{
	if (cell_index < 0 || cell_index >= dataSet->Rows())
		return;

	info.CopyFromDataSet(dataSet, cell_index);
}
