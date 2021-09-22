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


#include "calvin_files/writers/src/TemplateFileWriter.h"
//
#include "calvin_files/parsers/src/SAXTemplateHandlers.h"
#include "calvin_files/template/src/TemplateData.h"
#include "calvin_files/template/src/TemplateId.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
//
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
//
//#include <xercesc/dom/DOMWriter.hpp>

using namespace affymetrix_calvin_template;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;
XERCES_CPP_NAMESPACE_USE

#ifdef _MSC_VER
#pragma warning(disable: 4996) // ignore deprecated functions warning
#endif


/*! The expected version number */
#define TEMPLATE_SET_FILE_VERSION_NUMBER "1.0"

/*! This class provides utilities for converting native strings to XML strings.
 * This class is provided for platforms where the XMLCh is not a wchar_t (Mac OSX)
 */
class XMLChConversion
{
private:
	/*! The XML string */
	XMLCh *str;

	/*! converts an int.
	 * @param i The int.
	 */
	void convert(int i)
	{
		char cstr[64];
		sprintf(cstr, "%d", i);
		convert(cstr);
	}

	/*! Converts a 8 bit string.
	 * @param s The 8 bit string.
	 */
	void convert(const char *const s)
	{
		clear();
		int n=(int)strlen(s);
		str = new XMLCh[n+1];
		for (int i=0; i<n; i++)
			str[i] = (XMLCh)s[i];
		str[n]=0;
	}

	/*! Converts a 16 bit string.
	 * @param s The 16 bit string.
	 */
	void convert(const wchar_t *const s)
	{
		clear();
		int n=(int)wcslen(s);
		str = new XMLCh[n+1];
		for (int i=0; i<n; i++)
			str[i] = (XMLCh)s[i];
		str[n]=0;
	}

	/*! Clears the string */
	void clear()
	{
		if (str)
			delete[] str;
		str = NULL;
	}
public:
	/*! Constructor with integer.
	 * @param i The integer.
	 */
	XMLChConversion(int i) { str=NULL; convert(i); }
	/*! Constructor with 16 bit string.
	 * @param s The 16 bit string.
	 */
	XMLChConversion(const wchar_t *const s) { str=NULL; convert(s); }
	/*! Constructor with wstring.
	 * @param s The wstring.
	 */
	XMLChConversion(std::wstring s) { str=NULL; convert( s.c_str() ); }
	/*! Constructor with 8 bit string.
	 * @param s The 8 bit string.
	 */
	XMLChConversion(const char *const s) { str=NULL; convert(s); }
	/*! Constructor with string.
	 * @param s The string.
	 */
	XMLChConversion(std::string s) { str=NULL; convert(s.c_str()); }
	/*! Destructor */
	~XMLChConversion() { clear(); }
	/*! Returns the XML string.
	 * @return The XML string.
	 */
	const XMLCh *String() const { return str; }
};

/*! A macro to help with converting a string to XML string */
#define ToXMLCh(s) ( XMLChConversion(s).String() )


/*
 * Create the template element. This is the top level parent element.
 */
DOMElement* CreateTemplateElement(TemplateData &templateData, DOMDocument* doc, const AffymetrixGuidType &dataTypeIdentifier)
{
	// ArrayFile element
	DOMElement* arrayElement = doc->createElement(ToXMLCh(TEMPLATE_FILE_ELEMENT));
	arrayElement->setAttribute(ToXMLCh(TEMPLATE_FILE_ELEMENT_TYPE_ATTRIBUTE), ToXMLCh(TEMPLATE_FILE_TYPE_IDENTIFIER));
	arrayElement->setAttribute(ToXMLCh(TEMPLATE_FILE_ELEMENT_VERSION_ATTRIBUTE), ToXMLCh(TEMPLATE_SET_FILE_VERSION_NUMBER));
	arrayElement->setAttribute(ToXMLCh(TEMPLATE_FILE_ELEMENT_ID_ATTRIBUTE), ToXMLCh(templateData.TemplateFileIdentifier()));
	arrayElement->setAttribute(ToXMLCh(TEMPLATE_FILE_ELEMENT_CREATE_DATE_TIME_ATTRIBUTE), ToXMLCh(templateData.CreationDateTime()));
	arrayElement->setAttribute(ToXMLCh(TEMPLATE_FILE_ELEMENT_CREATED_BY_ATTRIBUTE), ToXMLCh(templateData.CreatedBy()));
	return arrayElement;
}

/*
 * Add the user attributes to the document if they exist.
 */
void AddUserAttributes(TemplateData &templateData, DOMDocument* doc, DOMElement* arrayElement)
{
	int nUserAttributes = (int) templateData.UserAttributes().size();
	if (nUserAttributes > 0)
	{
		DOMElement* userAttributesElement = doc->createElement(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ELEMENT));

		affymetrix_calvin_parameter::ParameterNameValueDefaultRequiredTypeList::iterator paramIt;
		for (paramIt=templateData.UserAttributes().begin(); paramIt!=templateData.UserAttributes().end(); ++ paramIt)
		{
			ParameterNameValueDefaultRequiredType &param = *paramIt;
			DOMElement* paramElement = doc->createElement(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT));
			paramElement->setAttribute(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_NAME_ATTRIBUTE), ToXMLCh(param.GetName()));
			paramElement->setAttribute(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_TYPE_ATTRIBUTE), ToXMLCh(
				ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(param.ValueType())));
			if (param.RequiredFlag() == true)
				paramElement->setAttribute(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_REQUIRED_ATTRIBUTE), ToXMLCh("Yes"));
			if (param.HasDefault() == true)
				paramElement->setAttribute(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_ELEMENT_DEFAULT_ATTRIBUTE), ToXMLCh(param.DefaultToString()));

			if (param.ValueType() == ParameterNameValueDefaultRequiredType::ControlMultiParameterType)
			{
				std::list<std::wstring>::iterator valIt;
				for (valIt = param.ControlMultiValues().begin(); valIt != param.ControlMultiValues().end(); ++valIt)
				{
					DOMElement *valueElement = doc->createElement(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_VALUE_ELEMENT));
					valueElement->setTextContent(ToXMLCh(*valIt));
					paramElement->appendChild(valueElement);
				}
			}
			else
			{
				DOMElement *valueElement = doc->createElement(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_VALUE_ELEMENT));
				valueElement->setTextContent(ToXMLCh(param.DefaultToString()));
				paramElement->appendChild(valueElement);
			}

			int nControl = (int) param.ControlledVocabulary().size();
			if (nControl > 0)
			{
				std::list<std::wstring>::iterator controlIt;
				for (controlIt=param.ControlledVocabulary().begin(); controlIt!=param.ControlledVocabulary().end(); ++controlIt)
				{
					DOMElement* controlElement = doc->createElement(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT));
					controlElement->setAttribute(ToXMLCh(TEMPLATE_USER_ATTRIBUTES_ATTRIBUTE_CONTROL_ELEMENT_VALUE_ATTRIBUTE), ToXMLCh(*controlIt));
					paramElement->appendChild(controlElement);
				}
			}
			userAttributesElement->appendChild(paramElement);
		}
		arrayElement->appendChild(userAttributesElement);
	}
}

/*
 * Initialize the class.
 */
TemplateFileWriter::TemplateFileWriter()
{
	dataTypeIdentifier = TEMPLATE_FILE_TYPE_IDENTIFIER;
}

/*
 * Clear the data.
 */
TemplateFileWriter::~TemplateFileWriter()
{
}

/*
 * Write the entire file, the header and body.
 */
bool TemplateFileWriter::Write(const std::string &fileName, affymetrix_calvin_template::TemplateData &templateData)
{
	// Initialize the XML4C2 system.
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException&)
	{
		return false;
	}

	// Create a DOM implementation object and create the document type for it.
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(ToXMLCh(L"LS"));
	//DOMDocumentType* dt  = impl->createDocumentType(ToXMLCh(TEMPLATE_FILE_ELEMENT), 0, ToXMLCh(TEMPLATE_FILE_DTD));
	DOMDocument* doc = impl->createDocument();
	//doc->setStandalone(true);
	//doc->appendChild(dt);

	// Create the serializer.
	DOMLSSerializer *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
	DOMLSOutput *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
	//theSerializer->setEncoding(ToXMLCh(TEMPLATE_FILE_ENCODING));
	theOutputDesc->setEncoding(ToXMLCh(TEMPLATE_FILE_ENCODING));

	// TemplateFile element
	DOMElement* templateElement = CreateTemplateElement(templateData, doc, dataTypeIdentifier);

	// UserAttributes element
	AddUserAttributes(templateData, doc, templateElement);

	// Add the template element to the document.
	doc->appendChild(templateElement);

	// Write the file.
	bool status = false;
	XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(fileName.c_str());
	theOutputDesc->setByteStream(myFormTarget);
	try
	{
		theSerializer->write(doc, theOutputDesc);
		status = true;
	}
	catch (...)
	{
		status = false;
	}

	// Clean up
	doc->release();
	theOutputDesc->release();
	theSerializer->release();
	delete myFormTarget;

	XMLPlatformUtils::Terminate();

	return status;
}
