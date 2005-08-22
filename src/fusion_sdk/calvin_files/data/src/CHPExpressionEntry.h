/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////
#ifndef _CHPExpressionEntry_HEADER_
#define _CHPExpressionEntry_HEADER_

#include <string>
#include <vector>
#include "AffymetrixBaseTypes.h"

#ifdef WIN32
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class stores a zone's background value */
class CHPExpressionEntry
{

private:

	std::wstring probeSetName;
	u_int8_t detection;
	float detectionPValue;
	float signal;
	u_int16_t numPairs;
	u_int16_t numPairsUsed;
	u_int8_t change;
	float changePValue;
	float sigLogRatio;
	float sigLogRatioLo;
	float sigLogRatioHi;
	u_int16_t commonPairs;

public:

	CHPExpressionEntry();
	CHPExpressionEntry(const std::wstring& probeSetname,
										u_int8_t detection,
										float detectionPValue,
										float signal,
										u_int16_t numPairs,
										u_int16_t numPairsUsed,
										u_int8_t change,
										float changePValue,
										float sigLogRatio,
										float sigLogRatioLo,
										float sigLogRatioHi,
										u_int16_t commonPairs);
	~CHPExpressionEntry();

	void Clear();

	/*! Assignment operator
	 * @param zn The zone to copy
	 * @return The new zone object
	 */
	CHPExpressionEntry operator=(CHPExpressionEntry zn);

	std::wstring GetProbeSetName() const;

	u_int8_t GetDetection() const;

	float GetDetectionPValue() const;

	float GetSignal() const;

	u_int16_t GetNumPairs() const;

	u_int16_t GetNumPairsUsed() const;

	u_int8_t GetChange() const;

	float GetChangePValue() const;

	float GetSigLogRatio() const;

	float GetSigLogRatioLo() const;

	float GetSigLogRatioHi() const;

	u_int16_t GetCommonPairs() const;

	void SetProbeSetName(const std::wstring& p);

	void SetDetection(u_int8_t p);

	void SetDetectionPValue(float p);

	void SetSignal(float p);

	void SetNumPairs(u_int16_t p);

	void SetNumPairsUsed(u_int16_t p);

	void SetChange(u_int8_t p);

	void SetChangePValue(float p);

	void SetSigLogRatio(float p);

	void SetSigLogRatioLo(float p);

	void SetSigLogRatioHi(float p);

	void SetCommonPairs(u_int16_t p);
};

/*! An STL list of zones */
typedef std::vector<CHPExpressionEntry> CHPExpressionEntryVector;

/*! iterator of CHPExpressionEntrys */
typedef std::vector<CHPExpressionEntry>::iterator CHPExpressionEntryVectorIt;

}

#endif // _CHPExpressionEntry_HEADER_