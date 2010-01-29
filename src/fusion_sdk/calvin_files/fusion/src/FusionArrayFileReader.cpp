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


#include "calvin_files/fusion/src/FusionArrayFileReader.h"
//
#include "calvin_files/parsers/src/ArrayFileReader.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include "file/DttArrayFileReader.h"
#include "file/EXPFileData.h"
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_array;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_io;
using namespace affymetrix_dttarray;
using namespace affymetrix_calvin_parameter;
using namespace affxexp;
using namespace std;

/*
 * Clear any members.
 */
FusionArrayFileReader::FusionArrayFileReader()
{
}

/*
 * Clear any members.
 */
FusionArrayFileReader::~FusionArrayFileReader()
{
}

/*
 * Read the file into the array data parameter.
 * First try the calvin array file,
 * next the DTT array file,
 * lastly the EXP file.
 *
 * The DTT array file will contain the probe array type and a list of
 * user attributes. Copy these attributes plus the array type to the
 * array data object. The probe array type will go in the physical
 * array section.
 *
 * The EXP array file will contain the probe array type and a list of 
 * sample parameters. Copy these to the array data object. The probe
 * array type will go in the physical array section.
 *
 */
bool FusionArrayFileReader::Read(const string &fileName, ArrayData &arrayData)
{
	arrayData.Clear();

	// The calvin array file.
	ArrayFileReader arrayReader;
	if (ArrayFileReader::IsFileType(fileName, ARRAY_SET_FILE_TYPE_IDENTIFIER) == true)
	{
		if (arrayReader.Read(fileName, arrayData) == true)
			return true;
	}

	// The probe array type name.
	wstring probeArrayTypeName = L"Probe Array Type";

	// The DTT or Exporter SDK MAGE-ML file.
	DttArrayData dttData;
	DttArrayFileReader dttReader;
	dttReader.SetFileName(fileName.c_str());
	if (dttReader.Read(dttData) == true)
	{
		ParameterNameValueDefaultRequiredTypeList &userAtts = arrayData.UserAttributes();
		ParameterNameValueDefaultRequiredType userAtt;
		AttributeNameValueTypeList &atts = dttData.Attributes();
		int paramIndex=0;
		for (AttributeNameValueTypeList::iterator it=atts.begin(); it!=atts.end(); it++, paramIndex++)
		{
			AttributeNameValueType param = *it;

			userAtt.SetName(StringUtils::ConvertMBSToWCS(param.name));
			userAtt.SetValueText(StringUtils::ConvertMBSToWCS(param.value));
			userAtts.push_back(userAtt);
		}

		ArrayAttributesVector &physArrays = arrayData.PhysicalArraysAttributes();
		physArrays.resize(1);
		ArrayAttributes &physArray = physArrays[0];
		ParameterNameValuePair nameValueParam;
		nameValueParam.Name = probeArrayTypeName;
		nameValueParam.Value = StringUtils::ConvertMBSToWCS(dttData.GetArrayType());
		physArray.Attributes().push_back(nameValueParam);

		return true;
	}

	// The MAS EXP file.
	CEXPFileData expReader;
	expReader.SetFileName(fileName.c_str());
	if (expReader.Read() == true)
	{
		ParameterNameValueDefaultRequiredTypeList &userAtts = arrayData.UserAttributes();
		ParameterNameValueDefaultRequiredType userAtt;
		TagValuePairTypeList &atts = expReader.GetSampleParameters();
		int paramIndex=0;
		for (TagValuePairTypeList::iterator it=atts.begin(); it!=atts.end(); it++, paramIndex++)
		{
			TagValuePairType param = *it;
			userAtt.SetName(StringUtils::ConvertMBSToWCS(param.Tag));
			userAtt.SetValueText(StringUtils::ConvertMBSToWCS(param.Value));
			userAtts.push_back(userAtt);
		}

		ArrayAttributesVector &physArrays = arrayData.PhysicalArraysAttributes();
		physArrays.resize(1);
		ArrayAttributes &physArray = physArrays[0];
		ParameterNameValuePair nameValueParam;
		nameValueParam.Name = probeArrayTypeName;
		nameValueParam.Value = StringUtils::ConvertMBSToWCS(expReader.GetArrayType());
		physArray.Attributes().push_back(nameValueParam);

		return true;
	}

	return false;
}
