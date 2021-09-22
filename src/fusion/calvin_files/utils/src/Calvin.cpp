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

#include "calvin_files/utils/src/Calvin.h"
//
#include "util/AffxMultiDimensionalArray.h"
#include "util/Fs.h"
#include "util/Util.h"
//
#include <typeinfo>
//


void Calvin::test()
{
	Verbose::out(1, "*");
	std::set<std::string> setIgnore;
	std::map<std::string, float> mapEpsilon;

	// Header parameters to ignore
	setIgnore.insert("FileCreationTime");
	setIgnore.insert("FileIdentifier");
	setIgnore.insert("program-version");
	setIgnore.insert("create_date");
	setIgnore.insert("create-date");
	setIgnore.insert("affymetrix-algorithm-param-option-verbose");
	setIgnore.insert("affymetrix-algorithm-param-option-exec-guid");
	setIgnore.insert("affymetrix-algorithm-param-option-program-cvs-id");
	setIgnore.insert("affymetrix-algorithm-param-option-version-to-report");
	setIgnore.insert("affymetrix-algorithm-param-option-command-line");
	setIgnore.insert("affymetrix-algorithm-param-option-run-probeset-genotype");
	setIgnore.insert("affymetrix-algorithm-param-option-cels");
	setIgnore.insert("affymetrix-algorithm-param-option-out-dir");
	setIgnore.insert("affymetrix-algorithm-param-start-time-start");
	setIgnore.insert("affymetrix-algorithm-param-start-free-mem-at-start");

	// Data Set columns to ignore (Specify as Group.Set.Column).
	setIgnore.insert("Segments.CN.SegmentID");
	setIgnore.insert("Segments.LOH.SegmentID");
	setIgnore.insert("Segments.CNNeutralLOH.SegmentID");
	setIgnore.insert("Segments.NormalDiploid.SegmentID");
	setIgnore.insert("Segments.Mosaicism.SegmentID");
	setIgnore.insert("Segments.NoCall.SegmentID");

	AffxString strFileName1 = "c:/apt/affy/sdk/copynumber/apt-copynumber-workflow/regression/test-generated/GenomeWideSNP_6/copynumber_HapMap270_6_cels/NA07029_GW6_C.CN5.cnchp";
	AffxString strFileName2 = "c:/apt/affy/sdk/regression-data/data/copy-number/GenomeWideSNP_6/copynumber_HapMap270_6_cels/NA07029_GW6_C.CN5.cnchp";
	Calvin::equivalent(strFileName1, strFileName2, setIgnore, mapEpsilon, (float)0.0001, false);
	Verbose::out(1, "*");
}

/** 
 * Read the Calvin file, and store the data in memory.
 * @param strFileName - The name of the Calvin file to load data from.
 * @return - bool value. (true if successful)
 */
bool Calvin::load(const AffxString& strFileName)
{
	bool bSuccessful = false;
	AffxString strValue;
	int8_t c = 0;
	unsigned char uc = 0;
	short n = 0;
	unsigned short un = 0;
	int i = 0;
	unsigned int ui = 0;
	float f = 0;
	std::wstring wstr;
	clear();
	affymetrix_calvin_io::GenericFileReader reader;
	affymetrix_calvin_io::GenericData genericData;
	try
	{
		m_strFileName = strFileName;
//		Verbose::out(1, "*");
		Verbose::out(1, "Reading Calvin file: " + m_strFileName);
		reader.SetFilename(m_strFileName);
		reader.Open(genericData);
		getCalvinHeader().add(new CalvinParameter("FileCreationTime", CalvinValue::TextType, StringUtils::ConvertWCSToMBS(genericData.Header().GetGenericDataHdr()->GetFileCreationTime())));	
		
		affymetrix_calvin_io::GenericDataHeader* pHeader = genericData.Header().GetGenericDataHdr();
		loadHeader(*pHeader, getCalvinHeader());
		int iParentCount = genericData.Header().GetGenericDataHdr()->GetParentCnt();
		for (int iParentIndex = 0; (iParentIndex < iParentCount); iParentIndex++)
		{
			CalvinHeader* pHeader = new CalvinHeader;
			getParentCalvinHeaders().add(pHeader);
			affymetrix_calvin_io::GenericDataHeader Header = genericData.Header().GetGenericDataHdr()->GetParent(iParentIndex);
			loadHeader(Header, *pHeader);
		}	
		genericData.Header().GetGenericDataHdr()->Clear();
		WStringVector vDataGroupNames;
		genericData.DataGroupNames(vDataGroupNames);
		for (unsigned int uiDataGroupIndex = 0; (uiDataGroupIndex < vDataGroupNames.size()); uiDataGroupIndex++)
		{
			CalvinDataGroup* pGroup = new CalvinDataGroup;
			getCalvinDataGroups().add(pGroup);
			pGroup->setName(StringUtils::ConvertWCSToMBS(vDataGroupNames[uiDataGroupIndex]));
			WStringVector vDataSetNames;
			genericData.DataSetNames(uiDataGroupIndex, vDataSetNames);
			for (unsigned int uiDataSetIndex = 0; (uiDataSetIndex < vDataSetNames.size()); uiDataSetIndex++)
			{
				CalvinDataSet* pSet = new CalvinDataSet;
				pGroup->getCalvinDataSets().add(pSet);
				affymetrix_calvin_io::DataSet* pDataSet = genericData.DataSet(uiDataGroupIndex, uiDataSetIndex);
				pDataSet->Open();
				pSet->setName(StringUtils::ConvertWCSToMBS(vDataSetNames[uiDataSetIndex]));
				for (int uiColIndex = 0; (uiColIndex < pDataSet->Cols()); uiColIndex++)
				{
					pSet->getColumnHeaders().add(new AffxString(StringUtils::ConvertWCSToMBS(pDataSet->Header().GetColumnInfo(uiColIndex).GetName())));
				}
				pSet->getCalvinValues().reserve(pDataSet->Rows());
				for (int uiRowIndex = 0; (uiRowIndex < pDataSet->Rows()); uiRowIndex++)
				{
					AffxArray<CalvinValue>* pRow = new AffxArray<CalvinValue>(0, pDataSet->Cols());
					pSet->getCalvinValues().add(pRow);
					for (int uiColIndex = 0; (uiColIndex < pDataSet->Cols()); uiColIndex++)
					{	
						strValue.clear();
						switch (pDataSet->Header().GetColumnInfo(uiColIndex).GetColumnType())
						{
						case CalvinValue::Int8Type:
							pDataSet->GetData(uiRowIndex, uiColIndex, c);
							strValue = ::getInt(c);
							break;
						case CalvinValue::UInt8Type:
							pDataSet->GetData(uiRowIndex, uiColIndex, uc);
							strValue = ::getInt(uc);
							break;
						case CalvinValue::Int16Type:
							pDataSet->GetData(uiRowIndex, uiColIndex, n);
							strValue = ::getInt(n);
							break;
						case CalvinValue::UInt16Type:
							pDataSet->GetData(uiRowIndex, uiColIndex, un);
							strValue = ::getInt(un);
							break;
						case CalvinValue::Int32Type:
							pDataSet->GetData(uiRowIndex, uiColIndex, i);
							strValue = ::getInt(i);
							break;
						case CalvinValue::UInt32Type:
							pDataSet->GetData((int)uiRowIndex, (int)uiColIndex, ui);
							strValue = ::getUnsignedInt(ui);
							break;
						case CalvinValue::FloatType:
							pDataSet->GetData(uiRowIndex, uiColIndex, f);
							strValue = ::getDouble(f, 10);
							break;
						case 7:
							pDataSet->GetData(uiRowIndex, uiColIndex, strValue);
							break;
						case 8:
							pDataSet->GetData(uiRowIndex, uiColIndex, wstr);
							strValue = StringUtils::ConvertWCSToMBS(wstr);
							break;
						default:
							break;
						}
						pRow->add(new CalvinValue((CalvinValue::ParameterType)pDataSet->Header().GetColumnInfo(uiColIndex).GetColumnType(), strValue)); 
					}
				}
				pDataSet->Close();
				pDataSet->Delete();
			}
		}
		genericData.Clear();	
		bSuccessful = true;
	} catch(...) {genericData.Clear(); Err::errAbort("Cannot read Calvin file: " + strFileName);}
	return bSuccessful;
}

/** 
 * Load the Calvin header into memory.
 * @param Header - A pointer to the affymetrix_calvin_io::GenericDataHeader for the specified Calvin file.
 */
void Calvin::loadHeader(affymetrix_calvin_io::GenericDataHeader& Header, CalvinHeader& header)
{
	AffxString strValue;
	header.reserve(Header.GetNameValParamCnt() + 4);
	//File header
	header.add(new CalvinParameter("FileIdentifier", CalvinValue::AsciiType, Header.GetFileId()));
	header.add(new CalvinParameter("FileTypeIdentifier", CalvinValue::AsciiType, Header.GetFileTypeId()));
	header.add(new CalvinParameter("FileLocale", CalvinValue::TextType, StringUtils::ConvertWCSToMBS(Header.GetLocale())));

	// Header Parameters
	int iParamCount = Header.GetNameValParamCnt();
	affymetrix_calvin_parameter::ParameterNameValueType param;
	for (int iParamIndex = 0; (iParamIndex < iParamCount); iParamIndex++)
	{
		param = Header.GetNameValParam(iParamIndex);
		strValue.clear();
		switch (param.GetParameterType())
		{
		case CalvinValue::Int8Type:
			strValue = ::getInt(param.GetValueInt8());
			break;
		case CalvinValue::UInt8Type:
			strValue = ::getInt(param.GetValueUInt8());
			break;
		case CalvinValue::Int16Type:
			strValue = ::getInt(param.GetValueInt16());
			break;
		case CalvinValue::UInt16Type:
			strValue = ::getInt(param.GetValueUInt16());
			break;
		case CalvinValue::Int32Type:
			strValue = ::getInt(param.GetValueInt32());
			break;
		case CalvinValue::UInt32Type:
			strValue = ::getUnsignedInt(param.GetValueUInt32());
			break;
		case CalvinValue::FloatType:
			strValue = ::getDouble(param.GetValueFloat(), 10);
			break;
		case CalvinValue::AsciiType:
			strValue = param.GetValueAscii();
			break;
		case CalvinValue::TextType:
			strValue = StringUtils::ConvertWCSToMBS(param.GetValueText());
			break;
		default:
			break;
		}
		header.add(new CalvinParameter(StringUtils::ConvertWCSToMBS(param.GetName()), (CalvinValue::ParameterType)param.GetParameterType(), strValue));
	}
}


bool Calvin::equivalent(Calvin& that, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon, bool bCheckHeader, float fFraction)
{
	if (bCheckHeader)
	{
		if (!equivalentHeader("File ", getCalvinHeader(), that.getCalvinHeader(), setIgnore, mapEpsilon, fEpsilon, fFraction)) 
		{
			return false;
		}
		if (getParentCalvinHeaders().getCount() != that.getParentCalvinHeaders().getCount())
		{
			Verbose::out(1, "Files do not have the same number of Parent Headers.");
			return false;
		}
		for (int iParentIndex = 0; (iParentIndex < getParentCalvinHeaders().getCount()); iParentIndex++)
		{
			if (!equivalentHeader("Parent " + ::getInt(iParentIndex + 1) + " ", *this->getParentCalvinHeaders().getAt(iParentIndex), *that.getParentCalvinHeaders().getAt(iParentIndex), setIgnore, mapEpsilon, fEpsilon, fFraction)) 
			{
				return false;
			}
		}
	}
	if (this->getCalvinDataGroups().getCount() != that.getCalvinDataGroups().getCount())
	{
		Verbose::out(1, "Files do not have the same number of Data Groups.");
		return false;
	}
	for (int iGroupIndex = 0; (iGroupIndex < getCalvinDataGroups().getCount()); iGroupIndex++)
	{
		CalvinDataGroup* pThisGroup = this->getCalvinDataGroups().getAt(iGroupIndex);
		CalvinDataGroup* pThatGroup = that.getCalvinDataGroups().getAt(iGroupIndex);
		if (pThisGroup->getName() != pThatGroup->getName())
		{
			Verbose::out(1, "File Data Group name mismatch: " + pThisGroup->getName() + " != " + pThatGroup->getName());
			return false;
		}
		if (pThisGroup->getCalvinDataSets().getCount() != pThatGroup->getCalvinDataSets().getCount())
		{
			Verbose::out(1, "Data Group " + pThisGroup->getName() + " does not have the same number of Data Sets.");
			return false;
		}
		for (int iSetIndex = 0; (iSetIndex < pThisGroup->getCalvinDataSets().getCount()); iSetIndex++)
		{
			CalvinDataSet* pThisSet = pThisGroup->getCalvinDataSets().getAt(iSetIndex);
			CalvinDataSet* pThatSet = pThatGroup->getCalvinDataSets().getAt(iSetIndex);
			if (pThisSet->getName() != pThatSet->getName())
			{
				Verbose::out(1, "File Data Set name mismatch: " + pThisGroup->getName() + "." + pThisSet->getName() + " != " + pThatGroup->getName() + "." + pThatSet->getName());
				return false;
			}
			if (pThisSet->getColumnHeaders().getCount() != pThatSet->getColumnHeaders().getCount())
			{
				Verbose::out(1, "Data Set " + pThisGroup->getName() + "." + pThisSet->getName() + " does not have the same number of Columns.");
				return false;
			}
			if (pThisSet->getCalvinValues().getCount() != pThatSet->getCalvinValues().getCount())
			{
				Verbose::out(1, "Data Set " + pThisGroup->getName() + "." + pThisSet->getName() + " does not have the same number of Rows.");
				return false;
			}
			for (int iColIndex = 0; (iColIndex < pThisSet->getColumnHeaders().getCount()); iColIndex++)
			{
				if (*pThisSet->getColumnHeaders().getAt(iColIndex) != *pThatSet->getColumnHeaders().getAt(iColIndex))
				{
					Verbose::out(1, "File Data Set column name mismatch: " + pThisGroup->getName() + "." + pThisSet->getName() + "." + *pThisSet->getColumnHeaders().getAt(iColIndex) + " != " + pThatGroup->getName() + "." + pThatSet->getName() + "." + *pThatSet->getColumnHeaders().getAt(iColIndex));
					return false;
				}
			}
			for (int iRowIndex = 0; (iRowIndex < pThisSet->getCalvinValues().getCount()); iRowIndex++)
			{
				if (pThisSet->getCalvinValues().getAt(iRowIndex)->getCount() != pThatSet->getCalvinValues().getAt(iRowIndex)->getCount())
				{
					Verbose::out(1, "Data Set " + pThisGroup->getName() + "." + pThisSet->getName() + " is malformed.");
					return false;
				}
				for (int iColIndex = 0; (iColIndex < pThisSet->getCalvinValues().getAt(iRowIndex)->getCount()); iColIndex++)
				{
					CalvinValue* pThis = pThisSet->getCalvinValues().getAt(iRowIndex)->getAt(iColIndex);
					CalvinValue* pThat = pThatSet->getCalvinValues().getAt(iRowIndex)->getAt(iColIndex);
					if (pThis->getParameterType() != pThat->getParameterType())
					{
						Verbose::out(1, "Value Type mismatch for Data Set Column " + pThisGroup->getName() + "." + pThisSet->getName() + "." + *pThisSet->getColumnHeaders().getAt(iColIndex));
						return false;
					}
					if (pThis->getParameterType() == CalvinValue::FloatType)
					{
						float fThis = (float)::getDouble(pThis->getValue());
						float fThat = (float)::getDouble(pThat->getValue());
						// allowed absolute difference from fractional tolerance (zero by default)
						float fEpsilon2 = fFraction*Max( fabs(fThis), fabs(fThat) );
						// absolute difference is acceptable if it satisfies either (least restrictive) tolerance
						if (fabs(fThis - fThat) > Max(fEpsilon,fEpsilon2) )
						{
							Verbose::out(1, "Value is out of spec. for Data Set Column " + pThisGroup->getName() + "." + pThisSet->getName() + "." + *pThisSet->getColumnHeaders().getAt(iColIndex) + "\tDifference = " + ::getDouble(fabs(fThis - fThat), 10));
							return false;
						}
					}
					else if (pThis->getValue() != pThat->getValue())
					{
						Verbose::out(1, "Value mismatch for Data Set Column " + pThisGroup->getName() + "." + pThisSet->getName() + "." + *pThisSet->getColumnHeaders().getAt(iColIndex) +  "\t" + pThis->getValue() + " != " + pThat->getValue());
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool Calvin::equivalentHeader(const AffxString& strPrompt, CalvinHeader& headerThis, CalvinHeader& headerThat, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon, float fFraction)
{
	if (headerThis.getCount() != headerThat.getCount())
	{
		Verbose::out(1, "WARNING: " + strPrompt + "Header does not have the same number of Parameters.");
//		return false;
	}
	// Note that this check allows the 'that' header to have more items in it than the 'this' header.
	// Assuming that the 'this' header is the gold data and the 'that' header is the test data.
	headerThat.quickSort(0); // By Parameter Name
	for (int iIndex = 0; (iIndex < headerThis.getCount()); iIndex++)
	{
		CalvinParameter* pThis = headerThis.getAt(iIndex);
		int iSearchIndex = headerThat.binarySearch(*pThis, 0); // By Parameter Name
		if (iSearchIndex != -1)
		{
			CalvinParameter* pThat = headerThat.getAt(iSearchIndex);
			if (!equivalentParameter(strPrompt, *pThis, *pThat, setIgnore, mapEpsilon, fEpsilon, fFraction)) 
			{
				return false;
			}
		}
		else
		{
			Verbose::out(1, strPrompt + "Header parameter " + pThis->getName() + " is missing.");
			return false;
		}
	}
	return true;
}

bool Calvin::equivalentParameter(const AffxString& strPrompt, CalvinParameter& This, CalvinParameter& That, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon, float fFraction)
{
	if (This.getName() != That.getName())
	{
		Verbose::out(1, strPrompt + "Header Parameter name mismatch: " + This.getName() + " != " + That.getName());
		return false;
	}
	if (This.getParameterType() != That.getParameterType())
	{
		Verbose::out(1, strPrompt + "Header Parameter Type mismatch for parameter: " + This.getName());
		return false;
	}		
	if (setIgnore.find(This.getName()) == setIgnore.end()) 
	{
		float fEps = fEpsilon;
		if (mapEpsilon.find(This.getName()) != mapEpsilon.end()) {fEps = mapEpsilon.find(This.getName())->second;}
		if (This.getParameterType() == CalvinValue::FloatType)
		{
			float fThis = (float)::getDouble(This.getValue());
			float fThat = (float)::getDouble(That.getValue());
			// allowed absolute difference from fractional tolerance (zero by default)
			float fEpsilon2 = fFraction*Max( fabs(fThis), fabs(fThat) );
			// absolute difference is acceptable if it satisfies either (least restrictive) tolerance
			if (fabs(fThis - fThat) > Max(fEps,fEpsilon2))
			{
				Verbose::out(1, strPrompt + "Header Parameter Value is out of spec. for " + This.getName() + "\tDifference = " + ::getDouble(fabs(fThis - fThat), 10) + "\tEpsilon = " + ::getDouble(fEps, 10));
				return false;
			}
		}
		else if (This.getValue() != That.getValue())
		{
			Verbose::out(1, strPrompt + "Header Parameter Value mismatch for " + This.getName() + "\t" + This.getValue() + " != " + That.getValue());
			return false;
		}
	}
	return true;

}


//  We have two functions named equivalent with different signatures.  Extra functionality of setSetIgnore was added.
//  The signatures are maintained for backwards compatibility. 
bool Calvin::equivalent(	const AffxString& strFileName1, 
                                const AffxString& strFileName2, 
                                std::set<std::string>& setIgnore, 
                                std::map<std::string, float>& mapEpsilon, 
                                float fEpsilon, 
                                double dCorrelationCutoff, 
                                bool bCheckHeader, 
                                int iMessageLimit, 
                                float fFraction)
{
        std::set<std::string> setSetIgnore;
 
        return equivalent(	strFileName1, 
                        strFileName2, 
                        setIgnore, 
                        setSetIgnore, 
                        mapEpsilon, 
                        fEpsilon, 
                        dCorrelationCutoff, 
                        bCheckHeader, 
                        iMessageLimit, 
                        fFraction);
}

bool Calvin::equivalent(	const AffxString& strFileName1, 
                                const AffxString& strFileName2, 
                                std::set<std::string>& setIgnore, 
                                std::set<std::string>& setSetIgnore, 
                                std::map<std::string, float>& mapEpsilon, 
                                float fEpsilon, 
                                double dCorrelationCutoff, 
                                bool bCheckHeader, 
                                int iMessageLimit, 
                                float fFraction)
{
	Verbose::out(1, "Comparing " + strFileName1 + " and " + strFileName2);
	bool bSuccessful = true;
	bool bFileOpen1 = false;
	bool bFileOpen2 = false;
	AffxString str1;
	AffxString str2;
	int8_t c1 = 0;
	int8_t c2 = 0;
	unsigned char uc1 = 0;
	unsigned char uc2 = 0;
	short n1 = 0;
	short n2 = 0;
	unsigned short un1 = 0;
	unsigned short un2 = 0;
	int i1 = 0;
	int i2 = 0;
	unsigned int ui1 = 0;
	unsigned int ui2 = 0;
	float f1 = 0;
	float f2 = 0;
	float fEpsilon2 = 0;
	std::wstring wstr1;
	std::wstring wstr2;
	affymetrix_calvin_io::GenericFileReader reader1;
	affymetrix_calvin_io::GenericData genericData1;
	affymetrix_calvin_io::GenericFileReader reader2;
	affymetrix_calvin_io::GenericData genericData2;
	affymetrix_calvin_io::DataSet* pDataSet1 = NULL;
	affymetrix_calvin_io::DataSet* pDataSet2 = NULL;
	try
	{
		reader1.SetFilename(Fs::convertToUncPath(strFileName1));
		reader1.Open(genericData1);		
		bFileOpen1 = true;  // remember status for file exception catch block
		//TODO file exception try/catch block for file1 should move here per Harley
		
		reader2.SetFilename(Fs::convertToUncPath(strFileName2));
		reader2.Open(genericData2);		
		bFileOpen2 = true;  // remember status for file exception catch block
		//TODO file exception try/catch block for file2 should move here per Harley

		if (bCheckHeader)
		{			
			CalvinParameter param1("FileCreationTime", CalvinValue::TextType, StringUtils::ConvertWCSToMBS(genericData1.Header().GetGenericDataHdr()->GetFileCreationTime()));	
			CalvinParameter param2("FileCreationTime", CalvinValue::TextType, StringUtils::ConvertWCSToMBS(genericData2.Header().GetGenericDataHdr()->GetFileCreationTime()));	

			if (!equivalentParameter("File ", param1, param1, setIgnore, mapEpsilon, fEpsilon, fFraction)) 
			{
				return false;
			}

			affymetrix_calvin_io::GenericDataHeader* pHeader1 = genericData1.Header().GetGenericDataHdr();
			affymetrix_calvin_io::GenericDataHeader* pHeader2 = genericData2.Header().GetGenericDataHdr();
			if (!equivalentHeader("File ", *pHeader1, *pHeader2, setIgnore, mapEpsilon, fEpsilon, fFraction)) 
			{
				return false;
			}
			int iParentCount1 = genericData1.Header().GetGenericDataHdr()->GetParentCnt();
			int iParentCount2 = genericData2.Header().GetGenericDataHdr()->GetParentCnt();
			int iParentCount = Min(iParentCount1, iParentCount2);
			if (iParentCount1 != iParentCount2)
			{
				Verbose::out(1, "Files do not have the same number of Parent Headers.");
				return false;
				 
			}
			for (int iParentIndex = 0; (iParentIndex < iParentCount); iParentIndex++)
			{
				affymetrix_calvin_io::GenericDataHeader Header1 = genericData1.Header().GetGenericDataHdr()->GetParent(iParentIndex);
				affymetrix_calvin_io::GenericDataHeader Header2 = genericData2.Header().GetGenericDataHdr()->GetParent(iParentIndex);
				if (!equivalentHeader("Parent " + ::getInt(iParentIndex + 1) + " ", Header1, Header2, setIgnore, mapEpsilon, fEpsilon, fFraction)) 
				{
					bSuccessful = false;
				}
			}
		}
		genericData1.Header().GetGenericDataHdr()->Clear();
		genericData2.Header().GetGenericDataHdr()->Clear();
		WStringVector vDataGroupNames1;
		WStringVector vDataGroupNames2;
		genericData1.DataGroupNames(vDataGroupNames1);
		genericData2.DataGroupNames(vDataGroupNames2);
		unsigned int uiDataGroupCount = Min(vDataGroupNames1.size(), vDataGroupNames2.size());
		if (vDataGroupNames1.size() != vDataGroupNames2.size())
		{
			Verbose::out(1, "Files do not have the same number of Data Groups. Gold = " + ::getInt(vDataGroupNames1.size()) + ", generated = " + ::getInt(vDataGroupNames2.size()));
			bSuccessful = false;
		}
		for (unsigned int uiDataGroupIndex = 0; (uiDataGroupIndex < uiDataGroupCount); uiDataGroupIndex++)
		{
			if (vDataGroupNames1[uiDataGroupIndex] != vDataGroupNames2[uiDataGroupIndex])
			{
				Verbose::out(1, "File Data Group name mismatch: " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + " != " + StringUtils::ConvertWCSToMBS(vDataGroupNames2[uiDataGroupIndex]));
				bSuccessful = false;
				continue;
			}
			WStringVector vDataSetNames1;
			WStringVector vDataSetNames2;
			genericData1.DataSetNames(uiDataGroupIndex, vDataSetNames1);
			genericData2.DataSetNames(uiDataGroupIndex, vDataSetNames2);
			unsigned int uiDataSetCount = Min(vDataSetNames1.size(), vDataSetNames2.size());
			if (vDataSetNames1.size() != vDataSetNames2.size())
			{
				Verbose::out(1, "Data Group " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + " does not have the same number of Data Sets.");
				bSuccessful = false;
				continue;
			}
			for (unsigned int uiDataSetIndex = 0; (uiDataSetIndex < uiDataSetCount); uiDataSetIndex++)
			{
				//Verbose::out(1, "Data Set " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]));
				pDataSet1 = genericData1.DataSet(uiDataGroupIndex, uiDataSetIndex);
				pDataSet2 = genericData2.DataSet(uiDataGroupIndex, uiDataSetIndex);
				pDataSet1->Open();
				pDataSet2->Open();
				if (vDataSetNames1[uiDataSetIndex] != vDataSetNames2[uiDataSetIndex])
				{
					Verbose::out(1, "File Data Set name mismatch: " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + " != " + StringUtils::ConvertWCSToMBS(vDataGroupNames2[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames2[uiDataSetIndex]));
					pDataSet1->Delete();
					pDataSet2->Delete();
					bSuccessful = false;
					continue;
				}

//MG

                                AffxString strSetName = StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) ;
                                if (setSetIgnore.find(strSetName) != setSetIgnore.end()) 
                                {
                                        Verbose::out(1, "Not doing a comparison of the DataSet" + strSetName);
                                        continue;
                                }
                                
				if (pDataSet1->Cols() != pDataSet2->Cols())
				{
					Verbose::out(1, "Data Set " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + " does not have the same number of Columns. Gold = " + ::getInt(pDataSet1->Cols()) + ", Generated = " + ::getInt(pDataSet2->Cols()));
					pDataSet1->Delete();
					pDataSet2->Delete();
					bSuccessful = false;
					continue;
				}
				if (pDataSet1->Rows() != pDataSet2->Rows())
				{
					Verbose::out(1, "Data Set " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + " does not have the same number of Rows. Gold = " + ::getInt(pDataSet1->Rows()) + ", Generated = " + ::getInt(pDataSet2->Rows()));
					pDataSet1->Delete();
					pDataSet2->Delete();
					bSuccessful = false;
					continue;
				}
				AffxMultiDimensionalArray<bool> vSkip(pDataSet1->Cols());
				AffxMultiDimensionalArray<float> vEpsilon(pDataSet1->Cols());
				for (int uiColIndex = 0; (uiColIndex < pDataSet1->Cols()); uiColIndex++)
				{
					if (pDataSet1->Header().GetColumnInfo(uiColIndex).GetName() != pDataSet2->Header().GetColumnInfo(uiColIndex).GetName())
					{
						Verbose::out(1, "File Data Set column name mismatch: " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + "." + StringUtils::ConvertWCSToMBS(pDataSet1->Header().GetColumnInfo(uiColIndex).GetName()) + " != " + StringUtils::ConvertWCSToMBS(vDataGroupNames2[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames2[uiDataSetIndex]) + "." + StringUtils::ConvertWCSToMBS(pDataSet2->Header().GetColumnInfo(uiColIndex).GetName()));
//						pDataSet1->Delete();
//						pDataSet2->Delete();
						bSuccessful = false;
						continue;
					}
					else
					{	
						AffxString strColumnName = StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + "." + StringUtils::ConvertWCSToMBS(pDataSet1->Header().GetColumnInfo(uiColIndex).GetName());
						if (setIgnore.find(strColumnName) != setIgnore.end()) {vSkip.set(uiColIndex, true);}
						if (mapEpsilon.find(strColumnName) != mapEpsilon.end()) {vEpsilon.set(uiColIndex, mapEpsilon.find(strColumnName)->second);}
						else {vEpsilon.set(uiColIndex, fEpsilon);}
					}
				}
				AffxMultiDimensionalArray<float> vMaxDifference(pDataSet1->Cols());
				AffxMultiDimensionalArray<int> vMaxDifferenceRow(pDataSet1->Cols());
				int iMessageCount = 0;
				for (int uiColIndex = 0; (uiColIndex < pDataSet1->Cols()); uiColIndex++)
				{						
					//Verbose::out(1, "Column: " + StringUtils::ConvertWCSToMBS(pDataSet1->Header().GetColumnInfo(uiColIndex).GetName()) + "\t" + ::getInt(pDataSet1->Header().GetColumnInfo(uiColIndex).GetColumnType()));

					int iFailureCount = 0;
					AffxMultiDimensionalArray<float> mx(pDataSet1->Rows(), 2);
					for (int uiRowIndex = 0; (uiRowIndex < pDataSet1->Rows()); uiRowIndex++)
					{
						if (pDataSet1->Header().GetColumnInfo(uiColIndex).GetColumnType() != pDataSet2->Header().GetColumnInfo(uiColIndex).GetColumnType())
						{
							Verbose::out(1, "Value Type mismatch for Data Set Column " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + "." + StringUtils::ConvertWCSToMBS(pDataSet1->Header().GetColumnInfo(uiColIndex).GetName()) + ", Gold = " + ::getInt(pDataSet1->Header().GetColumnInfo(uiColIndex).GetColumnType()) + ", Generated = " + ::getInt(pDataSet2->Header().GetColumnInfo(uiColIndex).GetColumnType()));
//							pDataSet1->Delete();
//							pDataSet2->Delete();
							bSuccessful = false;
							break;
						}
						if (vSkip.get(uiColIndex)) {continue;}
						bool bEquals = false;
						switch (pDataSet1->Header().GetColumnInfo(uiColIndex).GetColumnType())
						{
						case CalvinValue::Int8Type:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, c1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, c2);
							bEquals = (c1 == c2);
							break;
						case CalvinValue::UInt8Type:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, uc1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, uc2);
							bEquals = (uc1 == uc2);
							break;
						case CalvinValue::Int16Type:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, n1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, n2);
							f1 = (float)n1;
							f2 = (float)n2;
							if ((Util::isFinite(f1)) && (Util::isFinite(f2)))
							{
								mx.set(uiRowIndex, 0, f1);
								mx.set(uiRowIndex, 1, f2);
							}
							if ((f1 != f1) && (f2 != f2)) {bEquals = true;} // Both = NaN
							else {bEquals = (fabs(f1 - f2) <= vEpsilon.get(uiColIndex));}
							break;
						case CalvinValue::UInt16Type:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, un1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, un2);
							f1 = (float)un1;
							f2 = (float)un2;
							if ((Util::isFinite(f1)) && (Util::isFinite(f2)))
							{
								mx.set(uiRowIndex, 0, f1);
								mx.set(uiRowIndex, 1, f2);
							}
							if ((f1 != f1) && (f2 != f2)) {bEquals = true;} // Both = NaN
							else {bEquals = (fabs(f1 - f2) <= vEpsilon.get(uiColIndex));}
							break;
						case CalvinValue::Int32Type:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, i1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, i2);
							f1 = (float)i1;
							f2 = (float)i2;
							if ((Util::isFinite(f1)) && (Util::isFinite(f2)))
							{
								mx.set(uiRowIndex, 0, f1);
								mx.set(uiRowIndex, 1, f2);
							}
							if ((f1 != f1) && (f2 != f2)) {bEquals = true;} // Both = NaN
							else {bEquals = (fabs(f1 - f2) <= vEpsilon.get(uiColIndex));}
							break;
						case CalvinValue::UInt32Type:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, ui1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, ui2);
							f1 = (float)ui1;
							f2 = (float)ui2;
							if ((Util::isFinite(f1)) && (Util::isFinite(f2)))
							{
								mx.set(uiRowIndex, 0, f1);
								mx.set(uiRowIndex, 1, f2);
							}
							if ((f1 != f1) && (f2 != f2)) {bEquals = true;} // Both = NaN
							else {bEquals = (fabs(f1 - f2) <= vEpsilon.get(uiColIndex));}
							break;
						case CalvinValue::FloatType:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, f1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, f2);							
							fEpsilon2 = fFraction*Max( fabs(f1), fabs(f2) );
							if ((Util::isFinite(f1)) && (Util::isFinite(f2)))
							{
								mx.set(uiRowIndex, 0, f1);
								mx.set(uiRowIndex, 1, f2);
							}
							if ((f1 != f1) && (f2 != f2)) {bEquals = true;} // Both = NaN
							else {bEquals = (fabs(f1 - f2) <= Max(vEpsilon.get(uiColIndex),fEpsilon2));}
							break;
						case 7:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, str1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, str2);							
							bEquals = (str1 == str2);
							break;
						case 8:
							pDataSet1->GetData((int)uiRowIndex, (int)uiColIndex, wstr1);
							pDataSet2->GetData((int)uiRowIndex, (int)uiColIndex, wstr2);						
							bEquals = (wstr1 == wstr2);
							break;
						default:
							break;
						}
						if (!bEquals)
						{
							if ((pDataSet1->Header().GetColumnInfo(uiColIndex).GetColumnType() >= 2) && (pDataSet1->Header().GetColumnInfo(uiColIndex).GetColumnType() <= 6))
							{
								iFailureCount++;
								if (fabs(f1 - f2) > vMaxDifference.get(uiColIndex))
								{
									vMaxDifference.set(uiColIndex, fabs(f1 - f2));
									vMaxDifferenceRow.set(uiColIndex, uiRowIndex);
								}
							}
							else
							{
								bSuccessful = false;
								iMessageCount++;
								if (iMessageCount == (iMessageLimit + 1))
								{
									Verbose::out(1, "Message limit reached. Additional messages will be suppressed.");
								}
								else if (iMessageCount <= iMessageLimit)
								{
									Verbose::out(1, "Value mismatch for Data Set Column " +StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + "." + StringUtils::ConvertWCSToMBS(pDataSet1->Header().GetColumnInfo(uiColIndex).GetName()) +  " at row " + ::getUnsignedInt(uiRowIndex));
								}
							}
						}
					}
					if (vMaxDifference.get(uiColIndex) > 0)
					{
						double dCorrelation = mx.corr();
						if ((dCorrelation < dCorrelationCutoff) || (dCorrelationCutoff == 1))
						{
							bSuccessful = false;
							Verbose::out(1, "Value is out of spec. for Data Set Column " + StringUtils::ConvertWCSToMBS(vDataGroupNames1[uiDataGroupIndex]) + "." + StringUtils::ConvertWCSToMBS(vDataSetNames1[uiDataSetIndex]) + "." + StringUtils::ConvertWCSToMBS(pDataSet1->Header().GetColumnInfo(uiColIndex).GetName()) +  " MaxDifference at row " + ::getUnsignedInt(vMaxDifferenceRow.get(uiColIndex)) + "\tMaxDifference = " + ::getDouble(vMaxDifference.get(uiColIndex), 10) + "\tNumberFailures = " + ::getInt(iFailureCount) + "\tEpsilon = " + ::getDouble(vEpsilon.get(uiColIndex), 10) + "\tCorrelation = " + ::getDouble(dCorrelation, 10) + "\tNormDifference = " + ::getDouble(mx.normDifference(), 10));
						}
					}
				}
				pDataSet1->Delete();
				pDataSet2->Delete();
			}
		}
		if (bSuccessful)
		{
			Verbose::out(1, "Files are equivalent.");
		}
	} 
	/* When things go wrong see if we can die gracefully here. */
	catch(Except &e) {
		bSuccessful = false;
		Verbose::out(1, "Exception caught. "
				  "Message is: " + ToStr(e.what()));
	}
	catch(const std::bad_alloc &e) {
		bSuccessful = false;
		Verbose::out(1, "std::bad_alloc caught. "
				  "The application has run out of memory. "
				  "Message is: " + ToStr(e.what()));
	}
	catch(affymetrix_calvin_exceptions::FileNotFoundException &fnfe) {
		// rsatin fix for AFFY00023387: check file existence with message if not found
		bSuccessful = false;
		if( !bFileOpen1 )
			Verbose::out(1, "File not found: '" + strFileName1 + "'");
		else
			Verbose::out(1, "File not found: '" + strFileName2 + "'");
	}
	catch(affymetrix_calvin_exceptions::UnableToOpenFileException &utoe) {
		// rsatin fix for AFFY00023387: check file existence with message if not found
		bSuccessful = false;
		if( !bFileOpen1 )
			Verbose::out(1, "Unable to open file: '" + strFileName1 + "'");
		else
			Verbose::out(1, "Unable to open file: '" + strFileName2 + "'");
	}
	catch(affymetrix_calvin_exceptions::CalvinException &ce) {
		std::string strType = typeid(ce).name();
		bSuccessful = false;
		Verbose::out(1, "affymetrix_calvin_exceptions::CalvinException caught. " \
		 		  " Affymetrix GeneChip Command Console library has thrown an exception of type " + strType + \
				  " Message is: " + affymetrix_calvin_utilities::StringUtils::ConvertWCSToMBS(ce.Description()));
	}
	catch(const std::exception &e) {
		bSuccessful = false;
		Verbose::out(1, "std::exception caught. "
				  "Message is: " + ToStr(e.what()));
	}
	catch(...)
	{
		bSuccessful = false;
		Verbose::out(1, "Error in equivalency test. Unknown exception thrown.");
	}
	return bSuccessful;
}

/** 
 * Load the Calvin header into memory.
 * @param pHeader - A pointer to the affymetrix_calvin_io::GenericDataHeader for the specified Calvin file.
 */
bool Calvin::equivalentHeader(const AffxString& strPrompt, affymetrix_calvin_io::GenericDataHeader& Header1, affymetrix_calvin_io::GenericDataHeader& Header2, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon, float fFraction)
{
	bool bEquivalent = true;
	AffxString strValue1;
	AffxString strValue2;
	//File header
	CalvinParameter paramCalvin1;
	CalvinParameter paramCalvin2;
	paramCalvin1.set("FileIdentifier", CalvinValue::AsciiType, Header1.GetFileId());
	paramCalvin2.set("FileIdentifier", CalvinValue::AsciiType, Header2.GetFileId());
	if (!equivalentParameter(strPrompt, paramCalvin1, paramCalvin2, setIgnore, mapEpsilon, fEpsilon, fFraction)) {bEquivalent = false;}

	paramCalvin1.set("FileTypeIdentifier", CalvinValue::AsciiType, Header1.GetFileTypeId());
	paramCalvin2.set("FileTypeIdentifier", CalvinValue::AsciiType, Header2.GetFileTypeId());
	if (!equivalentParameter(strPrompt, paramCalvin1, paramCalvin2, setIgnore, mapEpsilon, fEpsilon, fFraction)) {bEquivalent = false;}

	paramCalvin1.set("FileLocale", CalvinValue::TextType, StringUtils::ConvertWCSToMBS(Header1.GetLocale()));
	paramCalvin2.set("FileLocale", CalvinValue::TextType, StringUtils::ConvertWCSToMBS(Header2.GetLocale()));
	if (!equivalentParameter(strPrompt, paramCalvin1, paramCalvin2, setIgnore, mapEpsilon, fEpsilon, fFraction)) {bEquivalent = false;}

	// Header Parameters
	int iParamCount1 = Header1.GetNameValParamCnt();
	int iParamCount2 = Header2.GetNameValParamCnt();
	if (iParamCount1 != iParamCount2)
	{
		Verbose::out(1, "WARNING: " + strPrompt + "Header does not have the same number of Parameters.");
//		bEquivalent = false;
	}
	affymetrix_calvin_parameter::ParameterNameValueType param1;
	affymetrix_calvin_parameter::ParameterNameValueType param2;
	for (int iParamIndex = 0; (iParamIndex < iParamCount1); iParamIndex++)
	{
		param1 = Header1.GetNameValParam(iParamIndex);
		//rsatin fix for AFFY00023387: ignore present/absent differences for fields in ignore list
		AffxString strParam1Name = StringUtils::ConvertWCSToMBS(param1.GetName());
		if (setIgnore.find(strParam1Name) != setIgnore.end()) {      
			continue;
		}
		bool bFound = false;
		for (int iParamIndex2 = 0; (iParamIndex2 < iParamCount2); iParamIndex2++)
		{
			param2 = Header2.GetNameValParam(iParamIndex2);
			if (param1.GetName() == param2.GetName())
			{
				bFound = true;
				if (param1.GetParameterType() != param2.GetParameterType())
				{
					Verbose::out(1, strPrompt + "Header Parameter Type mismatch for parameter: " + StringUtils::ConvertWCSToMBS(param1.GetName()));
					return false;
				}
				float fFraction_param = 0.0;
				strValue1.clear();
				strValue2.clear();
				switch (param1.GetParameterType())
				{
				case CalvinValue::Int8Type:
					strValue1 = ::getInt(param1.GetValueInt8());
					strValue2 = ::getInt(param2.GetValueInt8());
					break;
				case CalvinValue::UInt8Type:
					strValue1 = ::getInt(param1.GetValueUInt8());
					strValue2 = ::getInt(param2.GetValueUInt8());
					break;
				case CalvinValue::Int16Type:
					strValue1 = ::getInt(param1.GetValueInt16());
					strValue2 = ::getInt(param2.GetValueInt16());
					break;
				case CalvinValue::UInt16Type:
					strValue1 = ::getInt(param1.GetValueUInt16());
					strValue2 = ::getInt(param2.GetValueUInt16());
					break;
				case CalvinValue::Int32Type:
					strValue1 = ::getInt(param1.GetValueInt32());
					strValue2 = ::getInt(param2.GetValueInt32());
					break;
				case CalvinValue::UInt32Type:
					strValue1 = ::getUnsignedInt(param1.GetValueUInt32());
					strValue2 = ::getUnsignedInt(param2.GetValueUInt32());
					break;
				case CalvinValue::FloatType:
					fFraction_param = fFraction;
					strValue1 = ::getDouble(param1.GetValueFloat(), 10);
					strValue2 = ::getDouble(param2.GetValueFloat(), 10);
					break;
				case CalvinValue::AsciiType:
					strValue1 = param1.GetValueAscii();
					strValue2 = param2.GetValueAscii();
					break;
				case CalvinValue::TextType:
					strValue1 = StringUtils::ConvertWCSToMBS(param1.GetValueText());
					strValue2 = StringUtils::ConvertWCSToMBS(param2.GetValueText());
					break;
				default:
					break;
				}
				paramCalvin1.set(StringUtils::ConvertWCSToMBS(param1.GetName()), (CalvinValue::ParameterType)param1.GetParameterType(), strValue1);
				paramCalvin2.set(StringUtils::ConvertWCSToMBS(param2.GetName()), (CalvinValue::ParameterType)param2.GetParameterType(), strValue2);
				if (!equivalentParameter(strPrompt, paramCalvin1, paramCalvin2, setIgnore, mapEpsilon, fEpsilon, fFraction_param)) {return false;}
			}
		}
		if (!bFound)
		{
			Verbose::out(1, strPrompt + "Header Parameter name not found: " + StringUtils::ConvertWCSToMBS(param1.GetName()));
			return false;
		}
	}
	return bEquivalent;
}
