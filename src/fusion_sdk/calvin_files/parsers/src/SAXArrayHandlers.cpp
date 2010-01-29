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


#include "calvin_files/parsers/src/SAXArrayHandlers.h"
//
#include "calvin_files/utils/src/StringUtils.h"
//
#include <cstring>
#include <map>
#include <string>
//

using namespace affymetrix_calvin_array;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;
using namespace std;
XERCES_CPP_NAMESPACE_USE;

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif

/*! Converts a string to an integer.
 * @param wstr The string to convert
 * @return The integer representation of the string.
 */
static int WideToInt(wstring const &wstr)
{
	if (wstr.length() == 0)
		return 0;
	int val = 0;
	swscanf(wstr.c_str(), L"%d", &val);
	return val;
}

/*! Converts a string to an integer.
 * @param wstr The string to convert
 * @return The float representation of the string.
 */
static float WideToFloat(wstring const &wstr)
{
	if (wstr.length() == 0)
		return 0.0f;
	float val=0.0f;
	swscanf(wstr.c_str(), L"%f", &val);
	return val;
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
	readHeaderOnly(headerOnly),
	currentElement(ARRAY_FILE)
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

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_VALUE_ELEMENT && currentElement == USER_ATTRIBUTES_ATTRIBUTE_VALUE)
		currentElement = USER_ATTRIBUTES_ATTRIBUTE;

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_ELEMENT && currentElement == USER_ATTRIBUTES_ATTRIBUTE_VALUE)
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

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_VALUE_ELEMENT && currentElement == USER_ATTRIBUTES_ATTRIBUTE)
		currentElement = USER_ATTRIBUTES_ATTRIBUTE_VALUE;

	else if (name == USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT && currentElement == USER_ATTRIBUTES_ATTRIBUTE)
		currentElement = USER_ATTRIBUTES_ATTRIBUTE_CONTROL;

	else if (name == ARRAY_FILE_ELEMENT ||
		name == PHYSICAL_ARRAYS_ELEMENT ||
		name == PHYSICAL_ARRAY_ELEMENT ||
		name == PHYSICAL_ARRAY_ATTRIBUTE_ELEMENT ||
		name == USER_ATTRIBUTES_ELEMENT ||
		name == USER_ATTRIBUTES_ATTRIBUTE_ELEMENT ||
		name == USER_ATTRIBUTES_ATTRIBUTE_VALUE_ELEMENT ||
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
		StorePhysicalArrayElementAttributes(atts);
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
 * Based on the current element, assign the value.
 */
void SAXArrayHandlers::characters(const XMLCh *const chars, const XMLSize_t length)
{
	wstring str = XMLChToWString(chars);
	if (currentElement == PHYSICAL_ARRAY_ATTRIBUTES)
	{
		int n = (int) arrayData->PhysicalArraysAttributes().size();
		ParameterNameValuePairVector &arrayAtts = arrayData->PhysicalArraysAttributes()[n-1].Attributes();
		n = (int) arrayAtts.size();
		arrayAtts[n-1].Value = str;
	}
	else if (currentElement == USER_ATTRIBUTES_ATTRIBUTE_VALUE)
	{
		ParameterNameValueDefaultRequiredType *param = &(*arrayData->UserAttributes().rbegin());
		switch (param->ValueType())
		{
		case ParameterNameValueDefaultRequiredType::IntegerParameterType:
			param->SetValueInt32(WideToInt(str));
			break;

		case ParameterNameValueDefaultRequiredType::FloatParameterType:
			param->SetValueFloat(WideToFloat(str));
			break;

		case ParameterNameValueDefaultRequiredType::TextParameterType:
		case ParameterNameValueDefaultRequiredType::DateParameterType:
		case ParameterNameValueDefaultRequiredType::TimeParameterType:
		case ParameterNameValueDefaultRequiredType::DateTimeParameterType:
		case ParameterNameValueDefaultRequiredType::ControlSingleParameterType:
			param->SetValueText(str);
			break;

		case ParameterNameValueDefaultRequiredType::ControlMultiParameterType:
			param->ControlMultiValues().push_back(str);
			break;

		default:
			break;
		}
	}
}

/*
 * Store the user attribute.
 */
void SAXArrayHandlers::StoreUserAttribute(map<wstring, wstring> &attributes)
{
	ParameterNameValueDefaultRequiredType param;
	param.SetName(attributes[USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_NAME_ATTRIBUTE]);
	wstring value = attributes[USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_TYPE_ATTRIBUTE];
	wstring defvalue = attributes[USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_DEFAULT_ATTRIBUTE];

	if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::IntegerParameterType))
	{
		param.SetValueInt32(0);
		param.ValueType() = ParameterNameValueDefaultRequiredType::IntegerParameterType;
		if (defvalue.length() > 0)
			param.SetDefaultValueInt32(WideToInt(defvalue));
	}
	else if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::FloatParameterType))
	{
		param.SetValueFloat(0.0f);
		param.ValueType() = ParameterNameValueDefaultRequiredType::FloatParameterType;
		if (defvalue.length() > 0)
			param.SetDefaultValueFloat(WideToFloat(defvalue));
	}
	else
	{
		if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::TextParameterType))
		{
			param.ValueType() = ParameterNameValueDefaultRequiredType::TextParameterType;
		}
		else if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::DateParameterType))
		{
			param.ValueType() = ParameterNameValueDefaultRequiredType::DateParameterType;
		}
		else if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::TimeParameterType))
		{
			param.ValueType() = ParameterNameValueDefaultRequiredType::TimeParameterType;
		}
		else if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::DateTimeParameterType))
		{
			param.ValueType() = ParameterNameValueDefaultRequiredType::DateTimeParameterType;
		}
		else if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::ControlSingleParameterType))
		{
			param.ValueType() = ParameterNameValueDefaultRequiredType::ControlSingleParameterType;
		}
		else if (value == ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterNameValueDefaultRequiredType::ControlMultiParameterType))
		{
			param.ValueType() = ParameterNameValueDefaultRequiredType::ControlMultiParameterType;
		}
		param.SetValueText(L"");
		if (defvalue.length() > 0)
			param.SetDefaultValueText(defvalue);
	}
	value = attributes[USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_REQUIRED_ATTRIBUTE];
	if (value == L"true")
		param.RequiredFlag() = true;
	else
		param.RequiredFlag() = false;
	arrayData->UserAttributes().push_back(param);
}

/*
 * Store the control value for a user attribute.
 */
void SAXArrayHandlers::StoreUserAttributeControl(map<wstring, wstring> &attributes)
{
	ParameterNameValueDefaultRequiredType *param = &(*arrayData->UserAttributes().rbegin());
	param->ControlledVocabulary().push_back(attributes[USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT_VALUE_ATTRIBUTE]);
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
void SAXArrayHandlers::StorePhysicalArrayElementAttributes(map<wstring, wstring> &attributes)
{
	int n = (int) arrayData->PhysicalArraysAttributes().size();
	arrayData->PhysicalArraysAttributes().resize(n+1);
	arrayData->PhysicalArraysAttributes()[n].Identifier() =
		StringUtils::ConvertWCSToMBS(attributes[PHYSICAL_ARRAY_ELEMENT_ID_ATTRIBUTE]);

	wstring str;
	str = attributes[PHYSICAL_ARRAY_ELEMENT_ROW_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].MediaRow() = WideToInt(str);
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_COL_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].MediaCol() = WideToInt(str);
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_MEDIA_FILE_NAME_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].MediaFileName() = StringUtils::ConvertWCSToMBS(str);
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_MEDIA_FILE_GUID_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].MediaFileGUID() = StringUtils::ConvertWCSToMBS(str);
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_LIB_PACKAGE_NAME_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].LibraryPackageName() = StringUtils::ConvertWCSToMBS(str);
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_NAME_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].ArrayName() = StringUtils::ConvertWCSToMBS(str);
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_BARCODE_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].ArrayBarcode() = StringUtils::ConvertWCSToMBS(str);
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_TYPE_ATTRIBUTE];
	arrayData->PhysicalArraysAttributes()[n].Media() = MediaFromString(str);

	str = attributes[PHYSICAL_ARRAY_ELEMENT_MASTERFILE_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].MasterFile() = StringUtils::ConvertWCSToMBS(str);;
	}

	str = attributes[PHYSICAL_ARRAY_ELEMENT_PAT_ASSIGNMENT_ATTRIBUTE];
	arrayData->PhysicalArraysAttributes()[n].PatAssignment() = PATAssignmentMethodFromString(str);

	str = attributes[PHYSICAL_ARRAY_ELEMENT_MASTERFILE_GUID_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->PhysicalArraysAttributes()[n].MasterFileId() = StringUtils::ConvertWCSToMBS(str);
	}
	arrayData->PhysicalArraysAttributes()[n].CreationDateTime() = attributes[PHYSICAL_ARRAY_ELEMENT_CREATION_DATE_ATTRIBUTE];
	arrayData->PhysicalArraysAttributes()[n].CreatedBy() = attributes[PHYSICAL_ARRAY_ELEMENT_CREATED_BY_ATTRIBUTE];
	arrayData->PhysicalArraysAttributes()[n].Comment() = attributes[PHYSICAL_ARRAY_ELEMENT_COMMENT_ATTRIBUTE];

	str = attributes[ARRAY_FILE_ELEMENT_CREATED_STEP_ATTRIBUTE];
	arrayData->PhysicalArraysAttributes()[n].CreatedStep() = CreateStepFromString(str);
}

/*
 * Store the attributes to the array data and member variables.
 */
void SAXArrayHandlers::StoreArrayFileAttributes(map<wstring, wstring> &attributes)
{

	wstring str;
	str = attributes[ARRAY_FILE_ELEMENT_TYPE_ATTRIBUTE];
	if (str.length() > 0)
	{
		arrayData->DataTypeIdentifier() = StringUtils::ConvertWCSToMBS(str);
	}

	fileVersionNumber = attributes[ARRAY_FILE_ELEMENT_VERSION_ATTRIBUTE];
	arrayData->ArraySetFileIdentifier() = StringUtils::ConvertWCSToMBS(attributes[ARRAY_FILE_ELEMENT_ID_ATTRIBUTE]);
	arrayData->InitialProject() = attributes[ARRAY_FILE_ELEMENT_PROJECT_ATTRIBUTE];
	arrayData->CreationDateTime() = attributes[ARRAY_FILE_ELEMENT_CREATE_DATE_TIME_ATTRIBUTE];
	arrayData->CreatedBy() = attributes[ARRAY_FILE_ELEMENT_CREATED_BY_ATTRIBUTE];

	str = attributes[ARRAY_FILE_ELEMENT_CREATED_STEP_ATTRIBUTE];
	arrayData->CreatedStep() = CreateStepFromString(str);
}

