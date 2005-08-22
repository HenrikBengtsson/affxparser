/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "SAXArrayHandlers.h"
#include "StringUtils.h"
#include <string>
#include <map>

using namespace affymetrix_calvin_array;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;
using namespace std;
XERCES_CPP_NAMESPACE_USE;

/*! Converts a string to an integer.
 * @param wstr The string to convert
 * @return The integer representation of the string.
 */
static int WideToInt(wstring const &wstr)
{   
	int i = 0;
	int result = 0;
	int length = (int) wstr.length();
	while (i<length)
	{
		result = result*10 + (int)wstr[i];
		i++;
	}      
	return result;
}

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
 * Store the array data and set the starting element to the head.
 */
SAXArrayHandlers::SAXArrayHandlers(ArrayData *data, bool headerOnly) :
	arrayData(data),
	currentElement(ARRAY_FILE),
	fileVersionNumber(0),
	readHeaderOnly(headerOnly)
{
}

/*
 * Destructor
 */
SAXArrayHandlers::~SAXArrayHandlers()
{
}

/*
 * No processing needed. This is only here to provide a derived function.
 */
void SAXArrayHandlers::startDocument()
{
}

/*
 * No processing needed. This is only here to provide a derived function.
 */
void SAXArrayHandlers::endDocument()
{
}

/*
 * Back up the current element.
 */
void SAXArrayHandlers::endElement(const XMLCh* const name)
{
	MoveCurrentElementBack(name);
}

/*
 * Back up the current element.
 */
void SAXArrayHandlers::MoveCurrentElementBack(const XMLCh* const name)
{
	if (name == 0)
		return;

	if (name == PHYSICAL_ARRAY_ATTRIBUTE_ELEMENT && currentElement == PHYSICAL_ARRAY_ATTRIBUTES)
		currentElement = PHYSICAL_ARRAY;

	else if (name == PHYSICAL_ARRAY_ELEMENT)
		currentElement = PHYSICAL_ARRAYS;

	else if (name == PHYSICAL_ARRAYS_ELEMENT)
		currentElement = ARRAY_FILE;

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT && currentElement == USER_ATTRIBUTES_ATTRIBUTE_CONTROL)
		currentElement = USER_ATTRIBUTES_ATTRIBUTE;

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_ELEMENT && currentElement == USER_ATTRIBUTES_ATTRIBUTE)
		currentElement = USER_ATTRIBUTES;

	else if (name == USER_ATTRIBUTES_ELEMENT)
		currentElement = ARRAY_FILE;
}

/*
 * Advance the current element.
 */
bool SAXArrayHandlers::MoveCurrentElementForward(const XMLCh* const name)
{
	if (name == 0)
		return false;

	if (name == ARRAY_FILE_ELEMENT)
		currentElement = ARRAY_FILE;

	else if (name == PHYSICAL_ARRAYS_ELEMENT)
		currentElement = PHYSICAL_ARRAYS;

	else if (name == PHYSICAL_ARRAY_ELEMENT)
		currentElement = PHYSICAL_ARRAY;

	else if (name == PHYSICAL_ARRAY_ATTRIBUTE_ELEMENT && currentElement == PHYSICAL_ARRAY)
		currentElement = PHYSICAL_ARRAY_ATTRIBUTES;

	else if (name == USER_ATTRIBUTES_ELEMENT)
		currentElement = USER_ATTRIBUTES;

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_ELEMENT && currentElement == USER_ATTRIBUTES)
		currentElement = USER_ATTRIBUTES_ATTRIBUTE;

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT && currentElement == USER_ATTRIBUTES_ATTRIBUTE)
		currentElement = USER_ATTRIBUTES_ATTRIBUTE_CONTROL;

	else if (name == ARRAY_FILE_ELEMENT ||
		name == PHYSICAL_ARRAYS_ELEMENT ||
		name == PHYSICAL_ARRAY_ELEMENT ||
		name == PHYSICAL_ARRAY_ATTRIBUTE_ELEMENT ||
		name == USER_ATTRIBUTES_ELEMENT ||
		name == USER_ATTRIBUTES_ATTRIBUTE_ELEMENT ||
		name == USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT)
	{
	}

	else
		return false;

	return true;
}

/*
 * Set the current element based on the name and the current element.
 * Based on the current element, store the data in the array object.
 */
void SAXArrayHandlers::startElement(const XMLCh* const name, AttributeList&  attributes)
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
	case ARRAY_FILE:
		StoreArrayFileAttributes(atts);
		break;

	case PHYSICAL_ARRAY:
		StorePhysicalArrayID(atts);
		break;

	case PHYSICAL_ARRAY_ATTRIBUTES:
		StorePhysicalArrayAttribute(atts);
		break;

	case USER_ATTRIBUTES_ATTRIBUTE:
		StoreUserAttribute(atts);
		break;

	case USER_ATTRIBUTES_ATTRIBUTE_CONTROL:
		StoreUserAttributeControl(atts);
		break;

	default:
		break;
	}

	if (readHeaderOnly)
	{
		SAXArrayStopParsingException e;
		throw e;
	}
}

/*
 * Store the user attribute.
 */
void SAXArrayHandlers::StoreUserAttribute(map<wstring, wstring> &attributes)
{
	int n = (int) arrayData->UserAttributes().size();
	arrayData->UserAttributes().resize(n+1);
	arrayData->UserAttributes()[n].Name = attributes[USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_NAME_ATTRIBUTE];
	arrayData->UserAttributes()[n].Value = attributes[USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_VALUE_ATTRIBUTE];
}

/*
 * Store the control value for a user attribute.
 */
void SAXArrayHandlers::StoreUserAttributeControl(map<wstring, wstring> &attributes)
{
	int n = (int) arrayData->UserAttributes().size();
	arrayData->UserAttributes()[n-1].ControlledVocabulary.push_back(
		attributes[USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT_VALUE_ATTRIBUTE]);
}

/*
 * Store the array attribute.
 */
void SAXArrayHandlers::StorePhysicalArrayAttribute(map<wstring, wstring> &attributes)
{
	int n = (int) arrayData->PhysicalArraysAttributes().size();
	ParameterNameValuePairVector &arrayAtts = arrayData->PhysicalArraysAttributes()[n-1].Attributes();
	n = (int) arrayAtts.size();
	arrayAtts.resize(n+1);
	arrayAtts[n].Name = attributes[PHYSICAL_ARRAY_ATTRIBUTE_ELEMENT_NAME_ATTRIBUTE];
	arrayAtts[n].Value = attributes[PHYSICAL_ARRAY_ATTRIBUTE_ELEMENT_VALUE_ATTRIBUTE];
}

/*
 * Create a new entry in the physical array and store the ID.
 */
void SAXArrayHandlers::StorePhysicalArrayID(map<wstring, wstring> &attributes)
{
	int n = (int) arrayData->PhysicalArraysAttributes().size();
	arrayData->PhysicalArraysAttributes().resize(n+1);
	arrayData->PhysicalArraysAttributes()[n].Identifier() =
		StringUtils::ConvertWCSToMBS(attributes[PHYSICAL_ARRAY_ELEMENT_ID_ATTRIBUTE]);
}

/*
 * Store the attributes to the array data and member variables.
 */
void SAXArrayHandlers::StoreArrayFileAttributes(map<wstring, wstring> &attributes)
{
	wstring str;
	arrayData->ArraySetIdentifier() = StringUtils::ConvertWCSToMBS(attributes[ARRAY_FILE_ELEMENT_ID_ATTRIBUTE]);
	str = attributes[ARRAY_FILE_ELEMENT_VERSION_ATTRIBUTE];
	if (str.length() > 0)
	{
		fileVersionNumber = WideToInt(str);
	}
	str = attributes[ARRAY_FILE_ELEMENT_TYPE_ATTRIBUTE];
	if (str.length() > 0)
	{
		dataTypeIdentifier = StringUtils::ConvertWCSToMBS(str);
	}
}

