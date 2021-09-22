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


#ifndef _AuditFileConstants_HEADER_
#define _AuditFileConstants_HEADER_

/*! \file AuditFileConstants.h This file provides constants for an audit file.
 */

namespace affymetrix_calvin_io
{

/*! The tag for the user name */
#define AUDIT_USER_TAG "User"

/*! The tag for the date */
#define AUDIT_DATE_TAG "Date"

/*! The tag for the time */
#define AUDIT_TIME_TAG "Time"

/*! The tag for the action type */
#define AUDIT_ACTION_TAG "Action"

/*! The tag for the array id */
#define AUDIT_ARRAY_ID_TAG "ArrayId"

/*! The tag for an input id */
#define AUDIT_INPUT_ID_TAG "InputId"

/*! The tag for an output id */
#define AUDIT_OUTPUT_ID_TAG "OutputId"

/*! The tag for the start of an audit entry */
#define AUDIT_START_TAG "#Audit-Start"

/*! The tag for the end of an audit entry */
#define AUDIT_END_TAG "#Audit-End"

};


#endif // _AuditFileConstants_HEADER_
