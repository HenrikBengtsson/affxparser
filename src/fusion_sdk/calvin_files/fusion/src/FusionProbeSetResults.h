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


#ifndef _FusionProbeSetResults_HEADER_
#define _FusionProbeSetResults_HEADER_


//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

//////////////////////////////////////////////////////////////////////

#include "calvin_files/data/src/CHPExpressionEntry.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffyStlCollectionTypes.h"
//
#include <cstring>
#include <list>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affymetrix_fusion_io
{

////////////////////////////////////////////////////////////////////

/*! Provides a base class for probe set results */
class FusionProbeSetResults
{
public:
	/*! Constructor */
	FusionProbeSetResults() {};

	/*! Destructor */
	virtual ~FusionProbeSetResults() {};
};

////////////////////////////////////////////////////////////////////

/*! Expression analysis probe set results for the MAS5 algorithm */
class FusionExpressionProbeSetResults : public FusionProbeSetResults
{
private:
	/*! The detection p-value */
	float detectionPValue;

	/*! The signal value */
	float signal; 

	/*! The number of probe pairs in the set */
	u_int16_t numPairs;

	/*! The number of probe pairs used to calculate the signal value */
	u_int16_t numPairsUsed;

	/*! The detection call */
	u_int8_t detection;

	/*! Flag indicating that comparison results exist */
	bool hasCompResults;

	/*! The change p-value */
	float changePValue; 

	/*! The signal log ratio */
	float sigLogRatio; 

	/*! The signal log ratio low value */
	float sigLogRatioLo; 

	/*! The signal log ratio high value */
	float sigLogRatioHi; 

	/*! The number of probe pairs in common between the experiment and baseline data */
	u_int16_t numCommonPairs;

	/*! The change call */
	u_int8_t change;

public:

	/*! \brief Clears the members. */
	void Clear();

	/*! \brief Gets detection pvalue
	 *	\return Detection pvalue.
	 */
	float GetDetectionPValue() const;

	/*! \brief Gets the signal.
	 *	\return Signal.
	 */
	float GetSignal() const; 

	/*! \brief gets the number of pairs.
	 *	\return Number of pairs.
	 */
	u_int16_t GetNumPairs() const;

	/*! \brief Gets the number used pairs.
	 *	\return Number of used pairs.
	 */
	u_int16_t GetNumUsedPairs() const;

	/*! \brief Gets the detection.
	 *	\return Detection.
	 */
	u_int8_t GetDetection() const;

	/*! \brief Determines if comp results exsist.
	 *	\return True if comp results exsist.
	 */
	bool HasCompResults() const;

	/*! \brief Gets the change pvalue.
	 *	\return Cahnge pvalue.
	 */
	float GetChangePValue() const; 

	/*! \brief Gets the signal log ratio.
	 *	\return Signal log ratio.
	 */
	float GetSignalLogRatio() const; 

	/*! \brief Gets the signal log ratio low
	 *	\return Signal log ratio low.
	 */
	float GetSignalLogRatioLow() const; 

	/*! \brief Gets the signal log ratio high
	 *	\return Signal log ratio high.
	 */
	float GetSignalLogRatioHigh() const; 

	/*! \brief Gets the number of common pairs
	 *	\return Number of common pairs.
	 */
	u_int16_t GetNumCommonPairs() const;

	/*! \brief Gets the change.
	 *	\return Change.
	 */
	u_int8_t GetChange() const;

	/*! \brief Sets the detection pvalue.
	 *	\param p pvalue to set.
	 */
	void SetDetectionPValue(float p);

	/*! \brief Sets the signal.
	 *	\param p Signal to set.
	 */
	void SetSignal(float p); 

	/*! \brief Sets the number of pairs.
	 *	\param p Number of pairs.
	 */
	void SetNumPairs(u_int16_t p);

	/*! \brief Sets the number of used pairs.
	 *	\param p Number of used pairs to set.
	 */
	void SetNumUsedPairs(u_int16_t p);

	/*! \brief Sets detection.
	 *	\param p Detection to set.
	 */
	void SetDetection(u_int8_t p);

	/*! \brief Sets whether has comp results.
	 *	\param p Flag whether comp results exsists.
	 */
	void SetHasCompResults(bool p);

	/*! \brief Sets the change pvalue.
	 *	\param p Change pvalue to set.
	 */
	void SetChangePValue(float p); 

	/*! \brief Sets the signal log ratio.
	 *	\param p Signal log ratio.
	 */
	void SetSignalLogRatio(float p); 

	/*! \brief Sets the signal log ratio low.
	 *	\param p Signal ratio low.
	 */
	void SetSignalLogRatioLow(float p); 

	/*! \brief Sets the signal log ratio high
	 *	\param p Signal log ratio high to set.
	 */
	void SetSignalLogRatioHigh(float p); 

	/*! \brief Sets the number of common pairs.
	 *	\param p Number of common pairs.
	 */
	void SetNumCommonPairs(u_int16_t p);

	/*! \brief Sets the change.
	 *	\param p Change value to set.
	 */
	void SetChange(u_int8_t p);

	/*! \brief Gets a string representation of the detection call.
	 *	\return The detection call
	 */
	std::string GetDetectionString();

	/*! \brief Gets a string representation of the change call.
	 *	\return The change call
	 */
	std::string GetChangeString();

	/*! Assignment operator
	 * \param src The object to copy
	 * \return The copied object
	 */
	FusionExpressionProbeSetResults operator=(FusionExpressionProbeSetResults &src);

	/*! \brief Constructor */
	FusionExpressionProbeSetResults();

	/*! \brief Destructor */
	~FusionExpressionProbeSetResults();
};

////////////////////////////////////////////////////////////////////

/*! Genotyping analysis probe set results */
class FusionGenotypeProbeSetResults : public FusionProbeSetResults
{
private:
	/*! The allele call */
	u_int8_t alleleCall;

	/*! The confidence associated with the allele call */
	float confidence;

	/*! The relative allele strength for the first block */
	float RAS1;

	/*! The relative allele strength for the second block */
	float RAS2;

	/*! The p-value associated with an AA call */
	float pValueAA;

	/*! The p-value associated with an AB call */
	float pValueAB;

	/*! The p-value associated with an BB call */
	float pValueBB;

	/*! The p-value associated with an no call call */
	float pValueNoCall;

public:

	void Clear();

	/*! \brief The allele call 
	 *	\return Allele call.
	 */
	u_int8_t GetAlleleCall();

	/*! \brief The confidence associated with the allele call.
	 *	\return Confidence.
	 */
	float GetConfidence();

	/*! \brief The relative allele strength for the first block.
	 *	\return RAS1.
	 */
	float GetRAS1();

	/*! \brief The relative allele strength for the second block.
	 *	\return RAS2.
	 */
	float GetRAS2();

	/*! \brief The p-value associated with an AA call.
	 *	\return pvalue for AA.
	 */
	float GetPValueAA();

	/*! \brief The p-value associated with an AB call.
	 *	\return pvalue for AB.
	 */
	float GetPValueAB();

	/*! \brief The p-value associated with an BB call.
	 *	\return pvalue for BB.
	 */
	float GetPValueBB();

	/*! \brief The p-value associated with an no call call.
	 *	\return pvalue no call.
	 */
	float GetPValueNoCall();

	/*! \brief Sets the allele call.
	 *	\param p Allele call to set.
	 */
	void SetAlleleCall(u_int8_t p);

	/*! \brief The confidence associated with the allele call.
	 *	\param p Confidence to value to set.
	 */
	void SetConfidence(float p);

	/*! \brief Sets the relative allele strength for the first block.
	 *	\param p RAS1 value to set.
	 */
	void SetRAS1(float p);

	/*! \brief Sets the relative allele strength for the second block.
	 *	\param p RAS2 value to set.
	 */
	void SetRAS2(float p);

	/*! \brief Sets the p-value associated with an AA call.
	 *	\param p pvalue aa to set.
	 */
	void SetPValueAA(float p);

	/*! \brief Sets the p-value associated with an AB call.
	 *	\param p pvalue AB to set.
	 */
	void SetPValueAB(float p);

	/*! \brief Sets the p-value associated with an BB call.
	 *	\param p pvalue BB to set.
	 */
	void SetPValueBB(float p);

	/*! \brief Sets the p-value associated with an no call call.
	 *	\param p pvalue no call to set.
	 */
	void SetPValueNoCall(float p);

	/*! \brief Gets a string representation of the allele call.
	 *	\return Allele call.
	 */
	std::string GetAlleleCallString();

	/*! \brief Assignment operator
	 *	\param src Object to copy
	 *	\return Copied object
	 */
	FusionGenotypeProbeSetResults operator=(FusionGenotypeProbeSetResults &src);

	/*! \brief Constructor */
	FusionGenotypeProbeSetResults();

	/*! \brief Destructor */
	~FusionGenotypeProbeSetResults();
};

////////////////////////////////////////////////////////////////////

/*! Universal (tag array) analysis probe set results. */
class FusionUniversalProbeSetResults : public FusionProbeSetResults
{
private:

	/*! The background value.*/
	float background;

public:
	/*! Gets the background value.
	 * @return The background value.
	 */
	float GetBackground() const;

	/*! Sets the background value.
	 * @param bg The background value.
	 */
	void SetBackground(float bg);

	void Clear();

	/*! Assignment operator
	 * @param src The object to copy
	 * @return The copied object
	 */
	FusionUniversalProbeSetResults operator=(FusionUniversalProbeSetResults &src);

	/*! Constructor */
	FusionUniversalProbeSetResults();

	/*! Destructor */
	~FusionUniversalProbeSetResults();
};

////////////////////////////////////////////////////////////////////

/*! A structure to hold a base call at a given position (index). */
class FusionBaseCallType
{
protected:
	/*! The position (index) of the call. */
	int32_t position;

	/*! The call. */
	int8_t call;

public:

	/*! Constructor. */
	FusionBaseCallType();

	/*! Constructor with values.
	 * @param p The position.
	 * @param c The call.
	 */
	FusionBaseCallType(int32_t p, int8_t c);

	/*! Destructor */
	~FusionBaseCallType();

	/*! Clears the values. */
	void Clear();

	/*! Gets the position.
	 * @return The position.
	 */
	int32_t GetPosition() const { return position; }

	/*! Gets the call.
	 * @return The call
	 */
	int8_t GetCall() const { return call; }

	/*! Sets the position
	 * @param p The position.
	 */
	void SetPosition(int32_t p) { position = p; }

	/*! Sets the call.
	 * @param c The call.
	 */
	void SetCall(int8_t c) { call = c; }
};

/*! A vector of calls. */
typedef std::vector<FusionBaseCallType> FusionBaseCallVector;

////////////////////////////////////////////////////////////////////

/*! A class to hold a force call, its position and reason.
 *
 * A force call is the call the algorithm would have made if the thresholds
 * were not applied.
 */
class FusionForceCallType : public FusionBaseCallType
{
protected:
	/*! The reason for the call. */
	int8_t reason;

public:

	/*! \brief Constructor */
	FusionForceCallType();

	/*! \brief Constructor
	 *	\param p Position
	 *	\param c Call
	 *	\param r Reason
	 */
	FusionForceCallType(int32_t p, int8_t c, u_int8_t r);

	/*! \brief Destructor */
	~FusionForceCallType();

	/*! \brief Clears members. */
	void Clear();

	/*! \brief Gets the reason.
	 *	\return Reason.
	 */
	u_int8_t GetReason() const { return reason; }

	/*! \brief Sets the call.
	 *	\param r Reason value to set.
	 */
	void SetReason(u_int8_t r) { reason = r; }
};

/*! A vector of force calls. */
typedef std::vector<FusionForceCallType> FusionForceCallVector;

////////////////////////////////////////////////////////////////////

/*! \brief Resequencing results. */
class FusionResequencingResults
{
private:
	/*! \brief The called bases. */
	Int8Vector calledBases;

	/*! Base call scores. */
	FloatVector scores;

	/*! An array of force calls - base calls the algorithm would have made if the thresholds were removed. */
	FusionForceCallVector forceCalls;

	/*! An array of original calls. The calledBases contained the results of the algorithm and user edits.
	 * If a user edits a base the original algorithm called base is stored in this vector.
	 */
	FusionBaseCallVector origCalls;

public:
	/*! Constructor */
	FusionResequencingResults();

	/*! Destructor */
	~FusionResequencingResults();

	/*! Clears the members. */
	void Clear();

	/*! \brief Gets the called bases.
	 *	\return Array of called bases.
	 */
	const Int8Vector &GetCalledBases();

	/*! \brief Gets the called base at the given index.
	 *	\param index Index to the called bases array.
	 *	\return Called base.
	 */
	int8_t GetCalledBase(int index);

	/*! \brief Gets the size of the called bases array.
	 *	\return Size of the called bases array.
	 */
	int32_t GetCalledBasesSize() const;

	/*! \brief Resizes the called bases array.
	 *	\param size Size of the array.
	 */
	void ResizeCalledBases(int32_t size);

	/*! \brief Sets the called base.
	 *	\param index Index to the array.
	 *	\param call Call.
	 */
	void SetCalledBase(int32_t index, int8_t call);

	/*! \brief Adds the called base to the end of the list.
	 *	\param call Call.
	 */
	void AddCalledBase(int8_t call);

	/*! \brief Gets the scores.
	 *	\return Array of scores.
	 */
	const FloatVector& GetScores();

	/*! \brief Gets the score at the given index.
	 *	\param index Index to the scores array.
	 *	\return Score.
	 */
	float GetScore(int32_t index);

	/*! \brief Gets the size of the scores array.
	 *	\return Size of the scores array.
	 */
	int32_t GetScoresSize() const;

	/*! \brief Resizes the scores array.
	 *	\param size Size of the array.
	 */
	void ResizeScores(int32_t size);

	/*! \brief Sets the score.
	 *	\param index Index to the array.
	 *	\param score Score.
	 */
	void SetScore(int index, float score);

	/*! \brief Adds the score to the end of the list.
	 *	\param score Score.
	 */
	void AddScore(float score);

	/*! \brief Gets the force calls.
	 *	\return Array of force calls.
	 */
	const FusionForceCallVector& GetForceCalls();

	/*! \brief Gets the force call at the given index.
	 *	\param index Index to the force calls array.
	 *	\return Force call.
	 */
	FusionForceCallType GetForceCall(int32_t index);

	/*! \brief Gets the size of the force calls array.
	 *	\return Size of the force calls array.
	 */
	int32_t GetForceCallsSize() const;

	/*! \brief Resizes the force calls array.
	 *	\param size Size of the array.
	 */
	void ResizeForceCalls(int32_t size);

	/*! \brief Sets the force call.
	 *	\param index Index to the array.
	 *	\param call Force call.
	 */
	void SetForceCall(int32_t index, FusionForceCallType call);

	/*! \brief Adds the force call to the end of the list.
	 *	\param call Force call.
	 */
	void AddForceCall(FusionForceCallType call);

	/*! \brief Gets the original called bases.
	 *	\return Array of original calls.
	 */
	const FusionBaseCallVector& GetOrigCalls();

	/*! \brief Gets the original called base at the given index.
	 *	\param index Index to the original calls array.
	 *	\return Original call.
	 */
	FusionBaseCallType GetOrigCall(int32_t index);

	/*! \brief Gets the size of the original calls array.
	 *	\return Size of the original calls array.
	 */
	int32_t GetOrigCallsSize() const;

	/*! \brief Resizes the original calls array.
	 *	\param size Size of the array.
	 */
	void ResizeOrigCalls(int32_t size);

	/*! \brief Sets the original call.
	 * \param index Index to the array.
	 * \param call Original call.
	 */
	void SetOrigCall(int32_t index, FusionBaseCallType call);

	/*! \brief Adds the original call to the end of the list.
	 *	\param call Original call.
	 */
	void AddOrigCall(FusionBaseCallType call);
};

////////////////////////////////////////////////////////////////////

} // namespace


#endif // !defined(_FusionProbeSetResults_HEADER_)
