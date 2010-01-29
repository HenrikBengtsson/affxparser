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


#ifndef _SAXParameterFileHandlers_HEADER_
#define _SAXParameterFileHandlers_HEADER_

/*! \file SAXParameterFileHandlers.h This file provides SAX parsing handles for parameter files.
 */

#include "calvin_files/parameter/src/ParameterFileData.h"
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
/*! The name of the element that contains the parameter file attributes. */
#define PARAMETER_FILE_ELEMENT std::wstring(L"ParameterFile")

/*! The attribute name of the company name field. */
#define PARAMETER_FILE_COMPANY_ATTRIBUTE std::wstring(L"company")

/*! The attribute name of the user name field. */
#define PARAMETER_FILE_USER_NAME_ATTRIBUTE std::wstring(L"userName")

/*! The attribute name of the content version field. */
#define PARAMETER_FILE_CONTENT_VERSION_ATTRIBUTE std::wstring(L"contentVersion")

/*! The element that contains the implementation attributes. */
#define IMPLEMENTATION_ELEMENT std::wstring(L"Implementation")

/*! The name attribute. */
#define IMPLEMENTATION_NAME_ATTRIBUTE std::wstring(L"name")

/*! The version attribute. */
#define IMPLEMENTATION_VERSION_ATTRIBUTE std::wstring(L"version")

/*! The executableFileName attribute. */
#define IMPLEMENTATION_EXE_FILE_NAME_ATTRIBUTE std::wstring(L"executableFileName")

/*! The description attribute. */
#define IMPLEMENTATION_DESC_ATTRIBUTE std::wstring(L"description")

/*! The ParameterSet element name. */
#define PARAMETER_SET_ELEMENT std::wstring(L"ParameterSet")

/*! The Parameter element name. */
#define PARAMETER_ELEMENT std::wstring(L"Parameter")

/*! The name attribute. */
#define PARAMETER_NAME_ATTRIBUTE std::wstring(L"name")

/*! The index attribute. */
#define PARAMETER_INDEX_ATTRIBUTE std::wstring(L"index")

/*! The display name attribute. */
#define PARAMETER_DISPLAY_NAME_ATTRIBUTE std::wstring(L"displayName")

/*! The category attribute. */
#define PARAMETER_CATEGORY_ATTRIBUTE std::wstring(L"category")

/*! The isEditable attribute. */
#define PARAMETER_ISEDITABLE_ATTRIBUTE std::wstring(L"isEditable")

/*! The type attribute. */
#define PARAMETER_TYPE_ATTRIBUTE std::wstring(L"type")

/*! String8 type. */
#define PARAMETER_STRING8_TYPE std::wstring(L"String8")

/*! String16 type. */
#define PARAMETER_STRING16_TYPE std::wstring(L"String16")

/*! Int8 type. */
#define PARAMETER_INT8_TYPE std::wstring(L"Int8")
    
/*! UInt8 type. */
#define PARAMETER_UINT8_TYPE std::wstring(L"UInt8")
    
/*! Int16 type. */
#define PARAMETER_INT16_TYPE std::wstring(L"Int16")
    
/*! UInt16 type. */
#define PARAMETER_UINT16_TYPE std::wstring(L"UInt16")
    
/*! Int32 type. */
#define PARAMETER_INT32_TYPE std::wstring(L"Int32")
    
/*! UInt32 type. */
#define PARAMETER_UINT32_TYPE std::wstring(L"UInt32")
    
/*! Float type. */
#define PARAMETER_FLOAT_TYPE std::wstring(L"Float")

/*! SingleControl type. */
#define PARAMETER_SINGLE_CONTROL_TYPE std::wstring(L"SingleControl")
    
/*! Boolean type. */
#define PARAMETER_BOOLEAN_TYPE std::wstring(L"Boolean")
    
/*! Date type. */
#define PARAMETER_DATE_TYPE std::wstring(L"Date")
    
/*! Time type. */
#define PARAMETER_TIME_TYPE std::wstring(L"Time")
    
/*! DateTime type. */
#define PARAMETER_DATETIME_TYPE std::wstring(L"DateTime")
    
/*! ExternalFile type. */
#define PARAMETER_EXT_FILE_TYPE std::wstring(L"ExternalFile")
    
/*! Unknown type. */
#define PARAMETER_UNKNOWN_TYPE std::wstring(L"Unknown")

/*! The currentValue attribute. */
#define PARAMETER_CURVALUE_ATTRIBUTE std::wstring(L"currentValue")

/*! The minValue attribute. */
#define PARAMETER_MINVALUE_ATTRIBUTE std::wstring(L"minValue")

/*! The maxValue attribute. */
#define PARAMETER_MAXVALUE_ATTRIBUTE std::wstring(L"maxValue")

/*! The defaultValue attribute. */
#define PARAMETER_DEFVALUE_ATTRIBUTE std::wstring(L"defaultValue")

/*! The precision attribute. */
#define PARAMETER_PRECISION_ATTRIBUTE std::wstring(L"precision")

/*! The maxLength attribute. */
#define PARAMETER_MAXLENGTH_ATTRIBUTE std::wstring(L"maxLength")

/*! The description attribute. */
#define PARAMETER_DESC_ATTRIBUTE std::wstring(L"description")

/*! The MetaData element. */
#define META_DATA_ELEMENT std::wstring(L"MetaData")

/*! The lastChangedBy attribute. */
#define META_DATA_LAST_CHANGED_BY_ATTRIBUTE std::wstring(L"lastChangedBy")

/*! The lastChangedOn attribute */
#define META_DATA_LAST_CHANGED_ON_ATTRIBUTE std::wstring(L"lastChangedOn")

/*! The isReadOnly attribute. */
#define META_DATA_IS_READONLY_ATTRIBUTE std::wstring(L"isReadOnly")

/*! The Control element. */
#define CONTROL_ELEMENT std::wstring(L"Control")

/*! Enumerants to hold the elements in an parameter file. */
typedef enum {
	PARAMETER_FILE,
	IMPLEMENTATION,
	PARAMETER_SET,
    PARAMETER_SETS,
	PARAMETERS,
    PARAMETER,
    META_DATA,
    CONTROL
} ParameterFileElements;

/*! This class provides the SAX handlers for reading calvin parameter files. */
class SAXParameterFileHandlers : public XERCES_CPP_NAMESPACE::HandlerBase
{
private:
	/*! A pointer to the parameter file data object. */
    affymetrix_calvin_parameter::ParameterFileData *parameterFileData;

	/*! The parent element that is currently being processed. */
	ParameterFileElements currentElement;

	/*! Stores the attributes from the parameter file element.
	 *
	 * @param attributes The name/value attributes.
	 */
	void StoreParameterFileAttributes(std::map<std::wstring, std::wstring> &attributes);

	/*! Stores the attribites from the implementation element.
	 *
	 * @param attributes The name/value attributes.
	 */
	void StoreImplementationAttributes(std::map<std::wstring, std::wstring> &attributes);

	/*! Stores the attributes from the parameter attribute element.
	 *
	 * @param attributes The name/value attributes.
	 */
	void StoreParameterAttributes(std::map<std::wstring, std::wstring> &attributes);

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
	 * @param data The parameter file data.
	 */
    SAXParameterFileHandlers(affymetrix_calvin_parameter::ParameterFileData *data);

	/*! Destructor */
	~SAXParameterFileHandlers();

	/*! Called at the start of the document */
	void startDocument();

	/*! Called at the end of the document */
	void endDocument();

	/*! Receive notification of character data inside an element.
	 * @param chars The character data.
	 * @param length The length of the character string.
	 */
	void characters(const XMLCh *const chars, const unsigned int length);

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

};

};

#endif // _SAXParameterFileHandlers_HEADER_
