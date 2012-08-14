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

#ifndef _Calvin_H_
#define _Calvin_H_
/**
 * @file Calvin.h
 *
 * @brief This header contains the Calvin class definition. 
 */

#include "calvin_files/data/src/DataGroupHeader.h"
#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/utils/src/StringUtils.h"
#include "calvin_files/writers/src/GenericFileWriter.h"

//#include "chipstream/BioTypes.h" - not needed to build lib project
#include "file/CHPFileData.h"
#include "file/TsvFile/TsvFile.h"
#include "util/AffxArray.h"
#include "util/AffxString.h"
//
#include <set>
//
class CalvinValue
{
public:
	/*! Enumerant of the built-in parameter types. */
	enum ParameterType { 
			  Int8Type,			/*! an 8 bit integer. */
			  UInt8Type, 		/*! an 8 bit unsigned integer. */
			  Int16Type,		/*! a 16 bit integer. */
			  UInt16Type,		/*! a 16 bit unsigned integer. */
			  Int32Type,		/*! a 32 bit integer. */
			  UInt32Type,		/*! a 32 bit unsigned integer. */
			  FloatType,		/*! a 32 bit floating point. */
			  TextType,			/*! a 16 bit character. */
			  AsciiType,		/*! an 8 bit character. */
			  UnknownType		/*! an 8 bit integer. */
	};

protected:
	ParameterType m_eParameterType;
	AffxString m_strValue;

public:
	CalvinValue() {clear();}
	CalvinValue(ParameterType eParameterType, const AffxString& strValue) 
	{
		m_eParameterType = eParameterType;
		m_strValue = strValue;
	}
	ParameterType getParameterType() {return m_eParameterType;}
	AffxString& getValue() {return m_strValue;}
	void clear() {m_eParameterType = UnknownType; m_strValue.clear();}
	
	/** 
	 * Compare function.
	 * @param that - A reference to an instance of this class.
	 * @param iCompareCode - The code to switch on when doing compares. (Each code is a different compare.)
	 * @return - int value. (-1 if *this < that, 0 if *this == that, 1 if *this > that)
	*/
	int compareTo(CalvinValue& that, int iCompareCode)
	{
		int iCompareResult = 0;
		switch (iCompareCode)
		{
		case 0:
			switch (m_eParameterType)
			{
			case TextType:
//				iCompareResult = StringUtils::ConvertWCSToMBS(m_strValue).compareTo(StringUtils::ConvertWCSToMBS(that.m_strValue), iCompareCode);
			case AsciiType:
				iCompareResult = m_strValue.compareTo(that.m_strValue, iCompareCode);
			case FloatType:
				iCompareResult = AffxArray<float>::compare((float)::getDouble(m_strValue), (float)::getDouble(that.m_strValue));
			case UInt32Type:
				iCompareResult = AffxArray<unsigned int>::compare(::getUnsignedInt(m_strValue), ::getUnsignedInt(that.m_strValue));
			default:
				iCompareResult = AffxArray<int>::compare(::getInt(m_strValue), ::getInt(that.m_strValue));
			}
			break;
		}
		return iCompareResult;
	}
};

class CalvinParameter : public CalvinValue
{
protected:
	AffxString m_strName;

public:
	CalvinParameter()
	{
	}

	CalvinParameter(const AffxString& strName, CalvinValue::ParameterType eParameterType, const AffxString& strValue) : CalvinValue(eParameterType, strValue) 
	{
		m_strName = strName;
	}

	void set(const AffxString& strName, CalvinValue::ParameterType eParameterType, const AffxString& strValue)
	{
		m_strName = strName;
		m_eParameterType = eParameterType;
		m_strValue = strValue;
	}

	AffxString& getName() {return m_strName;}

	void clear() {m_strName.clear(); CalvinValue::clear();}

	/** 
	 * Compare function.
	 * @param that - A reference to an instance of this class.
	 * @param iCompareCode - The code to switch on when doing compares. (Each code is a different compare.)
	 * @return - int value. (-1 if *this < that, 0 if *this == that, 1 if *this > that)
	*/
	int compareTo(CalvinParameter& that, int iCompareCode)
	{
		int iCompareResult = 0;
		switch (iCompareCode)
		{
		case 0:
			iCompareResult = m_strName.compareTo(that.m_strName, 0);
//			if (iCompareResult == 0) {iCompareResult = this->CalvinValue::compareTo(that, 0);}
			break;
		}
		return iCompareResult;
	}

    template<int k> struct ComparePred {
        bool operator()(const CalvinParameter* lhs, const CalvinParameter* rhs) const {
            Err::errAbort("CalvinParameter: ComparePred instantiated with an invalid compare code = " + ToStr(k));
            return false;
        }
    };
};

template<> struct CalvinParameter::ComparePred<0> {
    bool operator()(const CalvinParameter* lhs, const CalvinParameter* rhs) const {
        return lhs->m_strName.compareTo(rhs->m_strName, 0) < 0;
    }
};

/**
 * @brief  A class for storing Calvin header data. 
 * 
 */
class CalvinHeader : public AffxArray<CalvinParameter>
{
public:
	CalvinHeader()
	{
	}
	virtual ~CalvinHeader()
	{
		clear();
	}

	void clear() 
	{
		deleteAll();
	}
};

class CalvinDataSet
{
protected:
	AffxString m_strName;
	AffxArray<AffxString> m_vColumnHeaders;
	AffxArray<AffxArray<CalvinValue> > m_mxCalvinValues;

public:
	CalvinDataSet()
	{
	}

	virtual ~CalvinDataSet() 
	{
		clear();
	}
	
	void clear() 
	{
		m_vColumnHeaders.deleteAll();
		for (int iIndex = 0; (iIndex < m_mxCalvinValues.getCount()); iIndex++)
		{
			m_mxCalvinValues.getAt(iIndex)->deleteAll();
		}
		m_mxCalvinValues.deleteAll();
	}
	
	void setName(const AffxString& str) {m_strName = str;}
	AffxString& getName() {return m_strName;}
	AffxArray<AffxString>& getColumnHeaders() {return m_vColumnHeaders;}
	AffxArray<AffxArray<CalvinValue> >& getCalvinValues() {return m_mxCalvinValues;}
	
	/** 
	 * Compare function.
	 * @param that - A reference to an instance of this class.
	 * @param iCompareCode - The code to switch on when doing compares. (Each code is a different compare.)
	 * @return - int value. (-1 if *this < that, 0 if *this == that, 1 if *this > that)
	*/
	int compareTo(CalvinDataSet& that, int iCompareCode)
	{
		int iCompareResult = 0;
		switch (iCompareCode)
		{
		case 0:
			iCompareResult = m_strName.compareTo(that.m_strName, 0);
			break;
		}
		return iCompareResult;
	}
};

class CalvinDataGroup
{
protected:
	AffxString m_strName;
	AffxArray<CalvinDataSet> m_vCalvinDataSets;

public:
	CalvinDataGroup()
	{
	}
	virtual ~CalvinDataGroup()
	{
		clear();
	}
	
	void clear() {m_vCalvinDataSets.deleteAll();}

	void setName(const AffxString& str) {m_strName = str;}
	AffxString& getName() {return m_strName;}
	AffxArray<CalvinDataSet>& getCalvinDataSets() {return m_vCalvinDataSets;}

	/** 
	 * Compare function.
	 * @param that - A reference to an instance of this class.
	 * @param iCompareCode - The code to switch on when doing compares. (Each code is a different compare.)
	 * @return - int value. (-1 if *this < that, 0 if *this == that, 1 if *this > that)
	*/
	int compareTo(CalvinDataGroup& that, int iCompareCode)
	{
		int iCompareResult = 0;
		switch (iCompareCode)
		{
		case 0:
			iCompareResult = m_strName.compareTo(that.m_strName, 0);
			break;
		}
		return iCompareResult;
	}
};

class Calvin
{
protected:
	AffxString m_strFileName;
	CalvinHeader m_objCalvinHeader;
	AffxArray<CalvinHeader> m_vParentCalvinHeaders;
	AffxArray<CalvinDataGroup> m_vCalvinDataGroups;

public:
	static void test();

	Calvin()
	{
	}

	virtual ~Calvin()
	{
		clear();
	}

	void clear() {m_objCalvinHeader.clear(); m_vParentCalvinHeaders.deleteAll(); m_vCalvinDataGroups.deleteAll();}

	AffxString& getFileName() {return m_strFileName;}
	CalvinHeader& getCalvinHeader() {return m_objCalvinHeader;}
	AffxArray<CalvinHeader>& getParentCalvinHeaders() {return m_vParentCalvinHeaders;}
	AffxArray<CalvinDataGroup>& getCalvinDataGroups() {return m_vCalvinDataGroups;}
	
	bool load(const AffxString& strFileName);
	bool equivalent(Calvin& that, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon = 0.0001, bool bCheckHeader = true, float fFraction = 0.0);
        static bool equivalent(	const AffxString& strFileName1,
                                const AffxString& strFileName2,
                                std::set<std::string>& setIgnore,
                                std::set<std::string>& setSetIgnore,
                                std::map<std::string, float>& mapEpsilon,
				float fEpsilon = 0.0001,
                                double dCorrelationCutoff = 1.0,
                                bool bCheckHeader = true,
                                int iMessageLimit = 1000,
                                float fFraction=0.0);

	static bool equivalent(const AffxString& strFileName1, const AffxString& strFileName2, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon = 0.0001, double dCorrelationCutoff = 1.0, bool bCheckHeader = true, int iMessageLimit = 1000, float fFraction = 0.0);

protected:
	void loadHeader(affymetrix_calvin_io::GenericDataHeader& Header, CalvinHeader& header);

	bool equivalentHeader(const AffxString& strPrompt, CalvinHeader& headerThis, CalvinHeader& headerThat, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon, float fFraction = 0.0);
	static bool equivalentParameter(const AffxString& strPrompt, CalvinParameter& This, CalvinParameter& That, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon, float fFraction = 0.0);
	static bool equivalentHeader(const AffxString& strPrompt, affymetrix_calvin_io::GenericDataHeader& Header1, affymetrix_calvin_io::GenericDataHeader& Header2, std::set<std::string>& setIgnore, std::map<std::string, float>& mapEpsilon, float fEpsilon, float fFraction = 0.0);

};


#endif


