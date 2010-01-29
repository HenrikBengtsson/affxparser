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


#ifndef _CreateStep_HEADER_
#define _CreateStep_HEADER_

/*! \file CreateStep.h This file provides a type for definining a step that created an array object.
 */

#include <cstring>
#include <string>
//

namespace affymetrix_calvin_array
{

/*! Defines the steps that can create array data. */
typedef enum _CreateStep
{
	NoStep,					/*! No step. */
	ArrayRegistrationStep,	/*! Array registration. */
	ScanningStep,			/*! Scanning. */
	GriddingStep,			/*! Grid analysis. */
	CELAnalysisStep,		/*! CEL file analysis. */
	FromStep,				/*! From. */
	JobOrderServerStep,		/*! Job order. */
	FileIndexerStep,		/*! File indexer. */
	OtherStep				/*! Any other step. */
} CreateStep;

/*! Converts the step type to a string.
 * @param step The create step
 * @return The string representation.
 */
std::wstring CreateStepToString(CreateStep step);

/*! Converts a string to step type.
 * @param step The string representation.
 * @return The create step
 */
CreateStep CreateStepFromString(const std::wstring &step);

}

#endif // _CreateStep_HEADER_
