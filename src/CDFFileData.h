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

#if !defined(AFX_CDFFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
#define AFX_CDFFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_

//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////

namespace affxcdf
{

//////////////////////////////////////////////////////////////////////

#define MAX_PROBE_SET_NAME_LENGTH 64

////////////////////////////////////////////////////////////////////


class CCDFFileHeader
{
protected:
	int m_Magic;
	int m_Version;
	int m_NumProbeSets;
	int m_NumQCProbeSets;
	std::string m_Reference;
	unsigned short m_Cols;
	unsigned short m_Rows;

	friend class CCDFFileData;

public:
	int GetCols() const { return m_Cols; }
	int GetRows() const { return m_Rows; }
	int GetNumProbeSets() const { return m_NumProbeSets; }
	int GetNumQCProbeSets() const { return m_NumQCProbeSets; }
	std::string &GetReference() { return m_Reference; }

	CCDFFileHeader();
};

////////////////////////////////////////////////////////////////////

enum GeneChipProbeSetType
{
	UnknownProbeSetType,
	ExpressionProbeSetType,
	GenotypingProbeSetType,
	ResequencingProbeSetType,
	TagProbeSetType
};

////////////////////////////////////////////////////////////////////

enum DirectionType
{
	NoDirection,
	SenseDirection,
	AntiSenseDirection
};

////////////////////////////////////////////////////////////////////

class CCDFProbeInformation
{
protected:
	int m_ListIndex;
	int m_Expos;
	unsigned short m_X;
	unsigned short m_Y;
	char m_PBase;
	char m_TBase;

	friend class CCDFProbeGroupInformation;
	friend class CCDFProbeSetInformation;
	friend class CCDFFileData;

public:
	int GetListIndex() const { return m_ListIndex; }
	int GetExpos() const { return m_Expos; }
	int GetX() const { return m_X; }
	int GetY() const { return m_Y; }
	char GetPBase() const { return m_PBase; }
	char GetTBase() const { return m_TBase; }

	CCDFProbeInformation();
};
#define PROBE_SIZE (4+4+2+2+1+1)	// This is the size of the object in a binary CDF file

////////////////////////////////////////////////////////////////////

class CCDFProbeGroupInformation
{
protected:
	int m_NumLists;
	int m_NumCells;
	int m_Start;
	int m_Stop;
	int m_ProbeSetIndex;
	int m_GroupIndex;
	std::string m_Name;
	unsigned char m_NumCellsPerList;
	unsigned char m_Direction;
	std::vector<CCDFProbeInformation> m_Cells;
	std::vector<CCDFProbeInformation> *m_pCells;

	bool m_bMapped;
	char *m_lpData;

	friend class CCDFProbeSetInformation;
	friend class CCDFFileData;

	void MakeShallowCopy(CCDFProbeGroupInformation &orig);

public:
	DirectionType GetDirection() const { return (DirectionType)m_Direction; }
	int GetNumLists() const { return m_NumLists; }
	int GetNumCells() const { return m_NumCells; }
	int GetNumCellsPerList() const { return (int)m_NumCellsPerList; }
	int GetStart() const { return m_Start; }
	int GetStop() const { return m_Stop; }
	std::string GetName() { return m_Name; }
	void GetCell(int cell_index, CCDFProbeInformation & info);

	CCDFProbeGroupInformation();
	~CCDFProbeGroupInformation();
};
#define PROBE_GROUP_SIZE (MAX_PROBE_SET_NAME_LENGTH+4+4+4+4+1+1)	// This is the size of the object in a binary CDF file

////////////////////////////////////////////////////////////////////

class CCDFProbeSetInformation
{
protected:
	int m_NumLists;
	int m_NumGroups;
	int m_NumCells;
	int m_Index;
	int m_ProbeSetNumber;
	unsigned short m_ProbeSetType;
	unsigned char m_Direction;
	unsigned char m_NumCellsPerList;
	std::vector<CCDFProbeGroupInformation> m_Groups;
	std::vector<CCDFProbeGroupInformation> *m_pGroups;

	bool m_bMapped;
	char *m_lpData;

	friend class CCDFFileData;

	void MakeShallowCopy(CCDFProbeSetInformation &orig);

public:
	GeneChipProbeSetType GetProbeSetType() const { return (GeneChipProbeSetType)m_ProbeSetType; }
	DirectionType GetDirection() const { return (DirectionType)m_Direction; }
	int GetNumLists() const { return m_NumLists; }
	int GetNumGroups() const { return m_NumGroups; }
	int GetNumCells() const { return m_NumCells; }
	int GetNumCellsPerList() const { return (int)m_NumCellsPerList; }
	int GetProbeSetNumber() const { return m_ProbeSetNumber; }
	void GetGroupInformation(int index, CCDFProbeGroupInformation & info);

	CCDFProbeSetInformation();
	~CCDFProbeSetInformation();
};
#define PROBE_SET_SIZE (4+4+4+4+2+1+1)	// This is the size of the object in a binary CDF file

////////////////////////////////////////////////////////////////////

class CCDFProbeSetNames
{
public:
	friend class CCDFFileData;

	CCDFProbeSetNames();
	~CCDFProbeSetNames();

	void Clear();

protected:
	bool m_bMapped;
	char *m_lpData;
	std::vector<std::string> m_ProbeSetNames;

	void Resize(int size);
	void SetName(int index, std::string name);

public:
	std::string GetName(int index) const;
};

////////////////////////////////////////////////////////////////////

class CCDFQCProbeInformation
{
protected:
	unsigned short m_X;
	unsigned short m_Y;
	unsigned char m_PLen;
	unsigned char m_PMProbe;
	unsigned char m_Background;

	friend class CCDFQCProbeSetInformation;
	friend class CCDFFileData;

public:
	int GetX() const { return m_X; }
	int GetY() const { return m_Y; }
	int GetPLen() const { return m_PLen; }
	bool IsPerfectMatchProbe() const { return (m_PMProbe == 1 ? true : false); }
	bool IsBackgroundProbe() const { return (m_Background == 1 ? true : false); }
};
#define QC_PROBE_SIZE (2+2+1+1+1)	// This is the size of the object in a binary CDF file

////////////////////////////////////////////////////////////////////

enum GeneChipQCProbeSetType
{
	UnknownQCProbeSetType,
	CheckerboardNegativeQCProbeSetType,
	CheckerboardPositiveQCProbeSetType,
	HybNegativeQCProbeSetType,
	HybPositiveQCProbeSetType,
	TextFeaturesNegativeQCProbeSetType,
	TextFeaturesPositiveQCProbeSetType,
	CentralNegativeQCProbeSetType,
	CentralPositiveQCProbeSetType,
	GeneExpNegativeQCProbeSetType,
	GeneExpPositiveQCProbeSetType,
	CycleFidelityNegativeQCProbeSetType,
	CycleFidelityPositiveQCProbeSetType,
	CentralCrossNegativeQCProbeSetType,
	CentralCrossPositiveQCProbeSetType,
	CrossHybNegativeQCProbeSetType,
	CrossHybPositiveQCProbeSetType,
	SpatialNormalizationNegativeQCProbeSetType,
	SpatialNormalizationPositiveQCProbeSetType
};

////////////////////////////////////////////////////////////////////

class CCDFQCProbeSetInformation
{
protected:
	int m_NumCells;
	unsigned short m_QCProbeSetType;
	std::vector<CCDFQCProbeInformation> m_Cells;
	std::vector<CCDFQCProbeInformation> *m_pCells;

	bool m_bMapped;
	char *m_lpData;

	friend class CCDFFileData;

	void MakeShallowCopy(CCDFQCProbeSetInformation &orig);

public:
	GeneChipQCProbeSetType GetQCProbeSetType() const { return (GeneChipQCProbeSetType)m_QCProbeSetType; }
	int GetNumCells() const { return m_NumCells; }
	void GetProbeInformation(int index, CCDFQCProbeInformation & info);

	CCDFQCProbeSetInformation();
	~CCDFQCProbeSetInformation();
};
#define QC_PROBE_SET_SIZE (4+2)	// This is the size of the object in a binary CDF file

////////////////////////////////////////////////////////////////////

class CCDFFileData  
{
protected:
	CCDFFileHeader m_Header;
	CCDFProbeSetNames m_ProbeSetNames;
	std::vector<CCDFProbeSetInformation> m_ProbeSets;
	std::vector<CCDFQCProbeSetInformation> m_QCProbeSets;
	std::string m_LibPath;
	std::string m_FileName;
	std::string m_FullPathName;
	std::string m_strError;

	bool Open(bool bReadHeaderOnly=false);
	bool ReadTextFormat(bool bReadHeaderOnly);
	bool ReadXDAFormat(bool bReadHeaderOnly);
	bool ReadXDAHeader(std::ifstream &instr);


	bool ReadXDAFormatUsingMemMap(bool bReadHeaderOnly);

	void  *m_lpFileMap;
	char  *m_lpData;
#ifdef WIN32
	HANDLE m_hFileMap;
	HANDLE m_hFile;
#else
	FILE *m_fp;
	int m_MapLen;
#endif
	bool m_bFileOpen;
	bool m_bFileMapped;

	// JHB added for PM/MM logic.
 	bool m_complementary_logic;

	std::vector<int> m_ProbeSetPositions;
	std::vector<int> m_QCProbeSetPositions;

public:
	// Paths and file name.
	void SetLibPath(const char *str) { m_LibPath = str; }
	void SetFileName(const char *name);

	/** JHB **/
	void SetComplementaryLogic(bool toggle) { m_complementary_logic = toggle; }

	// Accessor functions.
	CCDFFileHeader &GetHeader() { return m_Header; }
	std::string GetError() const { return m_strError; }
	std::string GetProbeSetName(int index) const;
	std::string GetFileName() const { return m_FileName; }
	std::string GetChipType() const;

	// Functions for reading a file.
	bool Read();
	bool ReadHeader();
	bool Exists();
	void Close();

	// Functions to determine new XDA format
	bool IsXDACompatibleFile();

	// ProbeSet accessors.
	GeneChipProbeSetType GetProbeSetType(int index) const;
	void GetProbeSetInformation(int index, CCDFProbeSetInformation & info);
	void GetQCProbeSetInformation(int index, CCDFQCProbeSetInformation & info);
	void GetQCProbeSetInformation(GeneChipQCProbeSetType qcType, CCDFQCProbeSetInformation & info);

	CCDFFileData();
	~CCDFFileData();
};

////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CDFFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
