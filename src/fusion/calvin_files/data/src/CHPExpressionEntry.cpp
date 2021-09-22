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


#include "calvin_files/data/src/CHPExpressionEntry.h"
//

using namespace affymetrix_calvin_io;

CHPExpressionEntry::CHPExpressionEntry() 
{
	Clear();
}

CHPExpressionEntry::CHPExpressionEntry(const std::string& psName,
									u_int8_t detect,
									float detectPValue,
									float sig,
									u_int16_t nPairs,
									u_int16_t nPairsUsed,
									bool compData,
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
	hasComparisonData = compData;
	change = chg;
	changePValue = chgPValue;
	sigLogRatio = sLogRatio;
	sigLogRatioLo = sLogRatioLo;
	sigLogRatioHi = sLogRatioHi;
	commonPairs = commonPrs;
}

CHPExpressionEntry::CHPExpressionEntry(const std::string& psName,
									u_int8_t detect,
									float detectPValue,
									float sig,
									u_int16_t nPairs,
									u_int16_t nPairsUsed)
{
	probeSetName = psName;
	detection = detect;
	detectionPValue = detectPValue;
	signal = sig;
	numPairs = nPairs;
	numPairsUsed = nPairsUsed;
	hasComparisonData = false;
	change = 0;
	changePValue = 0;
	sigLogRatio = 0;
	sigLogRatioLo = 0;
	sigLogRatioHi = 0;
	commonPairs = 0;
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
	hasComparisonData = false;
	change = 0;
	changePValue = 0;
	sigLogRatio = 0;
	sigLogRatioLo = 0;
	sigLogRatioHi = 0;
	commonPairs = 0;
}

CHPExpressionEntry CHPExpressionEntry::operator=(CHPExpressionEntry e)
{
	probeSetName = e.GetProbeSetName();
	detection = e.GetDetection();
	detectionPValue = e.GetDetectionPValue();
	signal = e.GetSignal();
	numPairs = e.GetNumPairs();
	numPairsUsed = e.GetNumPairsUsed();
	hasComparisonData = e.GetHasComparisonData();
	change = e.GetChange();
	changePValue = e.GetChangePValue();
	sigLogRatio = e.GetSigLogRatio();
	sigLogRatioLo = e.GetSigLogRatioLo();
	sigLogRatioHi = e.GetSigLogRatioHi();
	commonPairs = e.GetCommonPairs();
	return *this; 
}
