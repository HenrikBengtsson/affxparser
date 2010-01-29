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

#include "file/SAXDttArrayHandlers.h"
//
#include <cstring>
#include <map>
#include <string>
//

using namespace affymetrix_dttarray;
using namespace std;
XERCES_CPP_NAMESPACE_USE;


/*! Converts a XML character string to string.
 * @param c1 The XML string to convert
 * @return The converted string
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
SAXArrayHandlers::SAXArrayHandlers(DttArrayData *data) :
	levelsDeep(0),
	arrayData(data),
	currentElement(DTT_ARRAY_FILE)
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
	
	if ((currentElement == BIOSOURCE_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE ||
			  currentElement == EXPERIMENT_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE) && levelsDeep > 0)
	{
		--levelsDeep;
	}
	
	else if (name == BIOMATERIAL_PACKAGE_ELEMENT)
		currentElement = DTT_ARRAY_FILE;

	else if (name == BIOMATERIAL_ASSNLIST_ELEMENT)
		currentElement = BIOMATERIAL_PACKAGE;

	else if (name == BIOSOURCE_ELEMENT)
		currentElement = BIOMATERIAL_ASSNLIST;

	else if (name == PROPERTYSETS_ASSNLIST_ELEMENT && currentElement == BIOSOURCE_PROPERTYSETS_ASSNLIST)
		currentElement = BIOSOURCE;

	else if (name == NAME_VALUE_TYPE_ELEMENT && currentElement == BIOSOURCE_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE)
		currentElement = BIOSOURCE_PROPERTYSETS_ASSNLIST;

	else if (name == CHARACTERISTICS_ASSNLIST_ELEMENT && currentElement == BIOSOURCE_CHARACTERISTICS_ASSNLIST)
		currentElement = BIOSOURCE;

	else if (name == ONTOLOGY_ENTRY_ELEMENT && currentElement == BIOSOURCE_CHARACTERISTICS_ASSNLIST_ONTOLOGY)
		currentElement = BIOSOURCE_CHARACTERISTICS_ASSNLIST;

	else if (name == MATERIAL_TYPE_ASSN_ELEMENT && currentElement == BIOSOURCE_MATERIALTYPE_ASSNLIST)
		currentElement = BIOSOURCE;

	else if (name == ONTOLOGY_ENTRY_ELEMENT && currentElement == BIOSOURCE_MATERIALTYPE_ASSNLIST_ONTOLOGY)
		currentElement = BIOSOURCE_MATERIALTYPE_ASSNLIST;

	else if (name == EXPERIMENT_PACKAGE_ELEMENT)
		currentElement = DTT_ARRAY_FILE;

	else if (name == EXPERIMENT_ASSNLIST_ELEMENT)
		currentElement = EXPERIMENT_PACKAGE;

	else if (name == EXPERIMENT_ELEMENT)
		currentElement = EXPERIMENT_ASSNLIST;

	else if (name == PROPERTYSETS_ASSNLIST_ELEMENT && currentElement == EXPERIMENT_PROPERTYSETS_ASSNLIST)
		currentElement = EXPERIMENT;

	else if (name == NAME_VALUE_TYPE_ELEMENT && currentElement == EXPERIMENT_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE)
		currentElement = EXPERIMENT_PROPERTYSETS_ASSNLIST;

	else if (name == ARRAY_DESIGN_PACKAGE_ELEMENT)
		currentElement = DTT_ARRAY_FILE;

	else if (name == ARRAY_DESIGN_ASSNLIST_ELEMENT)
		currentElement = ARRAY_DESIGN_PACKAGE;

	else if (name == PHYSICAL_ARRAY_DESIGN_ELEMENT)
		currentElement = ARRAY_DESIGN_ASSNLIST;
}

/*
 * Advance the current element.
 */
bool SAXArrayHandlers::MoveCurrentElementForward(const XMLCh* const name)
{
	if (name == 0)
		return false;

	if (name == BIOMATERIAL_PACKAGE_ELEMENT)
		currentElement = BIOMATERIAL_PACKAGE;

	else if (name == BIOMATERIAL_ASSNLIST_ELEMENT)
		currentElement = BIOMATERIAL_ASSNLIST;

	else if (name == BIOSOURCE_ELEMENT)
		currentElement = BIOSOURCE;

	else if (name == PROPERTYSETS_ASSNLIST_ELEMENT && currentElement == BIOSOURCE)
		currentElement = BIOSOURCE_PROPERTYSETS_ASSNLIST;

	else if (name == NAME_VALUE_TYPE_ELEMENT && currentElement == BIOSOURCE_PROPERTYSETS_ASSNLIST)
		currentElement = BIOSOURCE_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE;

	else if (name == CHARACTERISTICS_ASSNLIST_ELEMENT && currentElement == BIOSOURCE)
		currentElement = BIOSOURCE_CHARACTERISTICS_ASSNLIST;

	else if (name == ONTOLOGY_ENTRY_ELEMENT && currentElement == BIOSOURCE_CHARACTERISTICS_ASSNLIST)
		currentElement = BIOSOURCE_CHARACTERISTICS_ASSNLIST_ONTOLOGY;

	else if (name == MATERIAL_TYPE_ASSN_ELEMENT && currentElement == BIOSOURCE)
		currentElement = BIOSOURCE_MATERIALTYPE_ASSNLIST;

	else if (name == ONTOLOGY_ENTRY_ELEMENT && currentElement == BIOSOURCE_MATERIALTYPE_ASSNLIST)
		currentElement = BIOSOURCE_MATERIALTYPE_ASSNLIST_ONTOLOGY;

	else if (name == EXPERIMENT_PACKAGE_ELEMENT)
		currentElement = EXPERIMENT_PACKAGE;

	else if (name == EXPERIMENT_ASSNLIST_ELEMENT)
		currentElement = EXPERIMENT_ASSNLIST;

	else if (name == EXPERIMENT_ELEMENT)
		currentElement = EXPERIMENT;

	else if (name == PROPERTYSETS_ASSNLIST_ELEMENT && currentElement == EXPERIMENT)
		currentElement = EXPERIMENT_PROPERTYSETS_ASSNLIST;

	else if (name == NAME_VALUE_TYPE_ELEMENT && currentElement == EXPERIMENT_PROPERTYSETS_ASSNLIST)
		currentElement = EXPERIMENT_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE;

	else if (name == ARRAY_DESIGN_PACKAGE_ELEMENT)
		currentElement = ARRAY_DESIGN_PACKAGE;

	else if (name == ARRAY_DESIGN_ASSNLIST_ELEMENT)
		currentElement = ARRAY_DESIGN_ASSNLIST;

	else if (name == PHYSICAL_ARRAY_DESIGN_ELEMENT)
		currentElement = PHYSICAL_ARRAY_DESIGN;

	else if (currentElement == BIOSOURCE_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE ||
			currentElement == EXPERIMENT_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE)
	{
		++levelsDeep;
		return false;
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

	switch (currentElement)
	{
	case EXPERIMENT:
		StoreExperimentNameAttribute(attributes);
		break;

	case BIOSOURCE:
		StoreSampleNameAttribute(attributes);
		break;

	case BIOSOURCE_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE:
	case EXPERIMENT_PROPERTYSETS_ASSNLIST_NAMEVALUETYPE:
		StoreNameValueAttributes(attributes);
		break;

	case BIOSOURCE_MATERIALTYPE_ASSNLIST_ONTOLOGY:
		StoreSampleTypeAttribute(attributes);
		break;

	case BIOSOURCE_CHARACTERISTICS_ASSNLIST_ONTOLOGY:
		StoreSampleProjectAttribute(attributes);
		break;

	case PHYSICAL_ARRAY_DESIGN:
		StoreArrayType(attributes);
		break;

	default:
		break;
	}
}

/*
 * Stores the array type.
 */
void SAXArrayHandlers::StoreArrayType(AttributeList&  attributes)
{
	unsigned int len = attributes.getLength();
	for (unsigned int index = 0; index < len; index++)
	{
		if (attributes.getName(index) == NAME_ATTRIBUTE)
		{
			arrayData->SetArrayType(XMLChToString(attributes.getValue(index)));
			break;
		}
	}
}

/*
 * Stores the experiment name attribute from the experiment element.
 */
void SAXArrayHandlers::StoreExperimentNameAttribute(AttributeList&  attributes)
{
	unsigned int len = attributes.getLength();
	for (unsigned int index = 0; index < len; index++)
	{
		if (attributes.getName(index) == NAME_ATTRIBUTE)
		{
			arrayData->SetExperimentName(XMLChToString(attributes.getValue(index)));
			break;
		}
	}
}

/*
 * Stores the sample name attribute from the bio source element.
 */
void SAXArrayHandlers::StoreSampleNameAttribute(AttributeList&  attributes)
{
	unsigned int len = attributes.getLength();
	for (unsigned int index = 0; index < len; index++)
	{
		if (attributes.getName(index) == NAME_ATTRIBUTE)
		{
			AttributeNameValueType param;
			param.name = GCOS_SAMPLE_NAME_PARAMETER_NAME;
			param.value = XMLChToString(attributes.getValue(index));
			param.type = STRING_TYPE;
			arrayData->Attributes().push_back(param);
			break;
		}
	}
}

/*
 * Stores the sample type attribute from the biosource/materialtype element.
 */
void SAXArrayHandlers::StoreSampleTypeAttribute(AttributeList&  attributes)
{
	unsigned int len = attributes.getLength();
	for (unsigned int index = 0; index < len; index++)
	{
		if (attributes.getName(index) == ONTOLOGY_VALUE_ATTRIBUTE)
		{
			AttributeNameValueType param;
			param.name = GCOS_SAMPLE_TYPE_PARAMETER_NAME;
			param.value = XMLChToString(attributes.getValue(index));
			param.type = STRING_TYPE;
			arrayData->Attributes().push_back(param);
			break;
		}
	}
}

/*
 * Stores the sample project attribute from the biosource/characteristics element.
 */
void SAXArrayHandlers::StoreSampleProjectAttribute(AttributeList&  attributes)
{
	unsigned int len = attributes.getLength();
	for (unsigned int index = 0; index < len; index++)
	{
		if (attributes.getName(index) == ONTOLOGY_VALUE_ATTRIBUTE)
		{
			AttributeNameValueType param;
			param.name = GCOS_SAMPLE_PROJECT_PARAMETER_NAME;
			param.value = XMLChToString(attributes.getValue(index));
			param.type = STRING_TYPE;
			arrayData->Attributes().push_back(param);
			break;
		}
	}
}

/*
 * Stores the attributes from a name value type element.
 * Parse out the probe array type into its own member.
 */
void SAXArrayHandlers::StoreNameValueAttributes(AttributeList&  attributes)
{
	unsigned int len = attributes.getLength();
	AttributeNameValueType param;
	for (unsigned int index = 0; index < len; index++)
	{
		if (attributes.getName(index) == NAME_ATTRIBUTE)
		{
			param.name = XMLChToString(attributes.getValue(index));
		}
		else if (attributes.getName(index) == VALUE_ATTRIBUTE)
		{
			param.value = XMLChToString(attributes.getValue(index));
		}
		else if (attributes.getName(index) == TYPE_ATTRIBUTE)
		{
			param.type = XMLChToString(attributes.getValue(index));
			if (param.name == PROBE_ARRAY_TYPE_PARAMETER_NAME)
			{
				arrayData->SetArrayType(param.value);
			}
			else
			{
				arrayData->Attributes().push_back(param);
			}
		}
	}
}
