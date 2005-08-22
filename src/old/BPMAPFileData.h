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

#if !defined(AFX_BPMAPFILEDATA_H__601D7E2E_5897_4CAA_ABF0_D6002698CB78__INCLUDED_)
#define AFX_BPMAPFILEDATA_H__601D7E2E_5897_4CAA_ABF0_D6002698CB78__INCLUDED_

//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <list>

//////////////////////////////////////////////////////////////////////

namespace affxbpmap
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

typedef std::vector<TagValuePairType> TagValuePairTypeVector;

////////////////////////////////////////////////////////////////////

typedef struct _GDACSequenceHitItemType
{
	unsigned int	PMX;
	unsigned int	PMY;
	unsigned int	MMX;
	unsigned int	MMY;
	float			MatchScore;
	unsigned int	Position;
	std::string		PMProbe;
	unsigned char	ProbeLength;
	unsigned char	TopStrand;	// 1 = top, 0 = bottom
} GDACSequenceHitItemType;

#define PROBE_STORAGE_BUFFER_LENGTH 7
#define HIT_ITEM_SIZE (4+4+4+4+1+PROBE_STORAGE_BUFFER_LENGTH+4+4+1)

//////////////////////////////////////////////////////////////////////

class CGDACSequenceItem
{
public:
	CGDACSequenceItem();
	~CGDACSequenceItem();

protected:
	std::string m_Name;
	std::string m_GroupName;
	std::string m_SeqVersion;
	int m_Number;
	int m_NumberHits;
	std::vector<GDACSequenceHitItemType> m_Hits;
	std::vector<GDACSequenceHitItemType> *m_pHits;
	TagValuePairTypeVector m_Parameters;

	bool m_bMapped;
	char *m_lpData;
	int m_HitStartPosition;

	void MakeShallowCopy(CGDACSequenceItem &orig);

public:
	std::string GetName() const { return m_Name; }
	std::string GroupName() const { return m_GroupName; }
	std::string GetSeqVersion() const { return m_SeqVersion; }
	int GetNumber() const { return m_Number; }
	int GetNumberHits() const { return m_NumberHits; }
	int GetNumberParameters() const { return (int)m_Parameters.size(); }
	TagValuePairType &GetParameter(int index) { return m_Parameters[index]; }

	void GetHitItem(int index, GDACSequenceHitItemType &hit, bool readProbe=false);

	friend class CBPMAPFileData;
};

//////////////////////////////////////////////////////////////////////

class CBPMAPFileData
{
public:
	CBPMAPFileData();
	~CBPMAPFileData();

protected:
	std::string m_FileName;
	std::string m_LibPath;
	std::string m_FullPathName;
	int	 m_NumberSequences;
	float m_Version;
	int m_DataStartPosition;

	std::string m_strError;

	std::vector<CGDACSequenceItem> m_SequenceItems;

	bool ReadFile(bool bReadHeaderOnly=false);
	bool ReadHeaderSection();
	bool ReadDataSection();

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
	std::string GetFileName() const { return m_FileName; }
	void SetFileName (const char *name);
	void SetLibPath (const char *path) { m_LibPath = path; }

	// Functions for reading a file.
	bool Read();
	bool ReadHeader();
	bool Exists();
	void Close();

	// Error string
	std::string GetError() const { return m_strError; }

	// Data accessors
	int GetNumberSequences() const { return m_NumberSequences; }
	float GetVersion() const { return m_Version; }

	// Data accessors.
	void GetSequenceItem(int index, CGDACSequenceItem &seq);
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BPMAPFILEDATA_H__601D7E2E_5897_4CAA_ABF0_D6002698CB78__INCLUDED_)
