////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#include "calvin_files/parsers/src/ParameterFileReader.h"
//
#include "calvin_files/parsers/src/SAXParameterFileHandlers.h"
//
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
//
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
XERCES_CPP_NAMESPACE_USE;

/*
 * Initialize the class.
 */
ParameterFileReader::ParameterFileReader()
{
}

/*
 * Clear the data.
 */
ParameterFileReader::~ParameterFileReader()
{
}

/*
 * Read the entire file using the XML SAX parser.
 */
bool ParameterFileReader::Read(const std::string &fileName, ParameterFileData &parameterFileData)
{
	parameterFileData.Clear();

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
	SAXParameterFileHandlers handler(&parameterFileData);
	parser->setDocumentHandler(&handler);
	parser->setErrorHandler(&handler);
	try
	{
		parser->parse(fileName.c_str());
		int errorCount = parser->getErrorCount();
		if (errorCount == 0)
		{
			status = true;
		}
	}
	catch (...)
	{
		status = false;
	}
	delete parser;
	XMLPlatformUtils::Terminate();
	return status;
}
