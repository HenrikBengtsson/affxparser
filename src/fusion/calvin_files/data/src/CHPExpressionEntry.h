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
#ifndef _CHPExpressionEntry_HEADER_
#define _CHPExpressionEntry_HEADER_

/*! \file CHPExpressionEntry.h Defines a class to store the expression analyis results. */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
#include <vector>
//

namespace affymetrix_calvin_io
{

/*! This class stores the expression probe set analysis results. */
class CHPExpressionEntry
{
private:

	/*! The probe set name. */
	std::string probeSetName;

	/*! The detection call. */
	u_int8_t detection;

	/*! The detection p-value. */
	float detectionPValue;

	/*! The signal. */
	float signal;

	/*! The number of probe pairs in the probe set. */
	u_int16_t numPairs;

	/*! The number of probe pairs used in the analysis. */
	u_int16_t numPairsUsed;

	/*! A flag to indicate if comparison data exists. */
	bool hasComparisonData;

	/*! The change call. */
	u_int8_t change;

	/*! The change p-value. */
	float changePValue;

	/*! The signal log ratio. */
	float sigLogRatio;

	/*! The signal log ratio low. */
	float sigLogRatioLo;

	/*! The signal log ratio high. */
	float sigLogRatioHi;

	/*! The number of probe pairs in common between control and experiment. */
	u_int16_t commonPairs;

public:

	/*! Constructor. */
	CHPExpressionEntry();

	/*! Constructor with absolute and comparison data.
	 * @param probeSetname The probe set name
	 * @param detection The detection
	 * @param detectionPValue The detection p-value
	 * @param signal The signal
	 * @param numPairs The number of probe pairs in the set.
	 * @param numPairsUsed The number of probe pairs used in the analysis
	 * @param compData Flag indicating if comp data exists.
	 * @param change The change call
	 * @param changePValue The change p-value
	 * @param sigLogRatio The signal log ratio
	 * @param sigLogRatioLo The signal log ratio low
	 * @param sigLogRatioHi The signal log ratio high
	 * @param commonPairs The number of probe pairs in common between control and experiment
	 */
	CHPExpressionEntry(const std::string& probeSetname,
										u_int8_t detection,
										float detectionPValue,
										float signal,
										u_int16_t numPairs,
										u_int16_t numPairsUsed,
										bool compData,
										u_int8_t change,
										float changePValue,
										float sigLogRatio,
										float sigLogRatioLo,
										float sigLogRatioHi,
										u_int16_t commonPairs);

	/*! Constructor with absolute data only.
	 * @param probeSetname The probe set name
	 * @param detection The detection
	 * @param detectionPValue The detection p-value
	 * @param signal The signal
	 * @param numPairs The number of probe pairs in the set.
	 * @param numPairsUsed The number of probe pairs used in the analysis
	 */
	CHPExpressionEntry(const std::string& probeSetname,
										u_int8_t detection,
										float detectionPValue,
										float signal,
										u_int16_t numPairs,
										u_int16_t numPairsUsed);

	/*! Destructor */
	~CHPExpressionEntry();

	/*! Clears the members. */
	void Clear();

	/*! Assignment operator
	 * @param e The entry to copy
	 * @return The new object
	 */
	CHPExpressionEntry operator=(CHPExpressionEntry e);

	/*! Gets the probe set name. */
	std::string GetProbeSetName() const { return probeSetName; }

	/*! Get the detection call. */
	u_int8_t GetDetection() const { return detection; }

	/*! Gets the detection p-value. */
	float GetDetectionPValue() const { return detectionPValue; }

	/*! Gets the signal value. */
	float GetSignal() const { return signal; }

	/*! Gets the number of probe pairs in the set. */
	u_int16_t GetNumPairs() const { return numPairs; }

	/*! Gets the number of probe pairs used in the analysis. */
	u_int16_t GetNumPairsUsed() const { return numPairsUsed; }

	/*! A flag to indicate if comparison data exists. */
	bool GetHasComparisonData() const { return hasComparisonData; }

	/*! Gets the change call. */
	u_int8_t GetChange() const { return change; }

	/*! Gets the change p-value. */
	float GetChangePValue() const { return changePValue; }

	/*! Gets the signal log ratio. */
	float GetSigLogRatio() const { return sigLogRatio; }

	/*! Gets the signal log ratio low. */
	float GetSigLogRatioLo() const { return sigLogRatioLo; }

	/*! Gets the signal log ratio high. */
	float GetSigLogRatioHi() const { return sigLogRatioHi; }

	/*! Gets the number of probe pairs in common between control and experiment. */
	u_int16_t GetCommonPairs() const { return commonPairs; }

	/*! Sets the probe set name. */
	void SetProbeSetName(const std::string& p) { probeSetName = p; }

	/*! Sets the detection value. */
	void SetDetection(u_int8_t p) { detection = p; }

	/*! Sets the detection p-value. */
	void SetDetectionPValue(float p) { detectionPValue = p; }

	/*! Sets the signal value. */
	void SetSignal(float p) { signal = p; }

	/*! Sets the number of pairs in the probe sets. */
	void SetNumPairs(u_int16_t p) { numPairs = p; }

	/*! Sets the number of pairs used in the analysis. */
	void SetNumPairsUsed(u_int16_t p) { numPairsUsed = p; }

	/*! Sets the flag to indicate if comparison data exists. */
	void SetHasComparisonData(bool b) { hasComparisonData = b; }

	/*! Sets the change call. */
	void SetChange(u_int8_t p) { change = p; }

	/*! Sets the change p-value. */
	void SetChangePValue(float p) { changePValue = p; }

	/*! Sets the signal log ratio. */
	void SetSigLogRatio(float p) { sigLogRatio = p; }

	/*! Sets the signal log ratio low. */
	void SetSigLogRatioLo(float p) { sigLogRatioLo = p; }

	/*! Sets the signal log ratio high. */
	void SetSigLogRatioHi(float p) { sigLogRatioHi = p; }

	/*! Sets the number of probe pairs in common between control and experiment. */
	void SetCommonPairs(u_int16_t p) { commonPairs = p; }
};

/*! An STL list of zones */
typedef std::vector<CHPExpressionEntry> CHPExpressionEntryVector;

/*! iterator of CHPExpressionEntrys */
typedef std::vector<CHPExpressionEntry>::iterator CHPExpressionEntryVectorIt;

}

#endif // _CHPExpressionEntry_HEADER_
