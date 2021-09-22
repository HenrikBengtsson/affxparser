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


#include "calvin_files/parsers/src/SAXParameterFileHandlers.h"
//
#include "calvin_files/utils/src/StringUtils.h"
//
#include <cstring>
#include <map>
#include <string>
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;
using namespace std;
XERCES_CPP_NAMESPACE_USE;

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif


/*! Converts a XML character string to wide string.
 * @param c1 The XML string to convert
 * @return The wide character string
 */
static wstring XMLChToWString(const XMLCh* const c1)
{
	wstring s;
	int i=0;
	while (c1[i] != 0)
	{
		s += c1[i];
		++i;
	}
	return s;
}

/*! Comparison operator for XML strings to wide character strings.
 * @param c1 The XML string
 * @param c2 The wide string
 * @return True if equal
 */
static bool operator==(const XMLCh* const &c1, wstring c2)
{
	if (c2.length() != XMLString::stringLen(c1))
		return false;
	for (int i=0; i<(int)c2.length(); i++)
	{
		if (c2[i] != c1[i])
			return false;
	}
	return true;
}

/*
 * Store the data and set the starting element to the head.
 */
SAXParameterFileHandlers::SAXParameterFileHandlers(ParameterFileData *data) :
	parameterFileData(data),
	currentElement(PARAMETER_FILE)
{
}

/*
 * Destructor
 */
SAXParameterFileHandlers::~SAXParameterFileHandlers()
{
}

/*
 * No processing needed. This is only here to provide a derived function.
 */
void SAXParameterFileHandlers::startDocument()
{
}

/*
 * No processing needed. This is only here to provide a derived function.
 */
void SAXParameterFileHandlers::endDocument()
{
}

/*
 * Back up the current element.
 */
void SAXParameterFileHandlers::endElement(const XMLCh* const name)
{
	MoveCurrentElementBack(name);
}

/*
 * Back up the current element.
 */
void SAXParameterFileHandlers::MoveCurrentElementBack(const XMLCh* const name)
{
	if (name == 0)
		return;

	if (name == PARAMETER_ELEMENT)
		currentElement = PARAMETER_FILE;

	else if (name == IMPLEMENTATION_ELEMENT)
		currentElement = PARAMETER_FILE;
}

/*
 * Advance the current element.
 */
bool SAXParameterFileHandlers::MoveCurrentElementForward(const XMLCh* const name)
{
	if (name == 0)
		return false;

	if (name == PARAMETER_FILE_ELEMENT)
		currentElement = PARAMETER_FILE;

	else if (name == IMPLEMENTATION_ELEMENT)
		currentElement = IMPLEMENTATION;

	else if (name == PARAMETER_ELEMENT)
		currentElement = PARAMETER;

	else
		return false;

	return true;
}

/*
 * Set the current element based on the name and the current element.
 * Based on the current element, store the data in the array object.
 */
void SAXParameterFileHandlers::startElement(const XMLCh* const name, AttributeList&  attributes)
{
	if (MoveCurrentElementForward(name) == false)
		return;

	map<wstring, wstring> atts;
	unsigned int len = attributes.getLength();
	for (unsigned int index = 0; index < len; index++)
	{
		atts[XMLChToWString(attributes.getName(index))] = XMLChToWString(attributes.getValue(index));
	}

	switch (currentElement)
	{
	case PARAMETER_FILE:
		StoreParameterFileAttributes(atts);
		break;

	case IMPLEMENTATION:
		StoreImplementationAttributes(atts);
		break;

	case PARAMETER:
		StoreParameterAttributes(atts);
		break;

	default:
		break;
	}
}

/*
 * Based on the current element, assign the value.
 */
void SAXParameterFileHandlers::characters(const XMLCh *const chars, const unsigned int length)
{
}

/*
 * Store the parameter attributes.
 */
void SAXParameterFileHandlers::StoreParameterAttributes(map<wstring, wstring> &attributes)
{
    ParameterType param;

    param.name = attributes[PARAMETER_NAME_ATTRIBUTE];
    param.index = attributes[PARAMETER_INDEX_ATTRIBUTE];
    param.displayName = attributes[PARAMETER_DISPLAY_NAME_ATTRIBUTE];
    param.category = attributes[PARAMETER_CATEGORY_ATTRIBUTE];
    param.isEditable = attributes[PARAMETER_ISEDITABLE_ATTRIBUTE];
    param.type = attributes[PARAMETER_TYPE_ATTRIBUTE];
    param.currentValue = attributes[PARAMETER_CURVALUE_ATTRIBUTE];
    param.minValue = attributes[PARAMETER_MINVALUE_ATTRIBUTE];
    param.maxValue = attributes[PARAMETER_MAXVALUE_ATTRIBUTE];
    param.defaultValue = attributes[PARAMETER_DEFVALUE_ATTRIBUTE];
    param.precision = attributes[PARAMETER_PRECISION_ATTRIBUTE];
    param.maxLength = attributes[PARAMETER_MAXLENGTH_ATTRIBUTE];
    param.description = attributes[PARAMETER_DESC_ATTRIBUTE];
        
    parameterFileData->Parameters().push_back(param);
}

/*
 * Store the implementation attributes.
 */
void SAXParameterFileHandlers::StoreImplementationAttributes(map<wstring, wstring> &attributes)
{
    parameterFileData->ImplementationAttributes().name = attributes[IMPLEMENTATION_NAME_ATTRIBUTE];
    parameterFileData->ImplementationAttributes().version = attributes[IMPLEMENTATION_VERSION_ATTRIBUTE];
    parameterFileData->ImplementationAttributes().executableFileName = attributes[IMPLEMENTATION_EXE_FILE_NAME_ATTRIBUTE];
    parameterFileData->ImplementationAttributes().description = attributes[IMPLEMENTATION_DESC_ATTRIBUTE];
}

/*
 * Store the parameter file attributes
 */
void SAXParameterFileHandlers::StoreParameterFileAttributes(map<wstring, wstring> &attributes)
{
    parameterFileData->ParameterFileAttributes().company = attributes[PARAMETER_FILE_COMPANY_ATTRIBUTE];
    parameterFileData->ParameterFileAttributes().userName = attributes[PARAMETER_FILE_USER_NAME_ATTRIBUTE];
    parameterFileData->ParameterFileAttributes().contentVersion = attributes[PARAMETER_FILE_CONTENT_VERSION_ATTRIBUTE];
}
