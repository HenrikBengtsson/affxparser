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


#ifndef _AuditFileWriter_HEADER_
#define _AuditFileWriter_HEADER_

/*! \file AuditFileWriter.h This file provides interfaces to write an audit file.
 */

#include "calvin_files/array/src/ArrayAudit.h"
//

namespace affymetrix_calvin_io
{

/*! This class provides interfaces to write an audit file. */
class AuditFileWriter
{
public:
	/*! Constructor */
	AuditFileWriter();

	/*! Destructor */
	~AuditFileWriter();

public:
	/*! Writes the audit object to an audit file.
	 *
	 * @param fileName The name of the audit file to write.
	 * @param auditData The audit data to write to the file.
	 * @return True if the file was successfully written.
	 */
	bool Write(const std::string &fileName, affymetrix_calvin_array::ArrayAuditEntry &auditData);
};

};

#endif // _AuditFileWriter_HEADER_
