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

#if !defined(AFX_BARFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
#define AFX_BARFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_

//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <list>
#include <fstream>

//////////////////////////////////////////////////////////////////////

namespace affxbar
{

//////////////////////////////////////////////////////////////////////

typedef enum _GDACFILES_BAR_DATA_TYPE
{
	BAR_DATA_DOUBLE,
	BAR_DATA_FLOAT,
	BAR_DATA_INTEGER,
	BAR_DATA_SHORT,
	BAR_DATA_CHAR,
	BAR_DATA_UINTEGER,
	BAR_DATA_USHORT,
	BAR_DATA_UCHAR
} GDACFILES_BAR_DATA_TYPE;

typedef std::vector<GDACFILES_BAR_DATA_TYPE> GDACFILES_BAR_DATA_TYPE_VECTOR;

//////////////////////////////////////////////////////////////////////

typedef union _BarSequenceResultData
{
	double dValue;
	float fValue;
	int iValue;
	short sValue;
	char cValue;
	unsigned int uiValue;
	unsigned short usValue;
	unsigned char ucValue;
} BarSequenceResultData;

//////////////////////////////////////////////////////////////////////

typedef struct _TagValuePairType
{
	std::string Tag;
	std::string Value;
	_TagValuePairType operator=(_TagValuePairType vp) { Tag = vp.Tag; Value = vp.Value; return *this; }
	bool operator==(_TagValuePairType vp) { if (vp.Tag == Tag) return true; return false; }
	bool operator==(const char *tag) { if (Tag == tag) return true; return false; }
} TagValuePairType;

typedef std::vector<TagValuePairType> TagValuePairTypeVector;

//////////////////////////////////////////////////////////////////////

class CGDACSequenceResultItem
{
public:
	CGDACSequenceResultItem();
	~CGDACSequenceResultItem();

protected:
	std::string m_Name;
	std::string m_Version;
	std::string m_GroupName;
	int m_NumberDataPoints;
	int m_NumberColumns;
	int	m_NumberParameters;
	GDACFILES_BAR_DATA_TYPE_VECTOR *m_pColumnTypes;
	TagValuePairTypeVector m_Parameters;
	BarSequenceResultData **m_ppData;
	BarSequenceResultData ***m_pppData;

	bool m_bMapped;
	char *m_lpData;
	int m_DataStartPosition;

	void MakeShallowCopy(CGDACSequenceResultItem &orig);

public:
	std::string GetName() const { return m_Name; }
	std::string GetVersion() const { return m_Version; }
	std::string GetGroupName() const { return m_GroupName; }
	int GetNumberDataPoints() const { return m_NumberDataPoints; }
	int GetNumberColumns() const { return m_NumberColumns; }
	GDACFILES_BAR_DATA_TYPE GetColumnType(int index) const { return (*m_pColumnTypes)[index]; }
	void GetData(int iData, int iCol, BarSequenceResultData &data );
	int GetNumberParameters() const { return m_NumberParameters; }
	TagValuePairType &GetParameter(int index) { return m_Parameters[index]; }

	void SetName(const char *name) { m_Name = name; }
	void SetVersion(const char *ver) { m_Version = ver; }
	void SetGroupName(const char *group) { m_GroupName = group; }
	void SetNumberDataPoints(int n);
	void SetDataPoint(int nIndex, int colIndex, BarSequenceResultData &data);
	void AddParameter(std::string tag, std::string value);

	friend class CBARFileData;
};

////////////////////////////////////////////////////////////////////

class CBARFileData
{
public:
	CBARFileData();
	~CBARFileData();

protected:
	std::string m_FileName;
	std::string m_DataPath;
	std::string m_LibPath;
	std::string m_FullPathName;
	float	m_Version;
	int		m_NumberSequences;
	int		m_NumberColumns;
	int		m_NumberParameters;
	int m_DataStartPosition;
	TagValuePairTypeVector m_Parameters;
	GDACFILES_BAR_DATA_TYPE_VECTOR m_ColumnTypes;
	std::vector<CGDACSequenceResultItem> m_Results;

	std::string m_strError;

	bool ReadFile(bool bReadHeaderOnly=false);
	bool ReadHeaderSection();
	bool ReadDataSection();

	int GetDataRowSize();

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
	// File name and paths.
	void SetFileName (const char *name);
	void SetLibPath (const char *path) { m_LibPath = path; }
	void SetDataPath (const char *path) { m_DataPath = path; }

	// Functions for reading a file.
	bool Read();
	bool ReadHeader();
	bool Exists();
	void Close();

	// Error string
	std::string GetError() const { return m_strError; }

	// Header accessors
	float GetVersion() const { return m_Version; }
	int GetNumberSequences() const { return m_NumberSequences; }
	int	GetNumberColumns() const { return m_NumberColumns; }
	int GetNumberParameters() const { return m_NumberParameters; }
	TagValuePairType &GetParameter(int index) { return m_Parameters[index]; }
	GDACFILES_BAR_DATA_TYPE &GetColumnTypes(int index) { return m_ColumnTypes[index]; }

	// Data accessors.
	void GetResults(int index, CGDACSequenceResultItem &seq);

	// Functions to add data.
	void AddAlgorithmParameter(const char *tag, const char *value);
	void AddColumn(GDACFILES_BAR_DATA_TYPE ctype);
	void SetNumberSequences(int n);
	CGDACSequenceResultItem *GetResultsPtr(int index) { return &m_Results[index]; }
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BARFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
