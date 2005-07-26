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

#if !defined(AFX_CELFILEDATA_H__26426988_54DA_4A5D_A266_0901CF50490A__INCLUDED_)
#define AFX_CELFILEDATA_H__26426988_54DA_4A5D_A266_0901CF50490A__INCLUDED_

//////////////////////////////////////////////////////////////////////

#include <fstream>
#include <string>
#include <list>
#include <map>

#ifdef WIN32
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

namespace affxcel
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

//////////////////////////////////////////////////////////////////////

typedef struct _CoordinatePoint
{ 
  int x; 
  int y; 
} CoordinatePoint; 

typedef struct _GridCoordinatesType
{
    CoordinatePoint upperleft;
    CoordinatePoint upperright;
    CoordinatePoint lowerright;
    CoordinatePoint lowerleft;
} GridCoordinatesType;

//////////////////////////////////////////////////////////////////////

class CCELFileHeaderData
{
public:
	CCELFileHeaderData();
	~CCELFileHeaderData();

protected:
	int m_Magic;
	int m_Version;
	int m_nCols;
	int m_nRows;
	int m_nCells;
	std::string m_Header;
	std::string m_Alg;
	std::string m_Params;
	std::string m_ChipType;
	int m_Margin;
	int m_nOutliers;
	int m_nMasked;
	TagValuePairTypeList m_Parameters; // extra parameters - not filled by the Read function.
	GridCoordinatesType m_CellGrid;

	void SetChipType();
	bool ReadHeader(char *pData);
	void ParseCorners();
	void Clear();

	friend class CCELFileData;

public:
	// Accessors for header information.
	int GetVersion() const { return m_Version; }
	int GetCols() const { return m_nCols; }
	int GetRows() const { return m_nRows; }
	int GetNumCells() const { return m_nCells; }
	std::string GetHeader() const { return m_Header; }
	std::string GetAlg() const { return m_Alg; }
	std::string GetParams() const { return m_Params; }
	std::string GetChipType() const { return m_ChipType; }
	int GetCellMargin() const { return m_Margin; }
	int GetNumOutliers() const { return m_nOutliers; }
	int GetNumMasked() const { return m_nMasked; }
	TagValuePairTypeList &GetParameters() { return m_Parameters; }
	GridCoordinatesType &GridCorners() { return m_CellGrid; }

	// To set data
	void AddAlgorithmParameter(const char *tag, const char *value);
	void SetGridCorners(GridCoordinatesType &grid);
	void SetDimensions(int rows, int cols);
	void SetAlgorithmName(const char *str) { m_Alg = str; }
	void SetChipType(const char *str) { m_ChipType = str; }
	void SetMargin(int margin) { m_Margin = margin; }
	void SetHeader(const char *str) { m_Header = str; }
};

//////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
typedef struct _CELFileEntryType
{
	float Intensity;
	float Stdv;
	short Pixels;
} CELFileEntryType;
#pragma pack(pop)

////////////////////////////////////////////////////////////////////

class CCELFileData  
{
public:
	// Constructor/destructor
	CCELFileData();
	~CCELFileData();

protected:
	bool m_bIncludeMaskAndOutliers;
	std::string m_strError;
	std::string m_FullPathName;
	std::string m_LibPath;
	std::string m_DataPath;
	std::string m_FileName;
	CCELFileHeaderData m_HeaderData;
	CELFileEntryType *m_pEntries;
	std::map<int, char> m_Outliers;
	std::map<int, char> m_Masked;

	bool ReadFile(bool bReadHeaderOnly);
	bool ReadTextFormat(bool bReadHeaderOnly);
	bool ReadXDAFormat(bool bReadHeaderOnly);
	bool ReadXDAHeader(std::ifstream &instr);
	void ReadXDAFooter(std::ifstream &instr);

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

public:
	// File name parameters.
	void SetDataPath(const char *str) { m_DataPath = str; }
	void SetLibPath(const char *str) { m_LibPath = str; }
	void SetFileName(const char *str);
	std::string GetFileName() const { return m_FileName; }
	std::string GetFilePath() const { return m_FullPathName; }

	// Error.
	std::string GetError() const { return m_strError; }

	// Header accessor
	CCELFileHeaderData &GetHeader() { return m_HeaderData; }

	// Accessors for intensity information.
	void GetEntry(int index, CELFileEntryType &entry);
	void GetEntry(int x, int y, CELFileEntryType &entry) { GetEntry(XYToIndex(x,y),entry); }
	float GetIntensity(int index);
	float GetIntensity(int x, int y) { return GetIntensity(XYToIndex(x,y)); }
	float GetStdv(int index);
	float GetStdv(int x, int y) { return GetStdv(XYToIndex(x,y)); }
	short GetPixels(int index);
	short GetPixels(int x, int y) { return GetPixels(XYToIndex(x,y)); }

	// Accessors for the mask/outlier flags
	bool IsOutlier(int x, int y) { return IsOutlier(XYToIndex(x,y)); }
	bool IsOutlier(int index);
	bool IsMasked(int x, int y) { return IsMasked(XYToIndex(x,y)); }
	bool IsMasked(int index);

	// Index/position conversions
	int IndexToX(int index) { return index % m_HeaderData.m_nCols; }
	int IndexToY(int index) { return index / m_HeaderData.m_nCols; }
	int XYToIndex(int x, int y) { return y*m_HeaderData.m_nCols + x; }

	// Sets the data values
	void SetCellEntry(int x, int y, CELFileEntryType *pEntry);
	void SetCellEntry(int index, CELFileEntryType *pEntry);
	void AllocateEntries();
	void SetMask(int x, int y, bool mask) { SetMask(XYToIndex(x,y), mask); }
	void SetMask(int index, bool mask);
	void SetOutlier(int x, int y, bool outlier) { SetOutlier(XYToIndex(x,y), outlier); }
	void SetOutlier(int index, bool outlier);

	// For reading a file.
	bool Exists();
	bool ReadHeader();
	bool Read(bool bIncludeMaskAndOutliers=true);
	void Close();
	bool IsXDACompatibleFile();
	void Clear();
};

//////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CELFILEDATA_H__26426988_54DA_4A5D_A266_0901CF50490A__INCLUDED_)
