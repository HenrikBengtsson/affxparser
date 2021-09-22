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


#include "calvin_files/array/src/CreateStep.h"
//

using namespace affymetrix_calvin_array;

/*! The none step. */
#define CREATED_STEP_NONE std::wstring(L"None")

/*! The array registration step. */
#define CREATED_STEP_ARRAY_REG std::wstring(L"ArrayRegistration")

/*! The scan step. */
#define CREATED_STEP_SCAN std::wstring(L"Scanning")

/*! The grid alignment step. */
#define CREATED_STEP_GRID std::wstring(L"Gridding")

/*! The cel analysis step. */
#define CREATED_STEP_CEL std::wstring(L"CELAnalysis")

/*! Other undefined step. */
#define CREATED_STEP_OTHER std::wstring(L"Other")

/*! From step. */
#define CREATED_STEP_FROM std::wstring(L"From")

/*! Job order step. */
#define CREATED_STEP_JOB_ORDER std::wstring(L"JobOrderServer")

/*! File indexer step. */
#define CREATED_STEP_FILE_INDEXER std::wstring(L"FileIndexer")

/*
 * Convert the step type to a string.
 */
std::wstring affymetrix_calvin_array::CreateStepToString(CreateStep step)
{
	switch (step)
	{
	case NoStep:
		return CREATED_STEP_NONE;
		break;

	case ArrayRegistrationStep:
		return CREATED_STEP_ARRAY_REG;
		break;

	case ScanningStep:
		return CREATED_STEP_SCAN;
		break;

	case GriddingStep:
		return CREATED_STEP_GRID;
		break;

	case CELAnalysisStep:
		return CREATED_STEP_CEL;
		break;

	case OtherStep:
		return CREATED_STEP_OTHER;
		break;

	case FromStep:
		return CREATED_STEP_FROM;
		break;
		
	case JobOrderServerStep:
		return CREATED_STEP_JOB_ORDER;
		break;
		
	case FileIndexerStep:
		return CREATED_STEP_FILE_INDEXER;
		break;

	default:
		return L"";
		break;
	}
}

/*
 * Convert a string to step type.
 */
CreateStep affymetrix_calvin_array::CreateStepFromString(const std::wstring &step)
{
	if (step == CREATED_STEP_NONE)
		return NoStep;

	else if (step == CREATED_STEP_ARRAY_REG)
		return ArrayRegistrationStep;

	else if (step == CREATED_STEP_SCAN)
		return ScanningStep;

	else if (step == CREATED_STEP_GRID)
		return GriddingStep;

	else if (step == CREATED_STEP_CEL)
		return CELAnalysisStep;

	else if (step == CREATED_STEP_OTHER)
		return OtherStep;

	else if (step == CREATED_STEP_FROM)
		return FromStep;
		
	else if (step == CREATED_STEP_JOB_ORDER)
		return JobOrderServerStep;

	else if (step == CREATED_STEP_FILE_INDEXER)
		return FileIndexerStep;

	else
		return OtherStep;
}

