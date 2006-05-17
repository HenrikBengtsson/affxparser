////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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

#include "PGFFileData.h"
#include "FileIO.h"
#include "FileWriter.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <istream>
#include <stdio.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

using namespace affxpgf;

//////////////////////////////////////////////////////////////////////

#define BUFFER_SIZE 500000

//////////////////////////////////////////////////////////////////////
CPGFFileColumnHeader::~CPGFFileColumnHeader()
{
	RemoveAll();
}

void CPGFFileColumnHeader::RemoveAll()
{
	if (!m_ColumnIndices.empty())
		m_ColumnIndices.clear();
	if (!m_ColumnNames.empty())
		m_ColumnNames.clear();
}

void CPGFFileColumnHeader::Append(const char* str)
{
	assert(str != NULL);
	std::string header = str;
	int index = (int) m_ColumnNames.size();
	m_ColumnNames.push_back(header);
	std::transform(header.begin(), header.end(), header.begin(), tolower);
	m_ColumnIndices.insert(std::make_pair(header, index));
}

std::string CPGFFileColumnHeader::GetColumnName(int32_t index)
{
	assert((index >= 0) && (index < (int32_t) m_ColumnNames.size()));
	std::string header = m_ColumnNames.at(index);
	return header;
}

int32_t CPGFFileColumnHeader::GetColumnIndex(const char* str)
{
	assert(str != NULL);
	int32_t index = -1;
	std::string header = str;
	std::transform(header.begin(), header.end(), header.begin(), tolower);
	std::map<std::string, int32_t>::iterator pos = m_ColumnIndices.find(header);
	if (pos != m_ColumnIndices.end())
		index = pos->second;
	return index;
}

void CPGFFileColumnHeader::GetColumnNames(std::vector<std::string>& names)
{
	names = m_ColumnNames;
}

void CPGFFileColumnHeader::ParseHeaders(std::string line)
{
	assert(line != "");
	std::string delimiter = "\t";
	std::string::size_type start = 0;
	std::string::size_type end = line.find(delimiter, start); 
	while (end != std::string::npos)
	{
		std::string column = line.substr(start, end - start);
		Append(column.c_str());
		start = end + delimiter.length();
		end = line.find(delimiter, start); 
	}
	std::string column = line.substr(start);
	Append(column.c_str());
}

//////////////////////////////////////////////////////////////////////

CPGFFileHeaderData::CPGFFileHeaderData()
{
	Clear();
}

CPGFFileHeaderData::~CPGFFileHeaderData()
{
	Clear();
}

void CPGFFileHeaderData::Clear()
{
	m_Version = 0.0;
	m_ChipType = "";
	m_MetaTags.clear();
	m_ColumnHeader0.RemoveAll();
	m_ColumnHeader1.RemoveAll();
	m_ColumnHeader2.RemoveAll();
}

void CPGFFileHeaderData::ParseMetaTags()
{
	std::multimap<std::string, std::string>::iterator pos;
	pos = m_MetaTags.find("pgf_format_version");
	if (pos != m_MetaTags.end()) m_Version = (float) atof(pos->second.c_str());
	pos = m_MetaTags.find("chip_type");
	if (pos != m_MetaTags.end()) m_ChipType = pos->second;
	pos = m_MetaTags.find("header0");
	if (pos != m_MetaTags.end()) 
		m_ColumnHeader0.ParseHeaders(pos->second);
	pos = m_MetaTags.find("header1");
	if (pos != m_MetaTags.end()) 
		m_ColumnHeader1.ParseHeaders(pos->second);
	pos = m_MetaTags.find("header2");
	if (pos != m_MetaTags.end()) 
		m_ColumnHeader2.ParseHeaders(pos->second);
}

void CPGFFileHeaderData::InsertMetaTag(const char* tag, const char* value)
{
	std::string t = tag;
	std::string v = value;
	m_MetaTags.insert(std::make_pair(t, v));
}

//////////////////////////////////////////////////////////////////////

void CPGFFileHeaderData::GetMetaTagNames(std::vector<std::string>& names)
{
	names.clear();
	std::string lastKey = "";
	for (std::multimap<std::string, std::string>::iterator pos = m_MetaTags.begin(); pos != m_MetaTags.end(); pos++)
	{
		if (pos->first != lastKey)
		{
			if (lastKey != "")
				names.push_back(lastKey);
			lastKey = pos->first;
		}
	} 
	if (lastKey != "")
		names.push_back(lastKey);
}

void CPGFFileHeaderData::GetMetaTagValues(const char* name, std::vector<std::string>& values)
{
	assert(name != NULL);
	assert(strlen(name) > 0);
	values.clear();
	std::string key = name;
	key = Trim(key);
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	std::multimap<std::string, std::string>::iterator posStart = m_MetaTags.find(key);
	std::multimap<std::string, std::string>::iterator posEnd = m_MetaTags.upper_bound(key);
	if (posStart != m_MetaTags.end())
	{
		for (std::multimap<std::string, std::string>::iterator pos = posStart; pos != posEnd; pos++)
		{
			values.push_back(pos->second);
		}
	}
}

std::string CPGFFileHeaderData::Trim(const std::string& str) 
{
	int len = (int) str.length();
	int pos = 0;

	while ((pos < len) && (str[pos] <= ' ')) 
	{
		pos++;
	}
	while ((pos < len) && (str[len - 1] <= ' ')) 
	{
		len--;
	}
	return ((pos > 0) || (len < (int) str.length())) ? str.substr(pos, len - pos) : str;
}

std::string CPGFFileHeaderData::GetColumnName(int32_t headerIndex, int32_t columnIndex)
{ 
	std::string strColumn = "";
	switch (headerIndex)
	{
	case 0:
		strColumn = m_ColumnHeader0.GetColumnName(columnIndex); 
		break;
	case 1:
		strColumn = m_ColumnHeader1.GetColumnName(columnIndex); 
		break;
	case 2:
		strColumn = m_ColumnHeader2.GetColumnName(columnIndex); 
		break;
	}
	return strColumn;
}

int32_t CPGFFileHeaderData::GetColumnIndex(int32_t headerIndex, std::string name)
{ 
	int32_t columnIndex = 0;
	switch (headerIndex)
	{
	case 0:
		columnIndex = m_ColumnHeader0.GetColumnIndex(name.c_str()); 
		break;
	case 1:
		columnIndex = m_ColumnHeader1.GetColumnIndex(name.c_str()); 
		break;
	case 2:
		columnIndex = m_ColumnHeader2.GetColumnIndex(name.c_str()); 
		break;
	}
	return columnIndex;
}

int32_t CPGFFileHeaderData::GetColumnCount(int32_t headerIndex)
{ 
	int32_t columnCount = 0;
	switch (headerIndex)
	{
	case 0:
		columnCount = m_ColumnHeader0.GetColumnCount(); 
		break;
	case 1:
		columnCount = m_ColumnHeader1.GetColumnCount(); 
		break;
	case 2:
		columnCount = m_ColumnHeader2.GetColumnCount(); 
		break;
	}
	return columnCount;
}

void CPGFFileHeaderData::GetProbeSetHeader(std::vector<std::string>& names)
{
	m_ColumnHeader0.GetColumnNames(names);
}

void CPGFFileHeaderData::GetAtomHeader(std::vector<std::string>& names)
{
	m_ColumnHeader1.GetColumnNames(names);
}

void CPGFFileHeaderData::GetProbeHeader(std::vector<std::string>& names)
{
	m_ColumnHeader2.GetColumnNames(names);
}

//////////////////////////////////////////////////////////////////////

CPGFType::CPGFType(const char* str)
{
	assert(strlen(str) != 0);
	ParseString(str);
}

CPGFType::~CPGFType()
{
	Clear();
}

CPGFType::CPGFType(const CPGFType& that)
{
	m_Types.operator=(that.m_Types);
}

const CPGFType& CPGFType::operator=(const CPGFType& that)
{
	m_Types.operator=(that.m_Types);

	return *this;
}

bool CPGFType::IsA(const char* str)
{
	bool bReturn = false;
	assert(strlen(str) != 0);

	std::string key = str;
	std::map<std::string, bool>::iterator pos = m_Types.find(key);
	if (pos != m_Types.end())
		bReturn = true;

	return bReturn;
}

void CPGFType::SetTypes(const char* str)
{
	m_Types.clear();
	ParseString(str);
}

void CPGFType::ParseString(const char* str)
{
	assert(strlen(str) != 0);

	std::string typeString = str;
	std::string delimiter = ":";
	std::vector<std::string> groups;
	std::string::size_type start = 0;
	std::string::size_type end = typeString.find(delimiter, start); 
	while (end != std::string::npos)
	{
		std::string group = typeString.substr(start, end - start);
		groups.push_back(group);
		start = end + delimiter.length();
		end = typeString.find(delimiter, start); 
	}
	std::string group = typeString.substr(start);
	groups.push_back(group);

	delimiter = "->";
	for (std::vector<std::string>::iterator pos = groups.begin(); pos != groups.end(); pos++)
	{
		std::string group = pos->c_str();
		start = 0;
		end = group.find(delimiter, start); 
		while (end != std::string::npos)
		{
			std::string type = group.substr(start, end - start);
			m_Types.insert(std::make_pair(type, true));
			start = end + delimiter.length();
			end = group.find(delimiter, start); 
		}
		std::string type = group.substr(start);
		m_Types.insert(std::make_pair(type, true));
	}
}

//////////////////////////////////////////////////////////////////////

CPGFProbe::CPGFProbe()
{
	Clear();
}

CPGFProbe::~CPGFProbe()
{
	Clear();
}

CPGFProbe::CPGFProbe(const CPGFProbe& that)
{
	m_nProbeID = that.m_nProbeID;
	m_Type = that.m_Type;
	m_Annotations.operator=(that.m_Annotations);
}

const CPGFProbe& CPGFProbe::operator=(const CPGFProbe& that)
{
	m_nProbeID = that.m_nProbeID;
	m_Type = that.m_Type;
	m_Annotations.operator=(that.m_Annotations);

	return *this;
}

void CPGFProbe::AddAnnotation(const char* header, const char* value)
{
	assert((strlen(header) != 0) && (strlen(value) != 0));
	std::string key = header;
	std::string annot = value;
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	m_Annotations.insert(std::make_pair(key, annot));
}

std::string CPGFProbe::GetAnnotation(const char* header)
{
	assert(strlen(header) != 0);
	std::string key = header;
	std::string value = "";
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	std::map<std::string, std::string>::iterator pos = m_Annotations.find(key);
	assert(pos != m_Annotations.end());
	value = pos->second;
	return value;
}

void CPGFProbe::Clear()
{
	m_nProbeID = 0;
	m_Type.Clear();
	m_Annotations.clear();
}

//////////////////////////////////////////////////////////////////////

CPGFAtom::CPGFAtom()
{
	Clear();
}

CPGFAtom::~CPGFAtom()
{
	Clear();
}

CPGFAtom::CPGFAtom(const CPGFAtom& that)
{
	m_nAtomID = that.m_nAtomID;
	m_Type = that.m_Type;
	m_Annotations.operator=(that.m_Annotations);
	m_Probes.operator=(that.m_Probes);
}

const CPGFAtom& CPGFAtom::operator=(const CPGFAtom& that)
{
	m_nAtomID = that.m_nAtomID;
	m_Type = that.m_Type;
	m_Annotations.operator=(that.m_Annotations);
	m_Probes.operator=(that.m_Probes);

	return *this;
}

void CPGFAtom::AddAnnotation(const char* header, const char* value)
{
	assert((strlen(header) != 0) && (strlen(value) != 0));
	std::string key = header;
	std::string annot = value;
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	m_Annotations.insert(std::make_pair(key, annot));
}

std::string CPGFAtom::GetAnnotation(const char* header)
{
	assert(strlen(header) != 0);
	std::string key = header;
	std::string value = "";
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	std::map<std::string, std::string>::iterator pos = m_Annotations.find(key);
	assert(pos != m_Annotations.end());
	value = pos->second;
	return value;
}

void CPGFAtom::AddProbe(CPGFProbe& probe)
{
	assert(probe.GetProbeID() != 0);
	m_Probes.push_back(probe);
}

void CPGFAtom::GetProbes(std::vector<CPGFProbe>& probes, const char* type)
{
	probes.clear();
	if (type != NULL)
	{
		assert(strlen(type) != 0);
		CPGFType probeType;
		for (std::vector<CPGFProbe>::size_type i = 0; i < m_Probes.size(); i++)
		{
			CPGFProbe probe = m_Probes.at(i);
			probe.GetProbeType(probeType);
			if (probeType.IsA(type))
				probes.push_back(probe);
		}
	}
	else
		probes.operator=(m_Probes);
}

void CPGFAtom::Clear()
{
	m_nAtomID = 0;
	m_Type.Clear();
	m_Annotations.clear();
	m_Probes.clear();
}

//////////////////////////////////////////////////////////////////////

CPGFProbeSet::CPGFProbeSet()
{
	Clear();
}

CPGFProbeSet::~CPGFProbeSet()
{
	Clear();
}

CPGFProbeSet::CPGFProbeSet(const CPGFProbeSet& that)
{
	m_nProbeSetID = that.m_nProbeSetID;
	m_Type = that.m_Type;
	m_Annotations.operator=(that.m_Annotations);
	m_Atoms.operator=(that.m_Atoms);
}

const CPGFProbeSet& CPGFProbeSet::operator=(const CPGFProbeSet& that)
{
	m_nProbeSetID = that.m_nProbeSetID;
	m_Type = that.m_Type;
	m_Annotations.operator=(that.m_Annotations);
	m_Atoms.operator=(that.m_Atoms);

	return *this;
}

void CPGFProbeSet::AddAnnotation(const char* header, const char* value)
{
	assert((strlen(header) != 0) && (strlen(value) != 0));
	std::string key = header;
	std::string annot = value;
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	m_Annotations.insert(std::make_pair(key, annot));
}

std::string CPGFProbeSet::GetAnnotation(const char* header)
{
	assert(strlen(header) != 0);
	std::string key = header;
	std::string value = "";
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	std::map<std::string, std::string>::iterator pos = m_Annotations.find(key);
	assert(pos != m_Annotations.end());
	value = pos->second;
	return value;
}

void CPGFProbeSet::AddAtom(CPGFAtom& atom)
{
	assert(atom.GetAtomID() != 0);
	m_Atoms.push_back(atom);
}

void CPGFProbeSet::GetAtoms(std::vector<CPGFAtom>& atoms, const char* type)
{
	atoms.clear();
	if (type != NULL)
	{
		assert(strlen(type) != 0);
		CPGFType atomType;
		for (std::vector<CPGFAtom>::size_type i = 0; i < m_Atoms.size(); i++)
		{
			CPGFAtom atom = m_Atoms.at(i);
			atom.GetAtomType(atomType);
			if (atomType.IsA(type))
				atoms.push_back(atom);
		}
	}
	else
		atoms.operator=(m_Atoms);
}

void CPGFProbeSet::Clear()
{
	m_nProbeSetID = 0;
	m_Type.Clear();
	m_Annotations.clear();
	m_Atoms.clear();
}

//////////////////////////////////////////////////////////////////////

CPGFFileData::CPGFFileData()
{
	m_Error = "";
	m_FileName = "";
	m_nMode = PGF_STREAM_MODE;
	m_IndexProbeSetOnly = false;
	m_Eof = false;
	m_Buffer = new char[BUFFER_SIZE];
	m_BufferSize = 0;
	m_BufferPos = 0;
	m_Columns.clear();
}

CPGFFileData::~CPGFFileData()
{
	Close();
	delete [] m_Buffer;
}

//////////////////////////////////////////////////////////////////////

void CPGFFileData::SetError(const char* str)
{
	assert(str != NULL);
	int iLen = (int) strlen(str);
	assert(iLen > 0);
	m_Error = str;
}

void CPGFFileData::SetReadMode(const int16_t mode)
{
	assert((mode >= PGF_INDEX_MODE) && (mode <= PGF_HASH_MODE)); 
	m_nMode = mode;
}

void CPGFFileData::SetFileName(const char *str)
{
	assert(str != NULL);
	int iLen = (int) strlen(str);
	assert(iLen > 0);
	m_FileName = str;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::Exists()
{
	assert(m_FileName != "");

	// Find the file stats.
	struct stat st;
	return ((stat(m_FileName.c_str(), &st) == 0)? true: false);
}

//////////////////////////////////////////////////////////////////////

int32_t CPGFFileData::GetFileSize()
{
	assert(m_FileName != "");

	int32_t size = 0;
	struct stat st;
	if (stat(m_FileName.c_str(), &st) == 0)
	{
		size = st.st_size;
	}
	return size;
}


//////////////////////////////////////////////////////////////////////

bool CPGFFileData::ReadBuffer()
{
	bool bSuccessful = false;
	assert(m_FileIn.is_open());

	memset(m_Buffer, 0, sizeof(char) * BUFFER_SIZE);
	m_BufferPos = 0;
	m_BufferSize = 0;
	m_BufferStartPos = m_FileIn.tellg();
	std::istream& stream = m_FileIn.read(m_Buffer, BUFFER_SIZE - 1);
	if (stream.good() || (stream.eof() && !m_Eof)) 
	{
		bSuccessful = true;
		m_BufferSize = stream.gcount();
	}
	if (stream.eof()) m_Eof = true;

	return bSuccessful;
}

std::string CPGFFileData::GetLine()
{
	assert(m_FileIn.is_open());
	std::string line = "";
	std::size_t len = 0;
	int32_t endPos = 0;
	bool bValid = false;
	bool bLineFeed = false;
	
	while (!bValid)
	{
		if (!m_Eof && ((m_BufferSize == 0) || (m_BufferPos >= m_BufferSize)))
		{
			bool bReturn = ReadBuffer();
			assert(bReturn);
		}
		else if (m_Eof && (m_BufferPos >= m_BufferSize))
			break;

		endPos = m_BufferPos;
		bLineFeed = false;
		for (int32_t i = m_BufferPos; i < m_BufferSize; i++)
		{
			if ((m_Buffer[i] != '\r') && (m_Buffer[i] != '\n'))
				endPos++;
			else
			{
				bLineFeed = true;
				i = m_BufferSize;
			}
		}
		if (endPos != m_BufferPos)
		{
			len = line.length();
			line.resize(len + endPos - m_BufferPos + 1, '\0');
			line.replace(len, endPos - m_BufferPos, m_Buffer + m_BufferPos, endPos - m_BufferPos);
			line = Trim(line);
			if (line == "")
			{
				line = "";
				bValid = false;
			}
			else if (bLineFeed)
				bValid = true;
			else
				bValid = false;
		}
		if (bLineFeed)
		{
			if (m_BufferSize > endPos + 1)
			{
				if ((m_Buffer[endPos] == '\r') && (m_Buffer[endPos + 1] == '\n'))
					endPos+=2;
				else
					endPos++;
			}
			else if (m_BufferSize > endPos)
				endPos++;
			if (line != "") bValid = true;
		}
		m_BufferPos = endPos;
	}	

	return line;
}

std::string CPGFFileData::Trim(const std::string& str) 
{
	int len = (int) str.length();
	int pos = 0;

	while ((pos < len) && (str[pos] != '\t') && (str[pos] <= ' ')) 
	{
		pos++;
	}
	while ((pos < len) && (str[len - 1] != '\t') && (str[len - 1] <= ' ')) 
	{
		len--;
	}
	return ((pos > 0) || (len < (int) str.length())) ? str.substr(pos, len - pos) : str;
}
//////////////////////////////////////////////////////////////////////

void CPGFFileData::ParseMetaTags()
{
	int32_t pos = m_BufferPos;
	std::string line = GetLine();
	assert(line != "");
	std::string::size_type index = std::string::npos;
	while ((line.length() >= 2) && (line.at(0) == '#') && (line.at(1) == '%'))
	{
		index = line.find('=');
		if (index != std::string::npos)
		{
			std::string key = line.substr(2, index - 2);
			std::string value = line.substr(index + 1);
			std::transform(key.begin(), key.end(), key.begin(), tolower);
			m_HeaderData.InsertMetaTag(key.c_str(), value.c_str());
		}
		pos = m_BufferPos;
		line = GetLine();
	}
	m_BufferPos = pos;
	m_HeaderData.ParseMetaTags();
}

//////////////////////////////////////////////////////////////////////

void CPGFFileData::GetColumns(std::vector<std::string>& columns)
{
	std::string delimiter = "\t";
	columns.clear();
	std::string line = GetLine();
	if (line != "")
	{
		std::string::size_type start = 0;
		std::string::size_type end = line.find(delimiter, start); 
		while (end != std::string::npos)
		{
			std::string column = line.substr(start, end - start);
			columns.push_back(column);
			start = end + delimiter.length();
			end = line.find(delimiter, start); 
		}
		std::string column = line.substr(start);
		columns.push_back(column);
	}
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::Open(const char* fileName, const short mode, const bool indexProbeSetOnly)
{
	bool bSuccessful = true;

	if (fileName == NULL)
	{
		m_Error = "Please provide a file name";
		return false;
	}

	SetFileName(fileName);
	SetReadMode(mode);
	SetIndexProbeSetOnly(indexProbeSetOnly);

	switch(m_nMode)
	{
	case PGF_STREAM_MODE:
		bSuccessful = StreamOpen();
		break;
	case PGF_HASH_MODE:
		bSuccessful = HashOpen();
		break;
	case PGF_INDEX_MODE:
		bSuccessful = IndexOpen();
		break;
	default:
		char sz[100];
		snprintf(sz, sizeof(sz), "Unknown open mode %d for parsing PGF", m_nMode);
		m_Error = sz;
		bSuccessful = false;
		break;
	}
	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////

void CPGFFileData::Clear()
{
	memset(m_Buffer, 0, sizeof(char) * BUFFER_SIZE);
	m_BufferPos = 0;
	m_Eof = false;
	m_Columns.clear();
	m_HeaderData.Clear();
	m_ProbeSetIndices.clear();
	m_AtomIndices.clear();
	m_ProbeIndices.clear();
}

void CPGFFileData::Close()
{
	Clear();
	if (m_FileIn.is_open())
	{
		m_FileIn.clear();
		m_FileIn.close();
	}
}

void CPGFFileData::GetMetaTagNames(std::vector<std::string>& names)
{
	m_HeaderData.GetMetaTagNames(names);
}

void CPGFFileData::GetMetaTagValues(const char* name, std::vector<std::string>& values)
{
	m_HeaderData.GetMetaTagValues(name, values);
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::StreamOpen()
{
	m_FileIn.open(m_FileName.c_str(), std::ios::in|std::ios::binary);

	// Check if open
	if (!m_FileIn)
	{
		m_Error = "Unable to open pgf file " + m_FileName;
		return false;
	}
	ParseMetaTags();
	return true;
}

bool CPGFFileData::NextEntry(CPGFProbeSet& probeSet)
{
	if (m_nMode != PGF_STREAM_MODE)
	{
		m_Error = "Cannot call NextEntry when not in stream mode";
		return false;
	}
	if (!m_FileIn.is_open())
	{
		m_Error = "No PGF file is opened";
		return false;
	}

	probeSet.Clear();
	if (m_Columns.size() <= 0) GetColumns(m_Columns);
	return _GetProbeSet(m_Columns, probeSet);
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::HashOpen()
{
	m_FileIn.open(m_FileName.c_str(), std::ios::in|std::ios::binary);

	// Check if open
	if (!m_FileIn)
	{
		m_Error = "Unable to open pgf file " + m_FileName;
		return false;
	}
	ParseMetaTags();

	std::vector<CPGFAtom> atoms;
	std::vector<CPGFProbe> probes;
	GetColumns(m_Columns);
	while (m_Columns.size() > 0)
	{
		int32_t id = 0;
		if (IsProbeSet(m_Columns)) 
		{
			id = atoi(m_Columns.at(0).c_str());
			CPGFProbeSet probeSet;
			if (_GetProbeSet(m_Columns, probeSet))
			{
				m_ProbeSetEntries.insert(std::make_pair(id, probeSet));
				atoms.clear();
				probeSet.GetAtoms(atoms);
				for (std::vector<CPGFAtom>::size_type iAtom = 0; iAtom < atoms.size(); iAtom++)
				{
					CPGFAtom atom = atoms.at(iAtom);
					m_AtomEntries.insert(std::make_pair(atom.GetAtomID(), atom));
					probes.clear();
					atom.GetProbes(probes);
					for (std::vector<CPGFProbe>::size_type iProbe = 0; iProbe < probes.size(); iProbe++)
					{
						CPGFProbe probe = probes.at(iProbe);
						m_ProbeEntries.insert(std::make_pair(probe.GetProbeID(), probe));
					}
				}
			}
		}		
		if (m_Columns.size() <= 0) GetColumns(m_Columns);
	}
	m_FileIn.clear();
	m_FileIn.close();

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::IndexOpen()
{
	m_FileIn.open(m_FileName.c_str(), std::ios::in|std::ios::binary);

	// Check if open
	if (!m_FileIn)
	{
		m_Error = "Unable to open pgf layout file " + m_FileName;
		return false;
	}
	ParseMetaTags();

	std::vector<std::string> columns;
	std::streampos posStart = m_BufferStartPos;
	std::streampos posLine = m_BufferStartPos;
	std::streamoff offset = m_BufferPos;
	posLine = posLine + offset;
	GetColumns(columns);
	while (columns.size() > 0)
	{
		int32_t id = 0;
		if (IsProbeSet(columns)) 
		{
			id = atoi(columns.at(0).c_str());
			m_ProbeSetIndices.insert(std::make_pair(id, posLine));
		}
		else if (!m_IndexProbeSetOnly && IsAtom(columns)) 
		{
			id = atoi(columns.at(1).c_str());
			m_AtomIndices.insert(std::make_pair(id, posLine));
		}
		else if (!m_IndexProbeSetOnly && IsProbe(columns)) 
		{
			id = atoi(columns.at(2).c_str());
			m_ProbeIndices.insert(std::make_pair(id, posLine));
		}
		
		posLine = m_BufferStartPos;
		offset = m_BufferPos;
		posLine = posLine + offset;
		GetColumns(columns);
	}

	m_FileIn.clear();
	m_FileIn.seekg(posStart);

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::GetProbeSet(const int32_t& probeSetId, CPGFProbeSet& probeSet)
{
	bool bSuccessful = false;

	if (probeSetId <= 0)
	{
		char buf[10];
		snprintf(buf, sizeof(buf), "%d", probeSetId);
		m_Error = "Probe set ID has to be greater than 0: ";
		m_Error += buf;
	}
	else
	{
		probeSet.Clear();
		switch(m_nMode)
		{
		case PGF_STREAM_MODE:
			m_Error = "Cannot call GetProbeSet when in stream mode";
			break;
		case PGF_HASH_MODE:
			bSuccessful = GetProbeSetByHash(probeSetId, probeSet);
			break;
		case PGF_INDEX_MODE:
			bSuccessful = GetProbeSetByIndex(probeSetId, probeSet);
			break;
		default:
			m_Error = "Cannot call GetProbeSet when not in index or hash mode";
			break;
		}
	}
	return bSuccessful;
}

bool CPGFFileData::GetProbeSetByHash(const int32_t& probeSetId, CPGFProbeSet& probeSet)
{
	bool bSuccessful = false;

	std::map<int32_t, CPGFProbeSet>::iterator pos = m_ProbeSetEntries.find(probeSetId);
	if (pos != m_ProbeSetEntries.end())
	{
		probeSet = pos->second;
		bSuccessful = true;
	}
	else
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", probeSetId);
		m_Error = "Probe set ";
		m_Error += buf;
		m_Error += " is not found in pgf file.";
	}

	return bSuccessful;
}

bool CPGFFileData::GetProbeSetByIndex(const int32_t& probeSetId, CPGFProbeSet& probeSet)
{
	bool bSuccessful = false;

	if (!m_FileIn.is_open())
	{
		m_Error = "PGF file is not open: " + m_FileName;
		return false;
	}

	std::map<int32_t, std::streampos>::iterator pos = m_ProbeSetIndices.find(probeSetId);
	if (pos != m_ProbeSetIndices.end())
	{
		m_FileIn.clear();
		m_FileIn.seekg(pos->second);
		m_BufferSize = 0;
		m_Eof = false;

		GetColumns(m_Columns);
		bSuccessful = _GetProbeSet(m_Columns, probeSet);
	}
	else
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", probeSetId);
		m_Error = "Probe set";
		m_Error += buf;
		m_Error += " is not found in pgf file.";
	}

	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::GetAtom(const int32_t& atomId, CPGFAtom& atom)
{
	bool bSuccessful = false;

	if (atomId <= 0)
	{
		char buf[10];
		snprintf(buf, sizeof(buf), "%d", atomId);
		m_Error = "Atom ID has to be greater than 0: ";
		m_Error += buf;
	}
	else
	{
		atom.Clear();
		switch(m_nMode)
		{
		case PGF_STREAM_MODE:
			m_Error = "Cannot call GetAtom when in stream mode";
			break;
		case PGF_HASH_MODE:
			bSuccessful = GetAtomByHash(atomId, atom);
			break;
		case PGF_INDEX_MODE:
			bSuccessful = GetAtomByIndex(atomId, atom);
			break;
		default:
			m_Error = "Cannot call GetAtom when not in index or hash mode";
			break;
		}
	}
	return bSuccessful;
}

bool CPGFFileData::GetAtomByHash(const int32_t& atomId, CPGFAtom& atom)
{
	bool bSuccessful = false;

	std::map<int32_t, CPGFAtom>::iterator pos = m_AtomEntries.find(atomId);
	if (pos != m_AtomEntries.end())
	{
		atom = pos->second;
		bSuccessful = true;
	}
	else
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", atomId);
		m_Error = "Atom ";
		m_Error += buf;
		m_Error += " is not found in pgf file.";
	}

	return bSuccessful;
}

bool CPGFFileData::GetAtomByIndex(const int32_t& atomId, CPGFAtom& atom)
{
	bool bSuccessful = false;

	if (!m_FileIn.is_open())
	{
		m_Error = "PGF file is not open: " + m_FileName;
		return false;
	}

	if (m_IndexProbeSetOnly)
	{
		m_Error = "Cannot call GetAtom in ParsePGF when INDEXPROBESETONLY flag is set.";
		return false;
	}

	std::map<int32_t, std::streampos>::iterator pos = m_AtomIndices.find(atomId);
	if (pos != m_AtomIndices.end())
	{
		m_FileIn.clear();
		m_FileIn.seekg(pos->second);
		m_BufferSize = 0;
		m_Eof = false;

		GetColumns(m_Columns);
		bSuccessful = _GetAtom(m_Columns, atom);
	}
	else
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", atomId);
		m_Error = "Atom ";
		m_Error += buf;
		m_Error += " is not found in pgf file.";
	}

	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::GetProbe(const int32_t& probeId, CPGFProbe& probe)
{
	bool bSuccessful = false;

	if (probeId <= 0)
	{
		char buf[10];
		snprintf(buf, sizeof(buf), "%d", probeId);
		m_Error = "Probe ID has to be greater than 0: ";
		m_Error += buf;
	}
	else
	{
		probe.Clear();
		switch(m_nMode)
		{
		case PGF_STREAM_MODE:
			m_Error = "Cannot call GetProbe when in stream mode";
			break;
		case PGF_HASH_MODE:
			bSuccessful = GetProbeByHash(probeId, probe);
			break;
		case PGF_INDEX_MODE:
			bSuccessful = GetProbeByIndex(probeId, probe);
			break;
		default:
			m_Error = "Cannot call GetProbe when not in index or hash mode";
			break;
		}
	}
	return bSuccessful;
}

bool CPGFFileData::GetProbeByHash(const int32_t& probeId, CPGFProbe& probe)
{
	bool bSuccessful = false;

	std::map<int32_t, CPGFProbe>::iterator pos = m_ProbeEntries.find(probeId);
	if (pos != m_ProbeEntries.end())
	{
		probe = pos->second;
		bSuccessful = true;
	}
	else
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", probeId);
		m_Error = "Probe ";
		m_Error += buf;
		m_Error += " is not found in pgf file.";
	}

	return bSuccessful;
}

bool CPGFFileData::GetProbeByIndex(const int32_t& probeId, CPGFProbe& probe)
{
	bool bSuccessful = false;

	if (!m_FileIn.is_open())
	{
		m_Error = "PGF file is not open: " + m_FileName;
		return false;
	}

	if (m_IndexProbeSetOnly)
	{
		m_Error = "Cannot call GetProbe in ParsePGF when INDEXPROBESETONLY flag is set.";
		return false;
	}

	std::map<int32_t, std::streampos>::iterator pos = m_ProbeIndices.find(probeId);
	if (pos != m_ProbeIndices.end())
	{
		m_FileIn.clear();
		m_FileIn.seekg(pos->second);
		m_BufferSize = 0;
		m_Eof = false;

		GetColumns(m_Columns);
		bSuccessful = _GetProbe(m_Columns, probe);
	}
	else
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", probeId);
		m_Error = "Probe ";
		m_Error += buf;
		m_Error += " is not found in pgf file.";
	}

	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::GetProbeSetPGFLines(const int32_t& probeSetId, std::vector<std::string>& lines)
{
	bool bSuccessful = false;

	if (probeSetId <= 0)
	{
		char buf[10];
		snprintf(buf, sizeof(buf), "%d", probeSetId);
		m_Error = "Probe set ID has to be greater than 0: ";
		m_Error += buf;
		return false;
	}
	if (m_nMode != PGF_INDEX_MODE)
	{
		m_Error = "GetProbeSetPGFLines method only valid when in index mode.";
		return false;
	}

	if (!m_FileIn.is_open())
	{
		m_Error = "PGF file is not open: " + m_FileName;
		return false;
	}

	lines.clear();

	std::map<int32_t, std::streampos>::iterator pos = m_ProbeSetIndices.find(probeSetId);
	if (pos != m_ProbeSetIndices.end())
	{
		m_FileIn.clear();
		m_FileIn.seekg(pos->second);
		m_BufferSize = 0;
		m_Eof = false;

		std::string line = GetLine();
		while (line.at(0) != '\t')
		{
			lines.push_back(line);
			line = GetLine();
		}
		bSuccessful = true;
	}
	else
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", probeSetId);
		m_Error = "Probe set ";
		m_Error += buf;
		m_Error += " is not found in pgf file.";
	}

	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::IsProbeSet(const std::vector<std::string>& columns)
{
	bool bProbeSet = false;

	if ((columns.size() > 0) && (columns.at(0) != ""))
		bProbeSet = true;

	return bProbeSet;
}

bool CPGFFileData::IsAtom(const std::vector<std::string>& columns)
{
	bool bAtom = false;

	if ((columns.size() >= 2) && (columns.at(0) == "") && (columns.at(1) != ""))
		bAtom = true;

	return bAtom;
}

bool CPGFFileData::IsProbe(const std::vector<std::string>& columns)
{
	bool bProbe = false;

	if ((columns.size() > 2) && (columns.at(0) == "") && (columns.at(1) == "") && (columns.at(2) != ""))
		bProbe = true;

	return bProbe;
}

//////////////////////////////////////////////////////////////////////

bool CPGFFileData::_GetProbeSet(const std::vector<std::string>& columns, CPGFProbeSet& probeSet)
{
	bool bSuccessful = true;
        std::vector<std::string>::size_type col;
	if (columns.size() > 0)
	{
		for ( col = 0; col < columns.size(); col++) 
		{
			std::string column = columns.at(col);
			if ((int) col < m_HeaderData.GetColumnCount(CPGFFileHeaderData::PROBESET_HEADER))
			{
				std::string colname = m_HeaderData.GetColumnName(CPGFFileHeaderData::PROBESET_HEADER, (int) col);
				if (colname == "probeset_id") probeSet.SetProbeSetID((int32_t) (atoi(column.c_str())));
				else if (colname == "type") { if (column != "") probeSet.SetProbeSetType(column.c_str()); }
				else
				{
					std::string value = column.c_str();
					probeSet.AddAnnotation(colname.c_str(), value.c_str());
				}
			}
		}
		if (m_HeaderData.GetColumnCount(CPGFFileHeaderData::PROBESET_HEADER) > (int) columns.size())
		{
			for (col = columns.size(); (int) col < m_HeaderData.GetColumnCount(CPGFFileHeaderData::PROBESET_HEADER); col++)
			{
				std::string colname = m_HeaderData.GetColumnName(CPGFFileHeaderData::PROBESET_HEADER, (int) col);
				std::string value = "";
				probeSet.AddAnnotation(colname.c_str(), value.c_str());
			}
		}
		GetColumns(m_Columns);
		while (bSuccessful && IsAtom(m_Columns))
		{
			CPGFAtom atom;
			bSuccessful = _GetAtom(m_Columns, atom);
			probeSet.AddAtom(atom);
			if (m_Columns.size() <= 0) GetColumns(m_Columns);
		}
	}
	else
		bSuccessful = false;
	return bSuccessful;
}

bool CPGFFileData::_GetAtom(const std::vector<std::string>& columns, CPGFAtom& atom)
{
  std::vector<std::string>::size_type col;
	bool bSuccessful = true;
	if (columns.size() > 0)
	{
		for (col = 0; col < columns.size(); col++) 
		{
			if (col > 0)
			{
				std::string column = columns.at(col);
				if ((int) col < m_HeaderData.GetColumnCount(CPGFFileHeaderData::ATOM_HEADER))
				{
					std::string colname = m_HeaderData.GetColumnName(CPGFFileHeaderData::ATOM_HEADER, (int) col);
					if (colname == "atom_id") atom.SetAtomID((int32_t) (atoi(column.c_str())));
					else if (colname == "type") { if (column != "") atom.SetAtomType(column.c_str()); }
					else
					{
						std::string value = column.c_str();
						atom.AddAnnotation(colname.c_str(), value.c_str());
					}
				}
			}
		}
		if (m_HeaderData.GetColumnCount(CPGFFileHeaderData::ATOM_HEADER) > (int) columns.size())
		{
			for (col = columns.size(); (int) col < m_HeaderData.GetColumnCount(CPGFFileHeaderData::ATOM_HEADER); col++)
			{
				std::string colname = m_HeaderData.GetColumnName(CPGFFileHeaderData::ATOM_HEADER, (int) col);
				std::string value = "";
				atom.AddAnnotation(colname.c_str(), value.c_str());
			}
		}
		GetColumns(m_Columns);
		while (bSuccessful && IsProbe(m_Columns))
		{
			CPGFProbe probe;
			bSuccessful = _GetProbe(m_Columns, probe);
			atom.AddProbe(probe);
			GetColumns(m_Columns);
		}
	}
	else
		bSuccessful = false;
	return bSuccessful;
}

bool CPGFFileData::_GetProbe(const std::vector<std::string>& columns, CPGFProbe& probe)
{
	bool bSuccessful = true;
        std::vector<std::string>::size_type    col;
	if (columns.size() > 0)
	{
		for ( col = 0; col < columns.size(); col++) 
		{
			if (col > 1)
			{
				std::string column = columns.at(col);
				if ((int) col < m_HeaderData.GetColumnCount(CPGFFileHeaderData::PROBE_HEADER))
				{
					std::string colname = m_HeaderData.GetColumnName(CPGFFileHeaderData::PROBE_HEADER, (int) col);
					if (colname == "probe_id") probe.SetProbeID((int32_t) (atoi(column.c_str())));
					else if (colname == "type") { if (column != "") probe.SetProbeType(column.c_str()); }
					else
					{
						std::string value = column.c_str();
						probe.AddAnnotation(colname.c_str(), value.c_str());
					}
				}
			}
		}
		if (m_HeaderData.GetColumnCount(CPGFFileHeaderData::PROBE_HEADER) > (int) columns.size())
		{
			for (col = columns.size(); (int) col < m_HeaderData.GetColumnCount(CPGFFileHeaderData::PROBE_HEADER); col++)
			{
				std::string colname = m_HeaderData.GetColumnName(CPGFFileHeaderData::PROBE_HEADER, (int) col);
				std::string value = "";
				probe.AddAnnotation(colname.c_str(), value.c_str());
			}
		}
	}
	else
		bSuccessful = false;
	return bSuccessful;
}
