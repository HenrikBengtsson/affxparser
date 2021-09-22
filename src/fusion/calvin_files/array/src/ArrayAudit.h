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


#ifndef _ArrayAudit_HEADER_
#define _ArrayAudit_HEADER_

/*! \file ArrayAudit.h This file provides interfaces for an audit item in an array file.
 */

#include "calvin_files/parameter/src/Parameter.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
#include "calvin_files/utils/src/DateTime.h"
//
#include <cstring>
#include <list>
#include <string>
//

namespace affymetrix_calvin_array
{

/*! A class to store an audit entry associated with an array. */
class ArrayAuditEntry
{
public:
	/*! Constructor */
	ArrayAuditEntry();

	/*! Destructor */
	~ArrayAuditEntry();

protected:
	/*! The user name creating the audit entry. */
	std::wstring userName;

	/*! The type of action being performed. */
	affymetrix_calvin_utilities::AffymetrixGuidType actionType;

	/*! The guid to the physical array associated with the action. */ 	 
	affymetrix_calvin_utilities::AffymetrixGuidType arrayGuid; 	 

	/*! A list of guids for the input file ids to the action. */ 	 
	affymetrix_calvin_utilities::AffymetrixGuidTypeList inputFileGuids; 	 

	/*! A list of guids for the output file ids from the action. */ 	 
	affymetrix_calvin_utilities::AffymetrixGuidTypeList outputFileGuids; 	 

	/*! A list of parameters associated with the action. */
	affymetrix_calvin_parameter::ParameterNameValuePairList actionParameters;

	/*! The date and time of the action. */
	affymetrix_calvin_utilities::DateTime actionDateTime;

public:
	/*! The user name associated with the audit entry.
	 *
	 * @return The user name associated with the audit entry.
	 */
	std::wstring &UserName() { return userName; }

	/*! The date and time of the action.
	 *
	 * @return The date and time of the action.
	 */
	affymetrix_calvin_utilities::DateTime &DateTime() { return actionDateTime; }

	/*! The type of action.
	 *
	 * @return The type of action.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &ActionType() { return actionType; }

	/*! The id of the physical array associated with the action.
	 *
	 * @return The physical array id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &ArrayGuid() { return arrayGuid; }

	/*! The list of guids for the input files to the action. 	 
	* 	 
	* @return  The list of guids for the input files to the action. 	 
	*/ 	 
	affymetrix_calvin_utilities::AffymetrixGuidTypeList &InputFileGuids() { return inputFileGuids; } 	 

	/*! The list of guids for the input files to the action. 	 
	* 	 
	* @return  The list of guids for the output files of the action. 	 
	*/ 	 
	affymetrix_calvin_utilities::AffymetrixGuidTypeList &OutputFileGuids() { return outputFileGuids; }

	/*! The list of parameters associated with the action.
	 *
	 * @return The list of parameters associated with the action.
	 */
	affymetrix_calvin_parameter::ParameterNameValuePairList &ActionParameters() { return actionParameters; }

	/*! Clears the member objects. */
	void Clear();
};

/*! An STL vector of tbd objects */
typedef std::vector<ArrayAuditEntry> ArrayAuditEntryVector;

/*! An STL list of tbd objects */
typedef std::list<ArrayAuditEntry> ArrayAuditEntryList;

};

#endif // _ArrayAudit_HEADER_
