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


#ifndef _CDFProbeGroupInformation_HEADER_
#define _CDFProbeGroupInformation_HEADER_

/*! \file CDFProbeGroupInformation.h This file defines the CDFProbeGroupInformation class.
 */

#ifdef _MSC_VER
#include <windows.h>
#endif

namespace affymetrix_calvin_io
{

class CDFProbeInformation;

/*! Class that exposes the CDF probe group information */
class CDFProbeGroupInformation
{
public:
	/*! Constructor */
	CDFProbeGroupInformation();
	/*! Desctructor */
	~CDFProbeGroupInformation();

	/*! Get the probe set direction */
	DirectionType GetDirection() const { return (DirectionType)direction; }
	/*! Get the number of lists (atoms) in the probe group */
	u_int32_t GetNumLists() const { return listCnt; }
	/*! Get the number of cells in the probe group */
	u_int32_t GetNumCells() const { return cellCnt; }
	/*! Get the number of cells per list (atom) */
	u_int8_t GetNumCellsPerList() const { return (u_int8_t)cellsPerList; }
	/*! Get the probe group name = DataPlane name */
	std::wstring GetName() const { if (dataSet) return dataSet->Header().GetName(); else return L""; }
	/*! Get information for the probe by index */
	void GetCell(int32_t cell_index, CDFProbeInformation& info);

protected:
	void SetDataSet(DataSet* ds);

protected:
	/*! DataSet pointer */
	DataSet* dataSet;

	/*! Probe set type - u_int8_t*/
	CDFDataTypeIds type;
	/*! Direction */
	u_int8_t direction;
	/*! Number of lists (atoms) in the probe group */
	u_int32_t listCnt;
	/*! Number of cells in the probe group */
	u_int32_t cellCnt;
	/*! Probe set number - same as ProbeSetNumber in C++ */
	u_int32_t probeSetNumber;
	/*! Cells per list (atom) - same as NumCellsPerList in C++ */
	u_int8_t cellsPerList;

	friend class CDFProbeSetInformation;
};

}
#endif // _CDFProbeGroupInformation_HEADER_
