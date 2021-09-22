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


#ifndef _CDFProbeInformation_HEADER_
#define _CDFProbeInformation_HEADER_

/*! \file CDFProbeInformation.h This file defines the CDFProbeInformation class.
 */
#ifdef _MSC_VER
#include <windows.h>
#endif

#include "calvin_files/data/src/DataSet.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//


namespace affymetrix_calvin_io
{
/*! Class that exposes the CDF probe information */
class CDFProbeInformation
{
public:
	/*! Constructor */
	CDFProbeInformation();

	/*! Get the list index (atom number) of the probe */
	u_int32_t GetListIndex() const { return listIndex; }
	/*! Get the exon position of the probe (for reseq)*/
	int GetExpos() const { return indexPos; }
	/*! Get the x-coordinate of the probe */
	u_int16_t GetX() const { return xCoord; }
	/*! Get the y-coordinate of the probe */
	u_int16_t GetY() const { return yCoord; }
	/*! Get the base of the probe at the substitution position */
	int8_t GetPBase() const { return baseProbe; }
	/*! Get the base of the target at the interrogation position */
	int8_t GetTBase() const { return baseTarget; }
	/*! Get the probe length */
	u_int16_t GetProbeLength() const { return probeLength; }
	/*! Get the probe grouping */
	u_int16_t GetProbeGrouping() const { return probeGrouping; }

protected:
	enum DataSetColumns { XCoordCol, YCoordCol, ListIndexCol, IndexPosCol, BaseProbeCol, BaseTargetCol, ProbeLenCol, ProbeGroupCol };

	/*!  */
	void CopyFromDataSet(DataSet* ds, int32_t row);

protected:
	/*! x-coordinate of the probe, m_X in C++ */
	u_int16_t xCoord;
	/*! y-coordinate of the probe, m_Y in C++ */
	u_int16_t yCoord;
	/*! list index (atom number), m_ListIndex in C++ */
	u_int32_t listIndex;
	/*! exon position for reseq, m_Expos in C++ */
	u_int32_t indexPos;
	/*! Base of the probe at the substitution position, m_PBase in C++ */
	int8_t baseProbe;
	/*! Base of the probe at the interrogation position, m_TBase in C++  */
	int8_t baseTarget;
	/*! Probe length, m_ProbeLength in C++  */
	u_int16_t probeLength;
	/*! Probe grouping, m_ProbeGrouping in C++  */
	u_int16_t probeGrouping;

	friend class CDFProbeGroupInformation;
};

}
#endif // _CDFProbeInformation_HEADER_
