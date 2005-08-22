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

#include "CHPExpressionEntry.h"

using namespace affymetrix_calvin_io;

CHPExpressionEntry::CHPExpressionEntry() 
{
	Clear();
}

CHPExpressionEntry::CHPExpressionEntry(const std::wstring& psName,
									u_int8_t detect,
									float detectPValue,
									float sig,
									u_int16_t nPairs,
									u_int16_t nPairsUsed,
									u_int8_t chg,
									float chgPValue,
									float sLogRatio,
									float sLogRatioLo,
									float sLogRatioHi,
									u_int16_t commonPrs)
{
	probeSetName = psName;
	detection = detect;
	detectionPValue = detectPValue;
	signal = sig;
	numPairs = nPairs;
	numPairsUsed = nPairsUsed;
	change = chg;
	changePValue = chgPValue;
	sigLogRatio = sLogRatio;
	sigLogRatioLo = sLogRatioLo;
	sigLogRatioHi = sLogRatioHi;
	commonPairs = commonPrs;
}

CHPExpressionEntry::~CHPExpressionEntry() {}

void CHPExpressionEntry::Clear() 
{
	probeSetName.clear();
	detection = 0;
	detectionPValue = 0;
	signal = 0;
	numPairs = 0;
	numPairsUsed = 0;
	change = 0;
	changePValue = 0;
	sigLogRatio = 0;
	sigLogRatioLo = 0;
	sigLogRatioHi = 0;
	commonPairs = 0;
}

CHPExpressionEntry CHPExpressionEntry::operator=(CHPExpressionEntry zn)
	{
		probeSetName = zn.GetProbeSetName();
		detection = zn.GetDetection();
		detectionPValue = zn.GetDetectionPValue();
		signal = zn.GetSignal();
		numPairs = zn.GetNumPairs();
		numPairsUsed = zn.GetNumPairsUsed();
		change = zn.GetChange();
		changePValue = zn.GetChangePValue();
		sigLogRatio = zn.GetSigLogRatio();
		sigLogRatioLo = zn.GetSigLogRatioLo();
		sigLogRatioHi = zn.GetSigLogRatioHi();
		commonPairs = zn.GetCommonPairs();
		return *this; 
	}

	std::wstring CHPExpressionEntry::GetProbeSetName() const
	{
		return probeSetName;
	}

	u_int8_t CHPExpressionEntry::GetDetection() const
	{
		return detection;
	}

	float CHPExpressionEntry::GetDetectionPValue() const
	{
		return detectionPValue;
	}

	float CHPExpressionEntry::GetSignal() const
	{
		return signal;
	}

	u_int16_t CHPExpressionEntry::GetNumPairs() const
	{
		return numPairs;
	}

	u_int16_t CHPExpressionEntry::GetNumPairsUsed() const
	{
		return numPairsUsed;
	}

	u_int8_t CHPExpressionEntry::GetChange() const
	{
		return change;
	}

	float CHPExpressionEntry::GetChangePValue() const
	{
		return changePValue;
	}

	float CHPExpressionEntry::GetSigLogRatio() const
	{
		return sigLogRatio;
	}

	float CHPExpressionEntry::GetSigLogRatioLo() const
	{
		return sigLogRatioLo;
	}

	float CHPExpressionEntry::GetSigLogRatioHi() const
	{
		return sigLogRatioHi;
	}

	u_int16_t CHPExpressionEntry::GetCommonPairs() const
	{
		return commonPairs;
	}

	void CHPExpressionEntry::SetProbeSetName(const std::wstring& p)
	{
		probeSetName = p;
	}

	void CHPExpressionEntry::SetDetection(u_int8_t p)
	{
		detection = p;
	}

	void CHPExpressionEntry::SetDetectionPValue(float p)
	{
		detectionPValue = p;
	}

	void CHPExpressionEntry::SetSignal(float p)
	{
		signal = p;
	}

	void CHPExpressionEntry::SetNumPairs(u_int16_t p)
	{
		numPairs = p;
	}

	void CHPExpressionEntry::SetNumPairsUsed(u_int16_t p)
	{
		numPairsUsed = p;
	}

	void CHPExpressionEntry::SetChange(u_int8_t p)
	{
		change = p;
	}

	void CHPExpressionEntry::SetChangePValue(float p)
	{
		changePValue = p;
	}

	void CHPExpressionEntry::SetSigLogRatio(float p)
	{
		sigLogRatio = p;
	}

	void CHPExpressionEntry::SetSigLogRatioLo(float p)
	{
		sigLogRatioLo = p;
	}

	void CHPExpressionEntry::SetSigLogRatioHi(float p)
	{
		sigLogRatioHi = p;
	}

	void CHPExpressionEntry::SetCommonPairs(u_int16_t p)
	{
		commonPairs = p;
	}
