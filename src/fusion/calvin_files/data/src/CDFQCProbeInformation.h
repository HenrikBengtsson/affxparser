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


#ifndef _CDFQCProbeInformation_HEADER_
#define _CDFQCProbeInformation_HEADER_

/*! \file CDFQCProbeInformation.h This file defines the CDFQCProbeInformation class.
 */

#include "calvin_files/data/src/DataSet.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//

namespace affymetrix_calvin_io
{
/*! Class that exposes the CDF QC probe information */
class CDFQCProbeInformation
{
public:
	/*! Constructor */
	CDFQCProbeInformation();

	/*! Get the x-coordinate of the probe */
	u_int16_t GetX() const { return xCoord; }
	/*! Get the y-coordinate of the probe */
	u_int16_t GetY() const { return yCoord; }
	/*! Get the probe length */
	u_int8_t GetPLen() const { return probeLength; }
	/*! Is this a perfect match probe */
	bool IsPerfectMatchProbe() const { return (perfectMatchFlag == 1 ? true : false); }
	/*! Is this a background probe */
	bool IsBackgroundProbe() const { return (backgroundProbeFlag == 1 ? true : false); }

protected:
	enum DataSetColumns { XCoordCol, YCoordCol, ProbeLengthCol, PerfectMatchFlagCol, BackgroundProbeFlagCol };

	/*!  */
	void CopyFromDataSet(DataSet* ds, int32_t row);

protected:
	/*! x-coordinate of the probe, m_X in C++ */
	u_int16_t xCoord;
	/*! y-coordinate of the probe, m_Y in C++ */
	u_int16_t yCoord;
	/*! Probe length */
	u_int8_t probeLength;
	/*! Perfect match flag, 1 = match */
	u_int8_t perfectMatchFlag;
	/*! Background probe flag, 1 = background */
	u_int8_t backgroundProbeFlag;

	friend class CDFQCProbeSetInformation;
};

}
#endif	// _CDFQCProbeInformation_HEADER_
