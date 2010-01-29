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


#ifndef _AuditFileReader_HEADER_
#define _AuditFileReader_HEADER_

/*! \file AuditFileReader.h This file provides interfaces to read an audit file.
 */

#include "calvin_files/array/src/ArrayAudit.h"
//

namespace affymetrix_calvin_io
{

/*! This class provides interfaces to read an audit file. */
class AuditFileReader
{
public:
	/*! Constructor */
	AuditFileReader();

	/*! Destructor */
	~AuditFileReader();

public:
	/*! Reads the entire contents of the file.
	 *
	 * @param fileName The name of the audit file to read.
	 * @param auditData The audit data to read from the file.
	 */
	bool Read(const std::string &fileName, affymetrix_calvin_array::ArrayAuditEntryList &auditData);
};

};

#endif // _AuditFileReader_HEADER_
