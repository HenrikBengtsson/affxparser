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

#include "file/DttArrayFileReader.h"
//
#include "file/SAXDttArrayHandlers.h"
//
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
//
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_dttarray;
XERCES_CPP_NAMESPACE_USE;

/*
 * Initialize the class.
 */
DttArrayData::DttArrayData()
{
}

/*
 * Clear the data.
 */
DttArrayData::~DttArrayData()
{
	Clear();
}

/*
 * Clear the data.
 */
void DttArrayData::Clear()
{
	attributes.clear();
	arrayType = "";
	expName = "";
}

/*
 * Initialize the class.
 */
DttArrayFileReader::DttArrayFileReader()
{
}

/*
 * Clear the data.
 */
DttArrayFileReader::~DttArrayFileReader()
{
}

/*
 * Read the entire file using the XML SAX parser.
 */
bool DttArrayFileReader::Read(DttArrayData &arrayData)
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
	SAXArrayHandlers handler(&arrayData);
	parser->setDocumentHandler(&handler);
	parser->setErrorHandler(&handler);
	try
	{
		parser->parse(m_FileName.c_str());
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

/*
 * Check if the file exists.
 */
bool DttArrayFileReader::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_FileName.c_str(), &st) == 0);
}
