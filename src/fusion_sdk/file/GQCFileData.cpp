////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 Affymetrix, Inc.
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

//#include <xercesc/dom/DOMWriter.hpp>
#include "file/GQCFileData.h"
//
#include "calvin_files/utils/src/DateTime.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
//
#include <cstring>
#include <map>
#include <string.h>
#include <stdio.h>
//
#define XERCES_STATIC_LIBRARY

using namespace std;
using namespace affxgqc;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;
XERCES_CPP_NAMESPACE_USE;

#define GENERIC_REPORT_FILE_ENCODING "UTF-8"
#define GENERIC_REPORT_ELEMENT_NAME "GenericReport"
#define NAME_VALUE_PAIRS_ELEMENT_NAME "NameValuePairs"
#define NAME_VALUE_PAIR_ELEMENT_NAME "NameValuePair"
#define REPORT_RESULTS_ELEMENT_NAME "ReportResults"
#define ANALYSIS_PARAMETERS "Analysis Parameters"
#define QC_RESULTS "QC Results"
#define SAMPLE_SIGNATURE "Sample Signature"
#define NAME_ATTRIBUTE "name"
#define ROWS_ATTRIBUTE "rows"
#define COLS_ATTRIBUTE "cols"
#define VALUE_ATTRIBUTE "value"
#define TYPE_ATTRIBUTE "type"
#define TITLE_ATTRIBUTE "title"
#define DATE_ATTRIBUTE "dateCreated"
#define REPORT_TYPE "affymetrix-generic-report"
#define REPORT_TITLE "Intensity Report"

#ifdef _MSC_VER
#pragma warning(disable: 4996) // ignore deprecated functions warning
#endif

/*! Converts a XML character string to string.
 * @param c1 The XML string to convert
 * @return The character string
 */
static string XMLChToString(const XMLCh* const c1)
{
	string s;
	int i=0;
	while (c1[i] != 0)
	{
		s += (char)c1[i];
		++i;
	}
	return s;
}

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
/*! Comparison operator for XML strings to wide character strings.
 * @param c1 The XML string
 * @param c2 The wide string
 * @return True if equal
 */
static bool operator==(const XMLCh* const &c1, string c2)
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


/*! SAX handlers for the XML report file. */
class SAXGenericReportHandlers : public XERCES_CPP_NAMESPACE::HandlerBase
{
private:
    /*! The file data. */
    GQCFileData *fileData;

    /*! The current parameters being loaded. */
    ParameterNameValuePairList *currentParameters;

    /*! Enumerants to hold the elements in the file. */
    typedef enum {
	    GENERIC_REPORT,
	    NAME_VALUE_PAIRS,
	    NAME_VALUE_PAIR
    } GenericReportElements;

	/*! The parent element that is currently being processed. */
	GenericReportElements currentElement;

	/*! Modifies the state machine given the end of an element.
	 * @param name The name of the element.
	 */
	void MoveCurrentElementBack(const XMLCh* const name)
    {        
	    if (name == 0)
		    return;

	    if (name == string(NAME_VALUE_PAIR_ELEMENT_NAME))
		    currentElement = NAME_VALUE_PAIRS;

        else if (name == string(NAME_VALUE_PAIRS_ELEMENT_NAME))
            currentElement = GENERIC_REPORT;
    }

	/*! Modifies the state machine given the start of an element.
	 * @param name The name of the element.
	 * @return True is a valid element was found.
	 */
	bool MoveCurrentElementForward(const XMLCh* const name)
    {    
	    if (name == 0)
		    return false;

	    if (name == string(GENERIC_REPORT_ELEMENT_NAME))
		    currentElement = GENERIC_REPORT;

        else if (name == string(NAME_VALUE_PAIRS_ELEMENT_NAME))
            currentElement = NAME_VALUE_PAIRS;

        else if (name == string(NAME_VALUE_PAIR_ELEMENT_NAME))
            currentElement = NAME_VALUE_PAIR;

        else
            return false;

        return true;
    }

public:
	/*! Constructor
	 * @param data The file data.
     */
    SAXGenericReportHandlers(GQCFileData *data) : fileData(data), currentParameters(NULL), currentElement(GENERIC_REPORT) {}

	/*! Destructor */
    ~SAXGenericReportHandlers() {}

	/*! Called at the start of the document */
    void startDocument() {}

	/*! Called at the end of the document */
    void endDocument() {}

	/*! Receive notification of character data inside an element.
	 * @param chars The character data.
	 * @param length The length of the character string.
	 */
	void characters(const XMLCh *const chars, const XMLSize_t length) {}

	/*! Called at the start of each element.
	 * @param name The name of the element.
	 * @param attributes The attributes of the element.
	 */
	void startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes)
    {
	    if (MoveCurrentElementForward(name) == false)
		    return;

        // Get the attributes.
	    map<string, string> atts;
	    unsigned int len = attributes.getLength();
	    for (unsigned int index = 0; index < len; index++)
	    {
		    atts[XMLChToString(attributes.getName(index))] = XMLChToString(attributes.getValue(index));
	    }

        // Store the results
	    switch (currentElement)
	    {
            case GENERIC_REPORT:
                break;

            case NAME_VALUE_PAIRS:
                if (atts[NAME_ATTRIBUTE] == ANALYSIS_PARAMETERS)
                    currentParameters = &fileData->AnalysisParameters();
                else if (atts[NAME_ATTRIBUTE] == QC_RESULTS)
                    currentParameters = &fileData->QCResults();
                else if (atts[NAME_ATTRIBUTE] == SAMPLE_SIGNATURE)
                    currentParameters = &fileData->SampleSignature();
                else
                    currentParameters = NULL;
                break;

            case NAME_VALUE_PAIR:
                if (currentParameters != NULL)
                {
                    ParameterNameValuePair param;
                    param.Name = StringUtils::ConvertMBSToWCS(atts[NAME_ATTRIBUTE]);
                    param.Value = StringUtils::ConvertMBSToWCS(atts[VALUE_ATTRIBUTE]);
                    currentParameters->push_back(param);
                }
                break;

	        default:
		        break;
        }
    }

	/*! Called at the end of each element.
	 * @param name The name of the element.
	 */
    void endElement(const XMLCh* const name) { MoveCurrentElementBack(name); }

};

/* Construct. */
GQCFileData::GQCFileData()
{
}

/* Clear the data. */
GQCFileData::~GQCFileData()
{
    Clear();
}

/* Clear the data. */
void GQCFileData::Clear()
{
    qcResults.clear();
    sampleSignature.clear();
    analysisParameters.clear();
}

/* Read the file. */
bool GQCFileData::Read(const std::string &fileName)
{
    Clear();

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
	SAXGenericReportHandlers handler(this);
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

/* Create a root element */
static DOMElement *CreateGenericReportElement(DOMDocument* doc)
{
    DOMElement* element = doc->createElement(ToXMLCh(GENERIC_REPORT_ELEMENT_NAME));
    element->setAttribute(ToXMLCh(TYPE_ATTRIBUTE), ToXMLCh(REPORT_TYPE));
    element->setAttribute(ToXMLCh(TITLE_ATTRIBUTE), ToXMLCh(REPORT_TITLE));
    DateTime now = DateTime::GetCurrentDateTime();
    element->setAttribute(ToXMLCh(DATE_ATTRIBUTE), ToXMLCh(now.ToString()));
    return element;
}

/* Add the name/value pairs to the document. */
static void AddNameValuePairs(const char *title, ParameterNameValuePairList &params, DOMDocument* doc, DOMElement* parentElement)
{
    if (params.size() == 0)
        return;

    DOMElement* element = doc->createElement(ToXMLCh(NAME_VALUE_PAIRS_ELEMENT_NAME));
    element->setAttribute(ToXMLCh(NAME_ATTRIBUTE), ToXMLCh(title));
    for (ParameterNameValuePairList::iterator it=params.begin(); it!=params.end(); it++)
    {
        ParameterNameValuePair &pair = *it;
        DOMElement* pairElement = doc->createElement(ToXMLCh(NAME_VALUE_PAIR_ELEMENT_NAME));
        pairElement->setAttribute(ToXMLCh(NAME_ATTRIBUTE), ToXMLCh(pair.Name));
        pairElement->setAttribute(ToXMLCh(VALUE_ATTRIBUTE), ToXMLCh(pair.Value));
        element->appendChild(pairElement);
    }
    parentElement->appendChild(element);
}

/*
 * Add an empty table (required by the DTD).
 */
static void AddBlankReportTable(DOMDocument* doc, DOMElement* parentElement)
{
    DOMElement* element = doc->createElement(ToXMLCh(REPORT_RESULTS_ELEMENT_NAME));
    element->setAttribute(ToXMLCh(NAME_ATTRIBUTE), ToXMLCh("NA"));
    element->setAttribute(ToXMLCh(ROWS_ATTRIBUTE), ToXMLCh("0"));
    element->setAttribute(ToXMLCh(COLS_ATTRIBUTE), ToXMLCh("0"));
    parentElement->appendChild(element);
}

/* Write the file. */
bool GQCFileData::Write(const std::string &fileName)
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
	DOMDocument* doc = impl->createDocument();
	//doc->setStandalone(true);

	// Create the serializer.
	DOMLSSerializer *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
	DOMLSOutput     *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
	//theSerializer->setEncoding(ToXMLCh(GENERIC_REPORT_FILE_ENCODING));
	theOutputDesc->setEncoding(ToXMLCh(GENERIC_REPORT_FILE_ENCODING));

    // Create the root element
    DOMElement *rootElement = CreateGenericReportElement(doc);

	// store the parameters
	AddNameValuePairs(ANALYSIS_PARAMETERS, analysisParameters, doc, rootElement);
	AddNameValuePairs(QC_RESULTS, qcResults, doc, rootElement);
	AddNameValuePairs(SAMPLE_SIGNATURE, sampleSignature, doc, rootElement);

    // Add an empty table (required by the DTD)
    AddBlankReportTable(doc, rootElement);

    // Store the element to the document.
    doc->appendChild(rootElement);

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
