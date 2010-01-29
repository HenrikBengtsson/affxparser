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

#ifndef _SAXDttArrayHandlers_HEADER_
#define _SAXDttArrayHandlers_HEADER_

/*! \file SAXDttArrayHandlers.h This file provides SAX parsing handles for array files.
 */

#include "file/DttArrayFileReader.h"
//
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
//
#include <cstring>
#include <map>
#include <string>
//

namespace affymetrix_dttarray
{

/*! The probe array type parameter name. */
#define PROBE_ARRAY_TYPE_PARAMETER_NAME std::string("Probe Array Type")

/*! The name of the biomaterial package element. */
#define BIOMATERIAL_PACKAGE_ELEMENT std::wstring(L"BioMaterial_package")

/*! The name of the biomaterial association list element. */
#define BIOMATERIAL_ASSNLIST_ELEMENT std::wstring(L"BioMaterial_assnlist")

/*! The name of the biosource list element. */
#define BIOSOURCE_ELEMENT std::wstring(L"BioSource")

/*! The name of the "name" attribute. */
#define NAME_ATTRIBUTE std::wstring(L"name")

/*! The name of the "value" attribute. */
#define VALUE_ATTRIBUTE std::wstring(L"value")

/*! The name of the "type" attribute. */
#define TYPE_ATTRIBUTE std::wstring(L"type")

/*! The value of the "type" attribute for sample info. */
#define STRING_TYPE std::string("string")

/*! The name of the property sets association list element. */
#define PROPERTYSETS_ASSNLIST_ELEMENT std::wstring(L"PropertySets_assnlist")

/*! The name of the name/value type element. */
#define NAME_VALUE_TYPE_ELEMENT std::wstring(L"NameValueType")

/*! The name of the characteristics association list element. */
#define CHARACTERISTICS_ASSNLIST_ELEMENT std::wstring(L"Characteristics_assnlist")

/*! The name of the material type association element. */
#define MATERIAL_TYPE_ASSN_ELEMENT std::wstring(L"MaterialType_assn")

/*! The name of the ontology entry element. */
#define ONTOLOGY_ENTRY_ELEMENT std::wstring(L"OntologyEntry")

/*! The name of the category attribute. */
#define ONTOLOGY_CATEGORY_ATTRIBUTE std::wstring(L"category")

/*! The name of the value attribute. */
#define ONTOLOGY_VALUE_ATTRIBUTE std::wstring(L"value")

/*! The experiment package element. */
#define EXPERIMENT_PACKAGE_ELEMENT std::wstring(L"Experiment_package")

/*! The experiment association list element. */
#define EXPERIMENT_ASSNLIST_ELEMENT std::wstring(L"Experiment_assnlist")

/*! The experiment element. */
#define EXPERIMENT_ELEMENT std::wstring(L"Experiment")

/*! The name of the array design package element. */
#define ARRAY_DESIGN_PACKAGE_ELEMENT std::wstring(L"ArrayDesign_package")

/*! The name of the array design association list. */
#define ARRAY_DESIGN_ASSNLIST_ELEMENT std::wstring(L"ArrayDesign_assnlist")

/*! The name of the physical array design. */
#define PHYSICAL_ARRAY_DESIGN_ELEMENT std::wstring(L"PhysicalArrayDesign")

/*! Enumerants to hold the elements in a dtt array file. */
typedef enum {
	DTT_ARRAY_FILE,
	BIOMATERIAL_PACKAGE,
	BIOMATERIAL_ASSNLIST,
	BIOSOURCE,
	BIOSOURCE_PROPERTYSETS_ASSNLIST,
	BIOSOURCE_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE,
	BIOSOURCE_CHARACTERISTICS_ASSNLIST,
	BIOSOURCE_CHARACTERISTICS_ASSNLIST_ONTOLOGY,
	BIOSOURCE_MATERIALTYPE_ASSNLIST,
	BIOSOURCE_MATERIALTYPE_ASSNLIST_ONTOLOGY,
	EXPERIMENT_PACKAGE,
	EXPERIMENT_ASSNLIST,
	EXPERIMENT,
	EXPERIMENT_PROPERTYSETS_ASSNLIST,
	EXPERIMENT_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE,
	ARRAY_DESIGN_PACKAGE,
	ARRAY_DESIGN_ASSNLIST,
	PHYSICAL_ARRAY_DESIGN

} DttArrayFileElements;

/*! This class provides the SAX handlers for reading calvin array files. */
class SAXArrayHandlers : public XERCES_CPP_NAMESPACE::HandlerBase
{
private:
	/*! Used to determine the number of levels past the expected name/value type element. */
	int levelsDeep;

	/*! A pointer to the array object. */
	DttArrayData *arrayData;

	/*! The parent element that is currently being processed. */
	DttArrayFileElements currentElement;

	/*! Stores the experiment name attribute from the experiment element.
	 * @param attributes The name/value attributes.
	 */
	void StoreExperimentNameAttribute(XERCES_CPP_NAMESPACE::AttributeList& attributes);

	/*! Stores the sample name attribute from the bio source element.
	 * @param attributes The name/value attributes.
	 */
	void StoreSampleNameAttribute(XERCES_CPP_NAMESPACE::AttributeList& attributes);

	/*! Stores the sample type attribute from the biosource/materialtype element.
	 * @param attributes The name/value attributes.
	 */
	void StoreSampleTypeAttribute(XERCES_CPP_NAMESPACE::AttributeList&  attributes);

	/*! Stores the sample project attribute from the biosource/characteristics element.
	 * @param attributes The name/value attributes.
	 */
	void StoreSampleProjectAttribute(XERCES_CPP_NAMESPACE::AttributeList&  attributes);

	/*! Stores the attributes from a name value type element.
	 * @param attributes The name/value attributes.
	 */
	void StoreNameValueAttributes(XERCES_CPP_NAMESPACE::AttributeList&  attributes);

	/*! Stores the array type from the physical array design element.
	 * @param attributes The name/value attributes.
	 */
	void StoreArrayType(XERCES_CPP_NAMESPACE::AttributeList&  attributes);

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
	 * @param data The array data.
	 */
	SAXArrayHandlers(DttArrayData *data);

	/*! Destructor */
	~SAXArrayHandlers();

	/*! Called at the start of the document */
	void startDocument();

	/*! Called at the end of the document */
	void endDocument();

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

#endif // _SAXDttArrayHandlers_HEADER_
