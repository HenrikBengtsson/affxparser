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


#ifndef _CDFProbeSetInformation_HEADER_
#define _CDFProbeSetInformation_HEADER_

/*! \file CDFProbeSetInformation.h This file defines the CDFProbeSetInformation class.
 */

#ifdef _MSC_VER
#include <windows.h>
#endif

#include "calvin_files/data/src/CDFDataTypes.h"
#include "calvin_files/data/src/DataGroup.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//


namespace affymetrix_calvin_io
{

class CDFProbeGroupInformation;

/*! Class that exposes the CDF probe set information */
class CDFProbeSetInformation
{
public:
	/*! Constructor */
	CDFProbeSetInformation();
	/*! Destructor */
	~CDFProbeSetInformation();

	/*! Get the probe set name */
	std::wstring GetName() const { if (dataGroup) return dataGroup->Header().GetName(); else return L""; }
	/*! Get the probe set direction */
	affymetrix_calvin_io::DirectionType GetDirection() const { return (affymetrix_calvin_io::DirectionType)direction; }
	/*! Get the total number of lists (atoms) in the probe set */
	u_int32_t GetNumLists() const { return listCnt; }
	/*! Get the number of groups (block) in the probe set */
	u_int32_t GetNumGroups() const { return groupCnt; }
	/*! Get the total number of cells in the probe set */
	u_int32_t GetNumCells() const { return cellCnt; }
	/*! Get the number of cells per list (atom) */
	u_int8_t GetNumCellsPerList() const { return (u_int8_t)cellsPerList; }
	/*! Get the probe set number */
	u_int32_t GetProbeSetNumber() const { return probeSetNumber; }
	/*! Get information for a probe set group by index */
	void GetGroupInformation(u_int32_t groupIdx, CDFProbeGroupInformation & info);
	/*! Get the probe set type. */
	CDFDataTypeIds GetProbeSetType() const { return (CDFDataTypeIds)type; }

protected:
	/*!  */
	void SetDataGroup(DataGroup& dg);

protected:
	/*! DataGroup */
	DataGroup* dataGroup;

	/*! Probe set type - based on unitType of first DataSet u_int8_t*/
	CDFDataTypeIds type;
	/*! Direction - based on first DataSet */
	u_int8_t direction;
	/*! Total number of groups (blocks), = number of DataSets in the DataGroup */
	u_int32_t groupCnt;
	/*! Total number of lists (atoms) in the probe set - sum of all DataSets */
	u_int32_t listCnt;
	/*! Total number of cells in the probe set - sum of all DataSets */
	u_int32_t cellCnt;
	/*! Probe set number - based on value in first DataSet, same as ProbeSetNumber in C++ */
	u_int32_t probeSetNumber;
	/*! Cells per list (Cell per atom) - based on value in the first DataSet, same as NumCellsPerList in C++ */
	u_int8_t cellsPerList;

	friend class CDFData;
};

}
#endif	// _CDFProbeSetInformation_HEADER_
