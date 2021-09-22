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


#ifndef _SAXTemplateHandlers_HEADER_
#define _SAXTemplateHandlers_HEADER_

/*! \file SAXTemplateHandlers.h This file provides SAX parsing handles for template files.
 */

#include "calvin_files/template/src/TemplateData.h"
//
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
//
#include <cstring>
#include <map>
#include <string>
//

namespace affymetrix_calvin_io
{
/*! The name of the DTD for the template file. */
#define TEMPLATE_FILE_DTD std::wstring(L"ArraySetAndTemplateFile.dtd")

/*! The encoding to use for template files. */
#define TEMPLATE_FILE_ENCODING std::wstring(L"UTF-16")

/*! The name of the element that contains the template file id, type and version data. */
#define TEMPLATE_FILE_ELEMENT std::wstring(L"TemplateFile")

/*! The attribute name of the ID field for the template file. */
#define TEMPLATE_FILE_ELEMENT_ID_ATTRIBUTE std::wstring(L"GUID")

/*! The attribute name of the type field for the template file. */
#define TEMPLATE_FILE_ELEMENT_TYPE_ATTRIBUTE std::wstring(L"Type")

/*! The attribute name of the version field for the template file. */
#define TEMPLATE_FILE_ELEMENT_VERSION_ATTRIBUTE std::wstring(L"Version")

/*! The attribute name of the creation date field for the template file. */
#define TEMPLATE_FILE_ELEMENT_CREATE_DATE_TIME_ATTRIBUTE std::wstring(L"CreatedDateTime")

/*! The attribute name of the create by field for the template file. */
#define TEMPLATE_FILE_ELEMENT_CREATED_BY_ATTRIBUTE std::wstring(L"CreatedBy")

/*! The name of the element that contains the user attributes. */
#define TEMPLATE_USER_ATTRIBUTES_ELEMENT std::wstring(L"UserAttributes")

/*! The name of the element that contains a single user attribute. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT std::wstring(L"UserAttribute")

/*! The name of the element that contains a single user attribute. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_VALUE_ELEMENT std::wstring(L"UserAttributeValue")

/*! The attribute name of the name field for a user attribute. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_NAME_ATTRIBUTE std::wstring(L"Name")

/*! The attribute name of the type field for a user attribute. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_TYPE_ATTRIBUTE std::wstring(L"Type")

/*! The attribute name of the default value field for a user attribute. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_DEFAULT_ATTRIBUTE std::wstring(L"DefaultValue")

/*! The attribute name of the required field for a user attribute. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_REQUIRED_ATTRIBUTE std::wstring(L"Required")

/*! The attribute name of the value field for a user attribute. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_VALUE_ATTRIBUTE std::wstring(L"Value")

/*! The name of the element that contains a controlled vocabulary. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT std::wstring(L"Control")

/*! The attribute name of the value field for a controlled vocabulary. */
#define TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT_VALUE_ATTRIBUTE std::wstring(L"Value")


/*! Enumerants to hold the elements in an template file. */
typedef enum {
	TEMPLATE_FILE,
	TEMPLATE_USER_ATTRIBUTES,
	TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE,
	TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_VALUE,
	TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_CONTROL
} TemplateFileElements;

/*! An exception for stoping parsing of a file. */
class SAXTemplateStopParsingException : public XERCES_CPP_NAMESPACE::SAXException
{
};

/*! This class provides the SAX handlers for reading calvin template files. */
class SAXTemplateHandlers : public XERCES_CPP_NAMESPACE::HandlerBase
{
private:
	/*! A pointer to the template object. */
	affymetrix_calvin_template::TemplateData *templateData;

	/*! A flag used to indicate that the header line should only be read. */
	bool readHeaderOnly;

	/*! The parent element that is currently being processed. */
	TemplateFileElements currentElement;

	/*! The files version number. */
	std::wstring fileVersionNumber;

	/*! An identifier to the type of data stored in the file */
	affymetrix_calvin_utilities::AffymetrixGuidType dataTypeIdentifier;

	/*! Stores the attributes from the template file element.
	 *
	 * @param attributes The name/value attributes.
	 */
	void StoreTemplateFileAttributes(std::map<std::wstring, std::wstring> &attributes);

	/*! Stores an attribute from the user attribute element.
	 *
	 * @param attributes The name/value attributes.
	 */
	void StoreUserAttribute(std::map<std::wstring, std::wstring> &attributes);

	/*! Stores a control value from the user attribute element.
	 *
	 * @param attributes The name/value attributes.
	 */
	void StoreUserAttributeControl(std::map<std::wstring, std::wstring> &attributes);

	/*! Modifies the state machine given the end of an element.
	 *
	 * @param name The name of the element.
	 */
	void MoveCurrentElementBack(const XMLCh* const name);

	/*! Modifies the state machine given the start of an element.
	 *
	 * @param name The name of the element.
	 * @return True is a valid element was found.
	 */
	bool MoveCurrentElementForward(const XMLCh* const name);

public:
	/*! Constructor
	 *
	 * @param data The template data.
	 * @param headerOnly Flag to indicate that the header line should only be read.
	 */
	SAXTemplateHandlers(affymetrix_calvin_template::TemplateData *data, bool headerOnly=false);

	/*! Destructor */
	~SAXTemplateHandlers();

	/*! Called at the start of the document */
	void startDocument();

	/*! Called at the end of the document */
	void endDocument();

	/*! Receive notification of character data inside an element.
	 * @param chars The character data.
	 * @param length The length of the character string.
	 */
	void characters(const XMLCh *const chars, const XMLSize_t length);

	/*! Called at the start of each element.
	 *
	 * @param name The name of the element.
	 * @param attributes The attributes of the element.
	 */
	void startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes);

	/*! Called at the end of each element.
	 *
	 * @param name The name of the element.
	 */
	void endElement(const XMLCh* const name);

	/*! The identifier of the type of data stored in the file.
	 *
	 * @return The identifier of the type of data.
	 */
	const affymetrix_calvin_utilities::AffymetrixGuidType &DataTypeIdentifier() const { return dataTypeIdentifier; }

	/*! The files version number.
	 *
	 * @return The file version.
	 */
	std::wstring FileVersionNumber() const { return fileVersionNumber; }
};

};

#endif // _SAXTemplateHandlers_HEADER_
