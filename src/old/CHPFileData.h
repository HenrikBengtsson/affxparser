/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#if !defined(AFX_CHPFILEDATA_H__A16FCA8B_7B8D_4071_81CD_F6B2BB61A169__INCLUDED_)
#define AFX_CHPFILEDATA_H__A16FCA8B_7B8D_4071_81CD_F6B2BB61A169__INCLUDED_

//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <list>
#include <fstream>

//////////////////////////////////////////////////////////////////////

namespace affxchp
{

//////////////////////////////////////////////////////////////////////

typedef struct _TagValuePairType
{
	std::string Tag;
	std::string Value;
	_TagValuePairType operator=(_TagValuePairType vp) { Tag = vp.Tag; Value = vp.Value; return *this; }
	bool operator==(_TagValuePairType vp) { if (vp.Tag == Tag) return true; return false; }
	bool operator==(const char *tag) { if (Tag == tag) return true; return false; }
} TagValuePairType;

typedef std::list<TagValuePairType> TagValuePairTypeList;

typedef struct _BackgroundZoneType
{
	float centerx;
	float centery;
	float background;
	_BackgroundZoneType operator=(_BackgroundZoneType zn)
	{
		centerx = zn.centerx; 
		centery = zn.centery; 
		background = zn.background; 
		return *this; 
	}
} BackgroundZoneType;
#define ZONE_INFO_TYPE_SIZE (3*sizeof(float))
typedef std::list<BackgroundZoneType> BackgroundZoneTypeList;

typedef struct _BackgroundZoneInfo
{
	int number_zones;
	float smooth_factor;
	BackgroundZoneTypeList zones;
} BackgroundZoneInfo;

////////////////////////////////////////////////////////////////////

class CCHPFileHeader
{
public:
	CCHPFileHeader();
	~CCHPFileHeader();

public:
	typedef enum {Expression, Genotyping, Resequencing, Universal, Unknown} GeneChipAssayType;

protected:
	int m_Magic;
	int m_Version;
	unsigned short m_Cols;
	unsigned short m_Rows;
	int m_NumProbeSets;
	GeneChipAssayType m_AssayType;
	std::string m_ChipType;
	std::string m_AlgorithmName;
	std::string m_AlgorithmVersion;
	std::string m_ParentCellFile;
	std::string m_ProgID;
	TagValuePairTypeList m_AlgorithmParameters;
	TagValuePairTypeList m_SummaryParameters;
	BackgroundZoneInfo m_BackgroundZoneInfo;

	void ParseString(TagValuePairTypeList &tagList,
					 std::string strSource,
					 std::string sDelimiter1, 
					 std::string sDelimiter2);

	void Clear();

	friend class CCHPFileData;

public:
	int GetCols() const { return m_Cols; }
	int GetRows() const { return m_Rows; }
	int GetNumProbeSets() const { return m_NumProbeSets; }
	GeneChipAssayType GetAssayType() const { return m_AssayType; }
	std::string GetChipType() const { return m_ChipType; }
	std::string GetAlgName() const { return m_AlgorithmName; }
	std::string GetAlgVersion() const { return m_AlgorithmVersion; }
	TagValuePairTypeList &AlgorithmParameters() { return m_AlgorithmParameters; }
	TagValuePairTypeList &SummaryParameters() { return m_SummaryParameters; }
	std::string GetParentCellFile() const { return m_ParentCellFile; }
	std::string GetProgID() const { return m_ProgID; }
	std::string GetAlgorithmParameter(const char *tag);
	std::string GetSummaryParameter(const char *tag);
	BackgroundZoneInfo &GetBackgroundZoneInfo() { return m_BackgroundZoneInfo; }
	BackgroundZoneTypeList &GetBackgroundZones() { return m_BackgroundZoneInfo.zones; }
	BackgroundZoneType GetBackgroundZone(int x, int y);

	int GetMagicNumber() const { return m_Magic; }
	int GetVersionNumber() const { return m_Version; }

	void SetCols(int n) { m_Cols=n; }
	void SetRows(int n) { m_Rows=n; }
	void SetNumProbeSets(int n) { m_NumProbeSets=n; }
	void SetAssayType(GeneChipAssayType t) { m_AssayType=t; }
	void SetChipType(const char *s) { m_ChipType=s; }
	void SetAlgName(const char *s) { m_AlgorithmName=s; }
	void SetAlgVersion(const char *s) { m_AlgorithmVersion=s; }
	void SetParentCellFile(const char *s) { m_ParentCellFile=s; }
	void SetProgID(const char *s) { m_ProgID=s; }
};

////////////////////////////////////////////////////////////////////

class CProbeSetResults
{
public:
	CProbeSetResults() {};
	virtual ~CProbeSetResults() {};
};

////////////////////////////////////////////////////////////////////

#define ABS_PRESENT_CALL 0
#define ABS_MARGINAL_CALL 1
#define ABS_ABSENT_CALL 2
#define ABS_NO_CALL 3

#define COMP_INCREASE_CALL 1
#define COMP_DECREASE_CALL 2
#define COMP_MOD_INCREASE_CALL 3
#define COMP_MOD_DECREASE_CALL 4
#define COMP_NO_CHANGE_CALL 5
#define COMP_NO_CALL 6

class CExpressionProbeSetResults : public CProbeSetResults
{
public:
	float DetectionPValue;
	float Signal; 
	unsigned short NumPairs;
	unsigned short NumUsedPairs;
	unsigned char Detection;

	bool m_HasCompResults;
	float ChangePValue; 
	float SignalLogRatio; 
	float SignalLogRatioLow; 
	float SignalLogRatioHigh; 
	unsigned short NumCommonPairs;
	unsigned char Change;

	std::string GetDetectionString();
	std::string GetChangeString();

	CExpressionProbeSetResults operator=(CExpressionProbeSetResults &src);

	CExpressionProbeSetResults() { m_HasCompResults = false; }
	~CExpressionProbeSetResults() {}
};

////////////////////////////////////////////////////////////////////

#define ALLELE_A_CALL 6
#define ALLELE_B_CALL 7
#define ALLELE_AB_CALL 8
#define ALLELE_NO_CALL 11

class CGenotypeProbeSetResults : public CProbeSetResults
{
public:
	unsigned char AlleleCall;
	float Confidence;
	float RAS1;
	float RAS2;
	float pvalue_AA;
	float pvalue_AB;
	float pvalue_BB;
	float pvalue_NoCall;

	std::string GetAlleleCallString();

	CGenotypeProbeSetResults operator=(CGenotypeProbeSetResults &src);

	CGenotypeProbeSetResults() {Confidence=RAS1=RAS2=pvalue_AA=pvalue_AB=pvalue_BB=pvalue_NoCall=0;}
	~CGenotypeProbeSetResults() {}
};

////////////////////////////////////////////////////////////////////

class CCHPFileData  
{
public:
	// Constructor/destructor
	CCHPFileData();
	~CCHPFileData();

protected:
	CCHPFileHeader m_Header;
	std::string m_FileName;
	std::string m_FullPathName;
	std::string m_DataPath;
	std::string m_strError;
	std::vector<CProbeSetResults *> m_ProbeSetResults;

	bool Open(bool bReadHeaderOnly=false);

public:
	// Accessors to header and data.
	CCHPFileHeader &GetHeader() { return m_Header; }
	
	CExpressionProbeSetResults *GetExpressionResults(int index);	
	CGenotypeProbeSetResults *GetGenotypingResults(int index);

	// Error string
	std::string GetError() const { return m_strError; }

	// Functions to read file.
	bool Read();
	bool ReadHeader();
	bool Exists();
	bool IsXDACompatibleFile();

	// File name and paths.
	void SetFileName(const char *name);
	void SetDataPath(const char *path) { m_DataPath = path; }
	std::string GetFileName() const { return m_FileName; }
	std::string GetFilePath() const { return m_FullPathName; }

	void Clear();
};

////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CHPFILEDATA_H__A16FCA8B_7B8D_4071_81CD_F6B2BB61A169__INCLUDED_)
