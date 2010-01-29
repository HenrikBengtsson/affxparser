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


#include "calvin_files/fusion/src/FusionProbeSetResults.h"
//
#include "file/CHPFileData.h"
//

using namespace affymetrix_fusion_io;

FusionExpressionProbeSetResults::FusionExpressionProbeSetResults()
{
	Clear();
}

FusionExpressionProbeSetResults::~FusionExpressionProbeSetResults() {}

FusionExpressionProbeSetResults FusionExpressionProbeSetResults::operator=(FusionExpressionProbeSetResults &src)
{
	SetDetectionPValue(src.GetDetectionPValue());
	SetSignal(src.GetSignal());
	SetNumPairs(src.GetNumPairs());
	SetNumUsedPairs(src.GetNumUsedPairs());
	SetDetection(src.GetDetection());
	SetHasCompResults(src.HasCompResults());
	SetChangePValue(src.GetChangePValue());
	SetSignalLogRatio(src.GetSignalLogRatio());
	SetSignalLogRatioLow(src.GetSignalLogRatioLow());
	SetSignalLogRatioHigh(src.GetSignalLogRatioHigh());
	SetNumCommonPairs(src.GetNumCommonPairs());
	SetChange(src.GetChange());
	return *this;
}

void FusionExpressionProbeSetResults::Clear()
{
	SetDetectionPValue(0.0);
	SetSignal(0.0);
	SetNumPairs(0);
	SetNumUsedPairs(0);
	SetDetection(0);
	SetHasCompResults(false);
	SetChangePValue(0.0);
	SetSignalLogRatio(0.0);
	SetSignalLogRatioLow(0.0);
	SetSignalLogRatioHigh(0.0);
	SetNumCommonPairs(0);
	SetChange(0);
}

float FusionExpressionProbeSetResults::GetDetectionPValue() const
{
	return detectionPValue;
}

float FusionExpressionProbeSetResults::GetSignal() const
{
	return signal;
}

u_int16_t FusionExpressionProbeSetResults::GetNumPairs() const
{
	return numPairs;
}

u_int16_t FusionExpressionProbeSetResults::GetNumUsedPairs() const
{
	return numPairsUsed;
}

u_int8_t FusionExpressionProbeSetResults::GetDetection() const
{
	return detection;
}

bool FusionExpressionProbeSetResults::HasCompResults() const
{
	return hasCompResults;
}

float FusionExpressionProbeSetResults::GetChangePValue() const
{
	return changePValue;
}

float FusionExpressionProbeSetResults::GetSignalLogRatio() const
{
	return sigLogRatio;
}

float FusionExpressionProbeSetResults::GetSignalLogRatioLow() const
{
	return sigLogRatioLo;
}

float FusionExpressionProbeSetResults::GetSignalLogRatioHigh() const
{
	return sigLogRatioHi;
}

u_int16_t FusionExpressionProbeSetResults::GetNumCommonPairs() const
{
	return numCommonPairs;
}

u_int8_t FusionExpressionProbeSetResults::GetChange() const
{
	return change;
}

void FusionExpressionProbeSetResults::SetDetectionPValue(float p)
{
	detectionPValue = p;
}

void FusionExpressionProbeSetResults::SetSignal(float p)
{
	signal = p;
}

void FusionExpressionProbeSetResults::SetNumPairs(u_int16_t p)
{
	numPairs = p;
}

void FusionExpressionProbeSetResults::SetNumUsedPairs(u_int16_t p)
{
	numPairsUsed = p;
}

void FusionExpressionProbeSetResults::SetDetection(u_int8_t p)
{
	detection = p;
}

void FusionExpressionProbeSetResults::SetHasCompResults(bool p)
{
	hasCompResults = p;
}

void FusionExpressionProbeSetResults::SetChangePValue(float p)
{
	changePValue = p;
}

void FusionExpressionProbeSetResults::SetSignalLogRatio(float p)
{
	sigLogRatio = p;
}

void FusionExpressionProbeSetResults::SetSignalLogRatioLow(float p)
{
	sigLogRatioLo = p;
}

void FusionExpressionProbeSetResults::SetSignalLogRatioHigh(float p)
{
	sigLogRatioHi = p;
}

void FusionExpressionProbeSetResults::SetNumCommonPairs(u_int16_t p)
{
	numCommonPairs = p;
}

void FusionExpressionProbeSetResults::SetChange(u_int8_t p)
{
	change = p;
}

std::string FusionExpressionProbeSetResults::GetDetectionString()
{
	switch (GetDetection())
	{
	case ABS_PRESENT_CALL:
		return "P";
		break;

	case ABS_MARGINAL_CALL:
		return "M";
		break;

	case ABS_ABSENT_CALL:
		return "A";
		break;

	case ABS_NO_CALL:
		return "No Call";
		break;

	default:
		break;
	}
	return "";
}

std::string FusionExpressionProbeSetResults::GetChangeString()
{
	switch (GetChange())
	{
	case COMP_INCREASE_CALL:
		return "I";
		break;

	case COMP_DECREASE_CALL:
		return "D";
		break;

	case COMP_MOD_INCREASE_CALL:
		return "MI";
		break;

	case COMP_MOD_DECREASE_CALL:
		return "MD";
		break;

	case COMP_NO_CHANGE_CALL:
		return "NC";
		break;

	case COMP_NO_CALL:
		return "No Call";
		break;

	default:
		break;
	}
	return "";
}

////////////////////////////////////////////////////////////////////

FusionGenotypeProbeSetResults::FusionGenotypeProbeSetResults()
{
Clear();
}

FusionGenotypeProbeSetResults::~FusionGenotypeProbeSetResults() {}

FusionGenotypeProbeSetResults FusionGenotypeProbeSetResults::operator=(FusionGenotypeProbeSetResults &src)
{
	SetAlleleCall(src.GetAlleleCall());
	SetConfidence(src.GetConfidence());
	SetRAS1(src.GetRAS1());
	SetRAS2(src.GetRAS2());
	SetPValueAA(src.GetPValueAA());
	SetPValueAB(src.GetPValueAB());
	SetPValueBB(src.GetPValueBB());
	SetPValueNoCall(src.GetPValueNoCall());
	return *this;
}

void FusionGenotypeProbeSetResults::Clear()
{
	SetAlleleCall(0);
	SetConfidence(0.0);
	SetRAS1(0.0);
	SetRAS2(0.0);
	SetPValueAA(0.0);
	SetPValueAB(0.0);
	SetPValueBB(0.0);
	SetPValueNoCall(0.0);
}

u_int8_t FusionGenotypeProbeSetResults::GetAlleleCall()
{
	return alleleCall;
}

float FusionGenotypeProbeSetResults::GetConfidence()
{
	return confidence;
}

float FusionGenotypeProbeSetResults::GetRAS1()
{
	return RAS1;
}

float FusionGenotypeProbeSetResults::GetRAS2()
{
	return RAS2;
}

float FusionGenotypeProbeSetResults::GetPValueAA()
{
	return pValueAA;
}

float FusionGenotypeProbeSetResults::GetPValueAB()
{
	return pValueAB;
}

float FusionGenotypeProbeSetResults::GetPValueBB()
{
	return pValueBB;
}

float FusionGenotypeProbeSetResults::GetPValueNoCall()
{
	return pValueNoCall;
}

void FusionGenotypeProbeSetResults::SetAlleleCall(u_int8_t p)
{
	alleleCall = p;
}

void FusionGenotypeProbeSetResults::SetConfidence(float p)
{
	confidence = p;
}

void FusionGenotypeProbeSetResults::SetRAS1(float p)
{
	RAS1 = p;
}

void FusionGenotypeProbeSetResults::SetRAS2(float p)
{
	RAS2 = p;
}

void FusionGenotypeProbeSetResults::SetPValueAA(float p)
{
	pValueAA = p;
}

void FusionGenotypeProbeSetResults::SetPValueAB(float p)
{
	pValueAB = p;
}

void FusionGenotypeProbeSetResults::SetPValueBB(float p)
{
	pValueBB = p;
}

void FusionGenotypeProbeSetResults::SetPValueNoCall(float p)
{
	pValueNoCall = p;
}

	std::string FusionGenotypeProbeSetResults::GetAlleleCallString()
{
	switch (GetAlleleCall())
	{
	case ALLELE_A_CALL:
		return "A";
		break;

	case ALLELE_B_CALL:
		return "B";
		break;

	case ALLELE_AB_CALL:
		return "AB";break;

	default:
		return "No Call";
		break;
	}
	return "";
}

////////////////////////////////////////////////////////////////////

float FusionUniversalProbeSetResults::GetBackground() const
{
	return background;
}

void FusionUniversalProbeSetResults::SetBackground(float bg)
{
	background = bg;
}

void FusionUniversalProbeSetResults::Clear()
{
	background = 0.0;
}

FusionUniversalProbeSetResults FusionUniversalProbeSetResults::operator=(FusionUniversalProbeSetResults &src)
{
	SetBackground(src.GetBackground());
	return *this;
}

FusionUniversalProbeSetResults::FusionUniversalProbeSetResults()
{
	Clear();
}

FusionUniversalProbeSetResults::~FusionUniversalProbeSetResults() {}

////////////////////////////////////////////////////////////////////

FusionForceCallType::FusionForceCallType()
{
	Clear();
}

FusionForceCallType::FusionForceCallType(int32_t p, int8_t c, u_int8_t r)
{
	position = p;
	call = c;
	reason = r;
}

FusionForceCallType::~FusionForceCallType()
{
	Clear();
}

void FusionForceCallType::Clear()
{
	position = 0;
	call = 0;
	reason = 0;
}

////////////////////////////////////////////////////////////////////

FusionBaseCallType::FusionBaseCallType()
{
	Clear();
}

FusionBaseCallType::FusionBaseCallType(int32_t p, int8_t c)
{
	position = p;
	call = c;
}

FusionBaseCallType::~FusionBaseCallType()
{
	Clear();
}

void FusionBaseCallType::Clear()
{
	position = 0;
	call = 0;
}

////////////////////////////////////////////////////////////////////

FusionResequencingResults::FusionResequencingResults() 
{
	Clear();
}

FusionResequencingResults::~FusionResequencingResults()
{
	Clear();
}

void FusionResequencingResults::Clear()
{
	calledBases.clear();
	scores.clear();
	forceCalls.clear();
	origCalls.clear();
}

const Int8Vector& FusionResequencingResults::GetCalledBases()
{
	return calledBases;
}

int8_t FusionResequencingResults::GetCalledBase(int index)
{
	return calledBases[index];
}

int32_t FusionResequencingResults::GetCalledBasesSize() const
{
	return (int32_t)calledBases.size();
}


void FusionResequencingResults::ResizeCalledBases(int32_t size)
{
	calledBases.resize(size);
}

void FusionResequencingResults::SetCalledBase(int32_t index, int8_t call)
{
	calledBases[index] = call;
}

void FusionResequencingResults::AddCalledBase(int8_t call)
{
	calledBases.push_back(call);
}

const FloatVector& FusionResequencingResults::GetScores()
{
	return scores;
}

float FusionResequencingResults::GetScore(int32_t index)
{
	return scores[index];
}

int32_t FusionResequencingResults::GetScoresSize() const
{
	return (int32_t)scores.size();
}

void FusionResequencingResults::ResizeScores(int32_t size)
{
	scores.resize(size);
}

void FusionResequencingResults::SetScore(int index, float score)
{
	scores[index] = score;
}

void FusionResequencingResults::AddScore(float score)
{
	scores.push_back(score);
}

const FusionForceCallVector& FusionResequencingResults::GetForceCalls()
{
	return forceCalls;
}

FusionForceCallType FusionResequencingResults::GetForceCall(int32_t index)
{
	return forceCalls[index];
}

int32_t FusionResequencingResults::GetForceCallsSize() const
{
	return (int32_t)forceCalls.size();
}

void FusionResequencingResults::ResizeForceCalls(int32_t size)
{
	forceCalls.resize(size);
}

void FusionResequencingResults::SetForceCall(int32_t index, FusionForceCallType call)
{
	forceCalls[index] = call;
}

void FusionResequencingResults::AddForceCall(FusionForceCallType call)
{
	forceCalls.push_back(call);
}

const FusionBaseCallVector& FusionResequencingResults::GetOrigCalls()
{
	return origCalls;
}

FusionBaseCallType FusionResequencingResults::GetOrigCall(int32_t index)
{
	return origCalls[index];
}

int32_t FusionResequencingResults::GetOrigCallsSize() const
{
	return (int32_t)origCalls.size();
}

void FusionResequencingResults::ResizeOrigCalls(int32_t size)
{
	origCalls.resize(size);
}

void FusionResequencingResults::SetOrigCall(int32_t index, FusionBaseCallType call)
{
	origCalls[index] = call;
}

void FusionResequencingResults::AddOrigCall(FusionBaseCallType call)
{
	origCalls.push_back(call);
}
