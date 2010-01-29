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


#include "calvin_files/writers/src/AuditFileWriter.h"
//
#include "calvin_files/parsers/src/AuditFileConstants.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include <fstream>
//

using namespace affymetrix_calvin_array;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_io;
using namespace std;

/*
 * Initialize the class.
 */
AuditFileWriter::AuditFileWriter()
{
}

/*
 * Clear the data.
 */
AuditFileWriter::~AuditFileWriter()
{
}

/*
 * Write the entry members in a tag=value style. Enclose the entry in a #Entry tag.
 */
bool AuditFileWriter::Write(const std::string &fileName, affymetrix_calvin_array::ArrayAuditEntry &auditData)
{
	ofstream auditFile(fileName.c_str(), ios::out | ios::app);
	if (!auditFile)
		return false;

	auditFile << AUDIT_START_TAG << endl;
	auditFile << AUDIT_USER_TAG << "=" << StringUtils::ConvertWCSToMBS(auditData.UserName()) << endl;
	auditFile << AUDIT_DATE_TAG << "=" << StringUtils::ConvertWCSToMBS(auditData.DateTime().Date()) << endl;
	auditFile << AUDIT_TIME_TAG << "=" << StringUtils::ConvertWCSToMBS(auditData.DateTime().Time()) << endl;
	auditFile << AUDIT_ACTION_TAG << "=" << auditData.ActionType() << endl;
	auditFile << AUDIT_ARRAY_ID_TAG << "=" << auditData.ArrayGuid() << endl;

	for (AffymetrixGuidTypeList::iterator it = auditData.InputFileGuids().begin();
		it != auditData.InputFileGuids().end(); ++it)
	{
		AffymetrixGuidType guid = (*it);
		auditFile << AUDIT_INPUT_ID_TAG << "=" << guid << endl;
	}

	for (AffymetrixGuidTypeList::iterator it = auditData.OutputFileGuids().begin();
		it != auditData.OutputFileGuids().end(); ++it)
	{
		AffymetrixGuidType guid = (*it);
		auditFile << AUDIT_OUTPUT_ID_TAG << "=" << guid << endl;
	}

	for (ParameterNameValuePairList::iterator it = auditData.ActionParameters().begin();
		it != auditData.ActionParameters().end(); ++it)
	{
		ParameterNameValuePair param = (*it);
		auditFile << StringUtils::ConvertWCSToMBS(param.Name) << "=" << StringUtils::ConvertWCSToMBS(param.Value) << endl;
	}

	auditFile << AUDIT_END_TAG << endl;
	auditFile.close();

	return (auditFile.fail() == 0);
}
