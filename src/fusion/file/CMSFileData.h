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

#ifndef _CmsFileData_h_
#define _CmsFileData_h_

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

//////////////////////////////////////////////////////////////////////

// Standard template library
#include <cstring>
#include <list>
#include <map>
#include <sstream>
#include <string>
//

//////////////////////////////////////////////////////////////////////

namespace affxcms
{

// File version
#define CMS_FILE_VERSION			1

// Section constants
#define	CMS_SECTION_HEADER			"HEADER"
#define	CMS_SECTION_DATA			"DATA"

// HEADER section key constants
#define	CMS_HEADER_KEY_VERSION		"Version"
#define	CMS_HEADER_KEY_ASSAY		"Assay"
#define	CMS_HEADER_KEY_ARRAYCOUNT	"ArrayCount"
#define	CMS_HEADER_KEY_SNPCOUNT		"SNPCount"

// DATA section key constants
#define	CMS_DATA_TYPES				"Types"
#define	CMS_DATA_SNP				"SNP"
#define CMS_DATA_DEL				"\t"

// Parameter input type
enum CMSInputType
{
	CMS_MAPPING=1
};

typedef struct _SNPTypeInfo
{
	std::string snpID;
	std::map<std::string, int> chiptype_to_unit;
} SNPTypeInfo;
typedef std::list<SNPTypeInfo> SNPTypeInfoList;


//////////////////////////////////////////////////////////////////////
// CMS file header class
//////////////////////////////////////////////////////////////////////
class CCMSFileHeader
{
protected:
	int			m_nVersion;			// File version number
	int			m_nArrayCount;		// Number of array types
	int			m_nSNPCount;		// Number of common SNPs
	std::string	m_strAssay;			// Assay name

public:

	// Get accessors
	int GetVersion() const					{ return m_nVersion; }
	int GetArrayCount() const				{ return m_nArrayCount; }
	int GetSNPCount() const					{ return m_nSNPCount; }
	std::string GetAssay() const			{ return m_strAssay; }

	// Set accessors
	void SetVersion(int value)				{ m_nVersion = value; }
	void SetArrayCount(int value)			{ m_nArrayCount = value; }
	void SetSNPCount(int value)				{ m_nSNPCount = value; }
	void SetAssay(const char *value)		{ m_strAssay = value; }

	// Constructors
	CCMSFileHeader();
};

//////////////////////////////////////////////////////////////////////
// Class: CCMSFileData
// Description: CMS file class
//////////////////////////////////////////////////////////////////////
class CCMSFileData
{
protected:
	CCMSFileHeader	m_Header;

	std::string		m_strFileName;
	std::string		m_strError;

	std::map<int,std::string> m_arrayList;
	SNPTypeInfoList	m_snpList;

	bool Open(bool bReadHeaderOnly=false);
	bool ReadFile(bool bReadHeaderOnly=false);

public:

	// Paths and file name accessors
	void SetFileName(const char *value) { m_strFileName = value; }
	std::string GetFileName() const			{ return m_strFileName; }
	std::string GetError() const			{ return m_strError; }

	// Header accessors
	CCMSFileHeader &GetHeader()				{ return m_Header; }

	// Data accessors
	std::map<int,std::string> &ArrayTypeInformation()	{ return m_arrayList; }
	SNPTypeInfoList &SNPInformation()					{ return m_snpList; }

	// IO methods
	bool Read();
	bool ReadHeader();
	bool Exists();
	void Close();

	// Constructor/Destructor
	CCMSFileData();
	~CCMSFileData();
};

////////////////////////////////////////////////////////////////////

} // namespace

#endif // _CmsFileData_h_
