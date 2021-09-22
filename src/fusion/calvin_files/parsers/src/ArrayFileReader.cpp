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


#include "calvin_files/parsers/src/ArrayFileReader.h"
//
#include "calvin_files/parsers/src/SAXArrayHandlers.h"
//
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
//
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_calvin_array;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;
XERCES_CPP_NAMESPACE_USE;

/*
 * Initialize the class.
 */
ArrayFileReader::ArrayFileReader()
{
}

/*
 * Clear the data.
 */
ArrayFileReader::~ArrayFileReader()
{
}

/*
 * Read the entire file using the XML SAX parser.
 */
bool ArrayFileReader::Read(const std::string &fileName, affymetrix_calvin_array::ArrayData &arrayData, bool headerOnly)
{
	arrayData.Clear();

	// Initialize the XML4C2 system
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException&)
	{
		return false;
	}

	bool status = false;
	SAXParser* parser = new SAXParser;
	parser->setValidationScheme(SAXParser::Val_Never);
	parser->setLoadExternalDTD(false);
	parser->setDoNamespaces(false);
	parser->setDoSchema(false);
	parser->setValidationSchemaFullChecking(false);
	SAXArrayHandlers handler(&arrayData, headerOnly);
	parser->setDocumentHandler(&handler);
	parser->setErrorHandler(&handler);
	try
	{
		parser->parse(fileName.c_str());
		int errorCount = parser->getErrorCount();
		if (errorCount == 0)
		{
			status = true;
			fileVersionNumber = handler.FileVersionNumber();
		}
	}
	catch (SAXArrayStopParsingException)
	{
		status = true;
		fileVersionNumber = handler.FileVersionNumber();
	}
	catch (...)
	{
		status = false;
	}
	delete parser;
	XMLPlatformUtils::Terminate();
	return status;
}

/*
 * Check if the data type matches what is in the file.
 */
bool ArrayFileReader::IsFileType(const std::string &fileName, const affymetrix_calvin_utilities::AffymetrixGuidType &dataTypeId)
{
	return (ArrayFileReader::DataTypeIdentifier(fileName) == dataTypeId);
}

/*
 * Read just the first few entries to determine if this file is
 * of the right type. Check the magic number, version number
 * and data type identifier. If they all match then this is the right
 * type of file.
 */
affymetrix_calvin_utilities::AffymetrixGuidType ArrayFileReader::DataTypeIdentifier(const std::string &fileName)
{
	ArrayFileReader reader;
	ArrayData arrayData;
	reader.Read(fileName, arrayData, true);
	return arrayData.DataTypeIdentifier();
}
