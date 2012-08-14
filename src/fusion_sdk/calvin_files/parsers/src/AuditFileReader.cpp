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

//
#include "calvin_files/parsers/src/AuditFileReader.h"
//
#include "calvin_files/parsers/src/AuditFileConstants.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include "util/Fs.h"
//
#include <cstring>
#include <fstream>
#include <string.h>
#include <string>
//

using namespace affymetrix_calvin_array;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;
using namespace std;

/*
 * Initialize the class.
 */
AuditFileReader::AuditFileReader()
{
}

/*
 * Clear the data.
 */
AuditFileReader::~AuditFileReader()
{
}

/*
 * Read the entire file.
 */
bool AuditFileReader::Read(const std::string &fileName, affymetrix_calvin_array::ArrayAuditEntryList &auditData)
{
	auditData.clear();

	// Open the file.
	std::ifstream fileStream;
        Fs::aptOpen(fileStream, fileName);
	if (!fileStream)
	{
		return false;
	}

	ArrayAuditEntry entry;
	DateTime dateTime;
	const int MAX_LINE_LENGTH = 1024;
	char buf[MAX_LINE_LENGTH];
	std::string line;
	std::string name;
	std::string value;

	// Read the lines of the file.
	while (fileStream.getline(buf, MAX_LINE_LENGTH))
	{
		line = buf;

		// Start tag
		if (line == AUDIT_START_TAG)
		{
			dateTime.Clear();
			entry.Clear();
			continue;
		}

		// End tag
		else if (line == AUDIT_END_TAG)
		{
			entry.DateTime() = dateTime;
			auditData.push_back(entry);
			continue;
		}

		// Split the line to the name and value separated by a = sign
		const char *index = strstr(line.c_str(), "=");
		if (index)
		{
			value = index+1;
			name = line.substr(0, line.length()-strlen(index));

			// User name tag
			if (name == AUDIT_USER_TAG)
				entry.UserName() = StringUtils::ConvertMBSToWCS(value);

			// Date time
			else if (name == AUDIT_DATE_TAG)
				dateTime.Date(StringUtils::ConvertMBSToWCS(value));

			// Date time
			else if (name == AUDIT_TIME_TAG)
				dateTime.Time(StringUtils::ConvertMBSToWCS(value));

			// Action
			else if (name == AUDIT_ACTION_TAG)
				entry.ActionType() = value;

			// Array id
			else if (name == AUDIT_ARRAY_ID_TAG)
				entry.ArrayGuid() = value;

			// Input id
			else if (name == AUDIT_INPUT_ID_TAG)
				entry.InputFileGuids().push_back(value);

			// Output id
			else if (name == AUDIT_OUTPUT_ID_TAG)
				entry.OutputFileGuids().push_back(value);

			// Parameter name/value pair.
			else
			{
				ParameterNameValuePair param;
				param.Name = StringUtils::ConvertMBSToWCS(name);
				param.Value = StringUtils::ConvertMBSToWCS(value);
				entry.ActionParameters().push_back(param);
			}
		}
	}

	// Close the file and return the status
	fileStream.close();
	return (fileStream.fail() != 0);
}
