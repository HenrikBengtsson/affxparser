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

// WARNING: This class is deprecated! Use file/TsvFile instead.

#include "TSVFileData.h"
#include "FileIO.h"
#include "FileWriter.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <istream>
#include <errno.h>
#include <stdio.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

using namespace affxtsv;

//////////////////////////////////////////////////////////////////////

#define BUFFER_SIZE 500000

//////////////////////////////////////////////////////////////////////

CTSVFileConvertOutputs::CTSVFileConvertOutputs (const std::string& SingleString, const std::vector<std::string>& MultipleStrings,
		const std::string& SingleInt, const std::vector<std::string>& MultipleInts,
		const std::string& SingleFloat, const std::vector<std::string>& MultipleFloats)
	      : SingleStringName (SingleString), SingleIntName (SingleInt), SingleFloatName (SingleFloat),
	        MultipleStringsNames (MultipleStrings), MultipleIntsNames (MultipleInts), MultipleFloatsNames (MultipleFloats)
{}

// copy assignment: follow 'Exceptional C++'
CTSVFileConvertOutputs& CTSVFileConvertOutputs::operator= (const CTSVFileConvertOutputs& other)
{
	CTSVFileConvertOutputs temp (other);	// make new via default copy constructor
	swap (temp);				// and exchange contents
	return *this;
}

void CTSVFileConvertOutputs::swap (CTSVFileConvertOutputs& other)
{
	SingleStringName.swap (other.SingleStringName);
	SingleIntName.swap (other.SingleIntName);
	SingleFloatName.swap (other.SingleFloatName);
	MultipleStringsNames.swap (other.MultipleStringsNames);
	MultipleIntsNames.swap (other.MultipleIntsNames);
	MultipleFloatsNames.swap (other.MultipleFloatsNames);
}

void CTSVFileConvertOutputs::Clear()
{
	SingleStringName = "";
	SingleIntName = "";
	SingleFloatName = "";
	MultipleStringsNames.clear();
	MultipleIntsNames.clear();
	MultipleFloatsNames.clear();
}

//////////////////////////////////////////////////////////////////////

CTSVFileConvertData::CTSVFileConvertData (std::string* SingleStringPtr, std::vector<std::string>* MultipleStringsPtr,
	    int32_t* SingleIntPtr, std::vector<int32_t>* MultipleIntsPtr,
	    float* SingleFloatPtr, std::vector<float>* MultipleFloatsPtr)
	      : SingleStringPtr (SingleStringPtr), SingleIntPtr (SingleIntPtr), SingleFloatPtr (SingleFloatPtr),
	      MultipleStringsPtr (MultipleStringsPtr), MultipleIntsPtr (MultipleIntsPtr), MultipleFloatsPtr (MultipleFloatsPtr)
{}

//////////////////////////////////////////////////////////////////////

CTSVFileColumnHeader::~CTSVFileColumnHeader()
{
	RemoveAll();
}

void CTSVFileColumnHeader::RemoveAll()
{
	if (!m_ColumnIndices.empty())
		m_ColumnIndices.clear();
	if (!m_ColumnNames.empty())
		m_ColumnNames.clear();
}

bool CTSVFileColumnHeader::Append(const char* str)
{
	assert(str != NULL);
	std::string header = str;
	int index = (int) m_ColumnNames.size();
	m_ColumnNames.push_back(header);
	std::transform(header.begin(), header.end(), header.begin(), tolower);
	if (!m_ColumnIndices.insert(std::make_pair(header, index)).second)
	{
		return false;
	}
	return true;
}

std::string CTSVFileColumnHeader::GetColumnName(int32_t index)
{
	assert((index >= 0) && (index < (int32_t) m_ColumnNames.size()));
	std::string header = m_ColumnNames.at(index);
	return header;
}

int32_t CTSVFileColumnHeader::GetColumnIndex(const char* str)
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

void CTSVFileColumnHeader::GetColumnNames(std::vector<std::string>& names)
{
	names = m_ColumnNames;
}

//////////////////////////////////////////////////////////////////////

CTSVFileData::CTSVFileData()
{
	m_Error = "";
	m_FileName = "";
	m_nMode = TSV_STREAM_MODE;
	m_Key = "";
	m_FileType = TSV_FILE;
	m_Eof = false;
	m_Buffer = new char[BUFFER_SIZE];
	m_BufferSize = 0;
	m_BufferPos = 0;
	m_SingleStringIndex = -1;
	m_SingleIntIndex = -1;
	m_SingleFloatIndex = -1;
	m_MultipleStringsIndices = std::vector<int32_t>();
	m_MultipleIntsIndices = std::vector<int32_t>();
	m_MultipleFloatsIndices = std::vector<int32_t>();
}

CTSVFileData::~CTSVFileData()
{
	Close();
	delete [] m_Buffer;
}

//////////////////////////////////////////////////////////////////////

void CTSVFileData::SetError(const char* str)
{
	assert(str != NULL);
	assert(strlen(str) > 0);
	m_Error = str;
}

void CTSVFileData::SetReadMode(const int16_t mode)
{
	assert((mode >= TSV_INDEX_MODE) && (mode <= TSV_HASH_MODE)); 
	m_nMode = mode;
}

void CTSVFileData::SetFileName(const char *str)
{
	assert(str != NULL);
	assert(strlen(str) > 0);
	m_FileName = str;
}

void CTSVFileData::SetKey(const char* str)
{
	assert(str != NULL);
	assert(strlen(str) > 0);
	std::string s = str;
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	m_Key = s.c_str();
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::Exists()
{
	assert(m_FileName != "");

	// Find the file stats.
	struct stat st;
	return ((stat(m_FileName.c_str(), &st) == 0)? true: false);
}

//////////////////////////////////////////////////////////////////////

int32_t CTSVFileData::GetFileSize()
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

void CTSVFileData::GetMetaTagNames(std::vector<std::string>& names)
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

void CTSVFileData::GetMetaTagValues(const char* name, std::vector<std::string>& values)
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

void CTSVFileData::GetColumnHeader(std::vector<std::string>& header)
{
	m_ColumnHeader.GetColumnNames(header); 
}

bool CTSVFileData::NameInColumnHeader(const char* name)
{
	return (m_ColumnHeader.GetColumnIndex(name) >= 0); 
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::ReadBuffer()
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

std::string CTSVFileData::GetLine()
{
	assert(m_FileIn.is_open());
	std::string line = "";
	int32_t endPos = 0;
	bool bValid = false;
	bool bLineFeed = false;
	
	while (!bValid)
	{
		if (!m_Eof && ((m_BufferSize == 0) || (m_BufferPos >= m_BufferSize)))
		{
			bool bReturn = ReadBuffer();
			if (!bReturn)	// avoid compiler warning
				assert(bReturn);
		}
		else if (m_Eof && (m_BufferPos >= m_BufferSize))
		{
			line = Trim(line);
			break;
		}

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
			line.append(m_Buffer + m_BufferPos, endPos - m_BufferPos);
			// avoid Trim() if an incomplete line wraps into a new buffer
			if (bLineFeed || endPos != (BUFFER_SIZE - 1))
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

std::string CTSVFileData::Trim(const std::string& str) 
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

std::string CTSVFileData::TrimDoubleQuote(const std::string& str) 
{
	int len = (int) str.length();
	std::string temp = str;

	if (len > 0)
	{
		if (str.at(0) == '\"')
		{
			temp = str.substr(1);
			len = (int) temp.length();
		}
		if (temp.at(len - 1) == '\"')
			temp = temp.substr(0, len - 1);
	}
	return temp;
}
//////////////////////////////////////////////////////////////////////

void CTSVFileData::ParseMetaTags()
{
	int32_t pos = m_BufferPos;
	std::string line = GetLine();
	assert(line != "");
	std::string::size_type index = std::string::npos;
	// a line beginning with '#%' contains a metatag; any other line
	// beginning with '#' is a comment
	unsigned int len;
	while (((len = line.length()) >= 1) && (line[0] == '#'))
	{
		if ((len >= 2) && (line[1] == '%'))
		{
			index = line.find('=');
			if (index != std::string::npos)
			{
				std::string key = line.substr(2, index - 2);
				std::string value = line.substr(index + 1);
				std::transform(key.begin(), key.end(), key.begin(), tolower);
				m_MetaTags.insert(std::make_pair(key, value));
				if (key == "netaffx-annotation-csv-version")
					SetFileType(NETAFFX_CSV_FILE);
			}
		}
		pos = m_BufferPos;
		line = GetLine();
	}
	m_BufferPos = pos;
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::ParseOutput()
{
	if (m_Outputs.SingleStringName != "")
	{
		m_SingleStringIndex = m_ColumnHeader.GetColumnIndex (m_Outputs.SingleStringName.c_str());
		if (m_SingleStringIndex < 0)
		{
			m_Error = "No match for requested column name " + m_Outputs.SingleStringName;
			return false;
		}
	}
	if (m_Outputs.SingleIntName != "")
	{
		m_SingleIntIndex = m_ColumnHeader.GetColumnIndex (m_Outputs.SingleIntName.c_str());
		if (m_SingleIntIndex < 0)
		{
			m_Error = "No match for requested column name " + m_Outputs.SingleIntName;
			return false;
		}
	}
	if (m_Outputs.SingleFloatName != "")
	{
		m_SingleFloatIndex = m_ColumnHeader.GetColumnIndex (m_Outputs.SingleFloatName.c_str());
		if (m_SingleFloatIndex < 0)
		{
			m_Error = "No match for requested column name " + m_Outputs.SingleFloatName;
			return false;
		}
	}
	const int32_t MultipleStringsCount = m_Outputs.MultipleStringsNames.size();
	for (int32_t i = 0; i < MultipleStringsCount; ++i)
	{
		int32_t j = m_ColumnHeader.GetColumnIndex (m_Outputs.MultipleStringsNames[i].c_str());
		if (j < 0)
		{
			m_Error = "No match for requested column name " + m_Outputs.MultipleStringsNames[i];
			return false;
		}
		m_MultipleStringsIndices.push_back (j);
	}
	const int32_t MultipleIntsCount = m_Outputs.MultipleIntsNames.size();
	for (int32_t i = 0; i < MultipleIntsCount; ++i)
	{
		int32_t j = m_ColumnHeader.GetColumnIndex (m_Outputs.MultipleIntsNames[i].c_str());
		if (j < 0)
		{
			m_Error = "No match for requested column name " + m_Outputs.MultipleIntsNames[i];
			return false;
		}
		m_MultipleIntsIndices.push_back (j);
	}
	const int32_t MultipleFloatsCount = m_Outputs.MultipleFloatsNames.size();
	for (int32_t i = 0; i < MultipleFloatsCount; ++i)
	{
		int32_t j = m_ColumnHeader.GetColumnIndex (m_Outputs.MultipleFloatsNames[i].c_str());
		if (j < 0)
		{
			m_Error = "No match for requested column name " + m_Outputs.MultipleFloatsNames[i];
			return false;
		}
		m_MultipleFloatsIndices.push_back (j);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

void CTSVFileData::GetColumns(std::vector<std::string>& columns)
{
	std::string delimiter = "\t";
	if (IsNetAffxCSV()) delimiter = "\",\"";
	columns.clear();

	std::string line;
	do {
		line = GetLine();
	} while ( (( line.length() == 0) || (line[0] == '#')) && (!m_Eof || (m_BufferPos < m_BufferSize)) );

	if (!((line == "")||(line[0] == '#')))
	{
		std::string::size_type start = 0;
		std::string::size_type end = line.find(delimiter, start); 
		if((end == std::string::npos) && !IsNetAffxCSV())
		{
			printf("WARNING: SETTING AUTO CSV MODE on file %s (line '%s')\n",m_FileName.c_str(), line.c_str());
			delimiter = "\",\"";
			SetFileType(NETAFFX_CSV_FILE);
			end = line.find(delimiter, start); 
		}
		while (end != std::string::npos)
		{
			std::string column = line.substr(start, end - start);
			column = TrimDoubleQuote(column);
			columns.push_back(column);
			start = end + delimiter.length();
			end = line.find(delimiter, start); 
		}
		std::string column = line.substr(start);
		column = TrimDoubleQuote(column);
		columns.push_back(column);
	}
}

bool CTSVFileData::ParseColumnHeader()
{
	std::vector<std::string> columns;
	GetColumns(columns);
	assert(columns.size() > 0);
	for (std::vector<std::string>::iterator i = columns.begin(); i != columns.end(); i++) 
	{
		if(!m_ColumnHeader.Append(i->c_str()))
		{
			std::string ColumnName(i->c_str());
			m_Error = "Column name " + ColumnName + " is a duplicate";
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::Open(const char* fileName, const short mode, const char* filetype, const char* key,
		const CTSVFileConvertOutputs* outputs)
{
	bool bSuccessful = true;

	if (strlen(fileName) == 0 || fileName == NULL)
	{
		m_Error = "Please provide a file name";
		return false;
	}
	if (((mode == TSV_INDEX_MODE) || (mode == TSV_HASH_MODE)) && (key == NULL))
	{
		m_Error = "Please provide a key when using INDEX mode or HASH mode";
		return false;
	}

	SetFileName(fileName);
	SetReadMode(mode);
	if (mode != TSV_STREAM_MODE) SetKey(key);
	std::string type = filetype;
	std::transform(type.begin(), type.end(), type.begin(), tolower);
	if ((type == "csv") || (type == "netaffx-csv"))
		SetFileType(NETAFFX_CSV_FILE);
	else if (type == "tab")
		SetFileType(TSV_FILE);
	else
	{
		m_Error = "Unknown file type (" + type + ") for ParseTSV of file" + m_FileName;
		return false;
	}
	if (outputs)
		SetOutputs (*outputs);

	switch(m_nMode)
	{
	case TSV_STREAM_MODE:
		bSuccessful = StreamOpen();
		break;
	case TSV_HASH_MODE:
		bSuccessful = HashOpen();
		break;
	case TSV_INDEX_MODE:
		bSuccessful = IndexOpen();
		break;
	default:
		char sz[100];
		snprintf(sz,sizeof(sz), "Unknown open mode %d for parsing TSV", m_nMode);
		m_Error = sz;
		bSuccessful = false;
		break;
	}
	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////

void CTSVFileData::Clear()
{
	memset(m_Buffer, 0, sizeof(char) * BUFFER_SIZE);
	m_BufferPos = 0;
	m_Eof = false;
	m_SingleStringIndex = -1;
	m_SingleIntIndex = -1;
	m_SingleFloatIndex = -1;
	m_MultipleStringsIndices.clear();
	m_MultipleIntsIndices.clear();
	m_MultipleFloatsIndices.clear();
	m_Outputs.Clear();
	m_MetaTags.clear();
	m_ColumnHeader.RemoveAll();
	m_KeyIndices.clear();
	for (std::multimap<std::string, std::vector<std::string>*>::iterator pos = m_Entries.begin(); pos != m_Entries.end(); pos++)
	{
		pos->second->clear();
		delete pos->second;
	}
	m_Entries.clear();
}

void CTSVFileData::Close()
{
	Clear();
	if (m_FileIn.is_open())
	{
		m_FileIn.clear();
		m_FileIn.close();
	}
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::StreamOpen()
{
	m_FileIn.open(m_FileName.c_str(), std::ios::in|std::ios::binary);

	// Check if open
	if (!m_FileIn)
	{
		m_Error = "Unable to open tsv layout file " + m_FileName;
		return false;
	}
	ParseMetaTags();
	if (!ParseColumnHeader())
	{
		if (m_Error == "")
			m_Error = "Unable to parse column headers";
		return false;
	}
	if (!ParseOutput())
	{
		if (m_Error == "")
			m_Error = "Unable to parse requested outputs";
		return false;
	}
	return true;
}

bool CTSVFileData::NextEntry(std::map<std::string, std::string>& data)
{
	if (m_nMode != TSV_STREAM_MODE)
	{
		m_Error = "Cannot call NextEntry when not in stream mode";
		return false;
	}
	if (!m_FileIn.is_open())
	{
		m_Error = "No TSV file is opened";
		return false;
	}

	return GetData(data);
}

bool CTSVFileData::NextEntryConverted (CTSVFileConvertData& data)
{
	if (m_nMode != TSV_STREAM_MODE)
	{
		m_Error = "Cannot call NextEntryConverted when not in stream mode";
		return false;
	}
	if (!m_FileIn.is_open())
	{
		m_Error = "No TSV file is opened";
		return false;
	}

	return GetDataConverted (data);
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::HashOpen()
{
	m_FileIn.open(m_FileName.c_str(), std::ios::in|std::ios::binary);

	// Check if open
	if (!m_FileIn)
	{
		m_Error = "Unable to open tsv layout file " + m_FileName;
		return false;
	}
	ParseMetaTags();
	if (!ParseColumnHeader())
	{
		if (m_Error == "")
			m_Error = "Unable to parse column headers";
		return false;
	}

	int keyColumn = m_ColumnHeader.GetColumnIndex(m_Key.c_str());
	if (keyColumn == -1)
	{
		m_Error = "Key " + m_Key + " is not found in file " + m_FileName;
		return false;
	}
	std::string keyValue = "";
	// int columnCount = m_ColumnHeader.GetColumnCount(); // unused
	int col = 0;
	std::vector<std::string> columns;
	GetColumns(columns);
	while (columns.size() > 0)
	{
		col = 0;
		std::vector<std::string>* values = new std::vector<std::string>();
		for (std::vector<std::string>::iterator pos = columns.begin(); pos != columns.end(); pos++, col++) 
		{
			if (col < m_ColumnHeader.GetColumnCount())
			{
				values->push_back(pos->c_str());
			}
			if (col == keyColumn)
				keyValue = pos->c_str();
		}
		if (m_ColumnHeader.GetColumnCount() > (int) columns.size())
		{
			for (col = (int) columns.size(); col < m_ColumnHeader.GetColumnCount(); col++)
			{
				values->push_back("");
			}
		}
		m_Entries.insert(std::make_pair(keyValue, values));
		
		GetColumns(columns);
	}
	m_FileIn.clear();
	m_FileIn.close();

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::IndexOpen()
{

	m_FileIn.open(m_FileName.c_str(), std::ios::in|std::ios::binary);

	// Check if open
	if (!m_FileIn)
	{
		m_Error = "Unable to open tsv layout file " + m_FileName;
		return false;
	}
	ParseMetaTags();
	if (!ParseColumnHeader())
	{
		if (m_Error == "")
			m_Error = "Unable to parse column headers";
		return false;
	}

	int keyColumn = m_ColumnHeader.GetColumnIndex(m_Key.c_str());
	if (keyColumn == -1)
	{
		m_Error = "Key " + m_Key + " is not found in file " + m_FileName;
		return false;
	}
	// std::multimap<std::string, std::streampos>::iterator pos; 
	std::string keyValue = "";
	// int columnCount = m_ColumnHeader.GetColumnCount(); // unused
	// int col = 0; // unused
	std::vector<std::string> columns;

	std::streampos posStart = m_BufferStartPos;
	std::streampos posLine = m_BufferStartPos;
	std::streamoff offset = m_BufferPos;
	posLine = posLine + offset;
	GetColumns(columns);
	while (columns.size() > 0)
	{
		keyValue = columns.at(keyColumn);
		m_KeyIndices.insert(std::make_pair(keyValue, posLine));
		
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

bool CTSVFileData::GetInfo(const std::string& key, std::vector<std::map<std::string, std::string> >& data)
{
	bool bSuccessful = false;

	switch(m_nMode)
	{
	case TSV_STREAM_MODE:
		m_Error = "Cannot call GetInfo when in stream mode";
		break;
	case TSV_HASH_MODE:
		bSuccessful = GetHashInfo(key, data);
		break;
	case TSV_INDEX_MODE:
		bSuccessful = GetIndexInfo(key, data);
		break;
	default:
		m_Error = "Cannot call GetInfo when not in index or hash mode";
		break;
	}

	return bSuccessful;
}

bool CTSVFileData::GetHashInfo(const std::string& key, std::vector<std::map<std::string, std::string> >& data)
{
	bool bSuccessful = false;

	std::vector<std::string>* entry = NULL;
	std::multimap<std::string, std::vector<std::string>*>::iterator posStart = m_Entries.find(key);
	std::multimap<std::string, std::vector<std::string>*>::iterator posEnd = m_Entries.upper_bound(key);
	if (posStart == m_Entries.end()) 
	{
		m_Error = "No data is associated with key " + key;
		bSuccessful = true;
	}
	else
	{
		data.clear();
		for (std::multimap<std::string, std::vector<std::string>*>::iterator pos = posStart; pos != posEnd; pos++)
		{
			int col = 0;
			entry = pos->second;
			std::map<std::string, std::string> columns;
			for (std::vector<std::string>::iterator posColumn = entry->begin(); posColumn != entry->end(); posColumn++, col++) 
			{
				if (col < m_ColumnHeader.GetColumnCount())
				{
					std::string colname = m_ColumnHeader.GetColumnName(col);
					std::string value = posColumn->c_str();
					std::transform(colname.begin(), colname.end(), colname.begin(), tolower);
					columns.insert(std::make_pair(colname, value));
				}
			}
			if (m_ColumnHeader.GetColumnCount() > (int) columns.size())
			{
				for (col = (int) columns.size(); col < m_ColumnHeader.GetColumnCount(); col++)
				{
					std::string colname = m_ColumnHeader.GetColumnName(col);
					std::string value = "";
					std::transform(colname.begin(), colname.end(), colname.begin(), tolower);
					columns.insert(std::make_pair(colname, value));
				}
			}
			data.push_back(columns);
		}
		bSuccessful = true;
	}

	return bSuccessful;
}

bool CTSVFileData::GetIndexInfo(const std::string& key, std::vector<std::map<std::string, std::string> >& data)
{
	bool bSuccessful = true;

	if (!m_FileIn.is_open())
	{
		m_Error = "Tsv layout file is not open: " + m_FileName;
		return false;
	}
	std::map<std::string, std::string>::iterator posKey;
	std::string keyValue = key;
	keyValue = Trim(keyValue);
	std::transform(keyValue.begin(), keyValue.end(), keyValue.begin(), tolower);
	std::multimap<std::string, std::streampos>::iterator posStart = m_KeyIndices.find(keyValue);
	std::multimap<std::string, std::streampos>::iterator posEnd = m_KeyIndices.upper_bound(keyValue);
	if (posStart == m_KeyIndices.end())
	{
		m_Error = "No data is associated with key " + key;
		bSuccessful = true;
	}
	else
	{
		data.clear();
		for (std::multimap<std::string, std::streampos>::iterator pos = posStart; bSuccessful && (pos != posEnd); pos++)
		{
			m_FileIn.clear();
			m_FileIn.seekg(pos->second);
			m_BufferSize = 0;
			m_Eof = false;
			std::map<std::string, std::string> columns;
			if (GetData(columns))
			{
				posKey = columns.find(m_Key);
				if ((posKey == columns.end()) || (posKey->second != keyValue))
				{
					m_Error = "Key mismatch: requested key [" + key;
					m_Error += "] does not match key in file [" + posKey->second;
					m_Error += "].";
					bSuccessful = false;
				}
				else
					data.push_back(columns);
			}
			else
			{
				m_Error = "Key " + key;
				m_Error += " not found in TSV file but is in index.";
				bSuccessful = false;
			}
		}
		if (data.size() > 0)
			bSuccessful = true;
		else
		{
			m_Error = "Key " + key;
			m_Error += " not found in TSV file but is in index.";
			bSuccessful = false;
		}
	}

	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::GetData(std::map<std::string, std::string>& data)
{
	std::vector<std::string> columns;
	GetColumns(columns);
	if (columns.empty())
		return false;

	data.clear();
	int col = 0;
	for (std::vector<std::string>::iterator pos = columns.begin(); pos != columns.end(); pos++, col++) 
	{
		if (col < m_ColumnHeader.GetColumnCount())
		{
			std::string colname = m_ColumnHeader.GetColumnName(col);
			std::string value = pos->c_str();
			std::transform(colname.begin(), colname.end(), colname.begin(), tolower);
			data.insert(std::make_pair(colname, value));
		}
	}
	if (m_ColumnHeader.GetColumnCount() > (int) columns.size())
	{
		for (col = (int) columns.size(); col < m_ColumnHeader.GetColumnCount(); col++)
		{
			std::string colname = m_ColumnHeader.GetColumnName(col);
			std::string value = "";
			std::transform(colname.begin(), colname.end(), colname.begin(), tolower);
			data.insert(std::make_pair(colname, value));
		}
	}
	return true;
}

void CTSVFileData::GetSubFields(const std::string& column, std::vector<std::string>& fields)
{
	std::string value = column;
	value = Trim(value);
	assert(value.length() > 0);

	std::string delimiter = " // ";

	fields.clear();
	std::string::size_type index = value.find(delimiter);
	while (index != std::string::npos)
	{
		std::string entry = value.substr(0, index);
		fields.push_back(entry);
		if ((index + delimiter.length()) < value.length())
		{
			value = value.substr(index + delimiter.length());
			index = value.find(delimiter);
		}
		else
			index = std::string::npos;
	}
	if (value != column)
		fields.push_back(value);
}

void CTSVFileData::GetSubEntries(const std::string& column, std::vector<std::string>& entries)
{
	std::string value = column;
	value = Trim(value);
	assert(value.length() > 0);

	std::string delimiter = " /// ";

	entries.clear();
	std::string::size_type index = value.find(delimiter);
	while (index != std::string::npos)
	{
		std::string entry = value.substr(0, index);
		entries.push_back(entry);
		if ((index + delimiter.length()) < value.length())
		{
			value = value.substr(index + delimiter.length());
			index = value.find(delimiter);
		}
		else
			index = std::string::npos;
	}
	if (value != column)
		entries.push_back(value);
}

//////////////////////////////////////////////////////////////////////

bool CTSVFileData::GetDataConverted (CTSVFileConvertData& data)
{
	std::vector<std::string> columns;

	GetColumns(columns);
	if (columns.empty())
		return false;

	// caller requests a single string value
	if (m_SingleStringIndex >= 0)
	{
		assert (data.SingleStringPtr);	// need somewhere to put it
		*(data.SingleStringPtr) = columns [m_SingleStringIndex];	// copy
	}
	// caller requests a single int value
	if (m_SingleIntIndex >= 0)
	{
		assert (data.SingleIntPtr);
		int32_t i;
		if (! toInt (columns [m_SingleIntIndex].c_str(), i) )	// call data conversion routine
		{
			m_Error = "Could not convert '" + columns [m_SingleIntIndex] + "' to an int";
			return false;
		}
		*(data.SingleIntPtr) = i;
	}
	// caller requests a single float value
	if (m_SingleFloatIndex >= 0)
	{
		assert (data.SingleFloatPtr);
		float f;
		if (! toFloat (columns[m_SingleFloatIndex].c_str(), f) )
		{
			m_Error = "Could not convert '" + columns [m_SingleFloatIndex] + "' to a float";
			return false;
		}
		*(data.SingleFloatPtr) = f;
	}
	// caller requests multiple strings
	const uint32_t StringsIndicesCount = m_MultipleStringsIndices.size();
	if (StringsIndicesCount)
	{
		// caller must provide a pointer to a vector of the same size as the requested data
		assert (data.MultipleStringsPtr);
		assert (StringsIndicesCount == data.MultipleStringsPtr->size());
		for (uint32_t i = 0; i < StringsIndicesCount; ++i)
			(*data.MultipleStringsPtr)[i] = columns [m_MultipleStringsIndices[i]];
	}
	// caller requests multiple ints
	const uint32_t IntsIndicesCount = m_MultipleIntsIndices.size();
	if (IntsIndicesCount)
	{
		assert (data.MultipleIntsPtr);
		assert (IntsIndicesCount == data.MultipleIntsPtr->size());
		for (uint32_t i = 0; i < IntsIndicesCount; ++i)
		{
			int32_t j;
			if (! toInt (columns [m_MultipleIntsIndices[i]].c_str(), j) )
			{
				m_Error = "Could not convert '" + columns [m_MultipleIntsIndices[i]] + "' to an int";
				return false;
			}
			(*data.MultipleIntsPtr)[i] = j;
		}
	}
	// caller requests multiple floats
	const uint32_t FloatsIndicesCount = m_MultipleFloatsIndices.size();
	if (FloatsIndicesCount)
	{
		assert (data.MultipleFloatsPtr);
		assert (FloatsIndicesCount == data.MultipleFloatsPtr->size());
		for (uint32_t i = 0; i < FloatsIndicesCount; ++i)
		{
			float f;
			if (! toFloat (columns [m_MultipleFloatsIndices[i]].c_str(), f) )
			{
				m_Error = "Could not convert '" + columns [m_MultipleFloatsIndices[i]] + "' to a float";
				return false;
			}
			(*data.MultipleFloatsPtr)[i] = f;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

namespace affxtsv
{

bool toInt (const char *num, int32_t& i)
{
	assert (num); 
	errno = 0;
	char *end = NULL;
	int64_t l = strtol (num, &end, 10);

	bool ok = true;
	// end will be NULL if entire string converted.
	if (*end != '\0')
		ok = false;

	// cast to int and make sure that we didn't overflow
	i = (int32_t)l;
	if (errno != 0 || (int64_t)i != l) 
		ok = false;

	// if we had a problem set to 0 for consistency.
	if (!ok)
		i = 0;

	return ok;
}

bool toFloat (const char *num, float& f)
{
	assert (num); 
	errno = 0;
	char *end = NULL;

	// Solaris 5.8 doesnt have "strtof", so we use "strtod"
	// and convert it to a float.
	f = (float) strtod (num, &end);

	bool ok = true;
	// end will point at NULL if conversion successful
	if (*end != '\0')
		ok = false;

	// errno should be set if over/under flow.
	if (errno != 0)
		ok = false;

	if (!ok)
		f = 0.0;

	return ok;
}

//////////////////////////////////////////////////////////////////////

} // namespace
