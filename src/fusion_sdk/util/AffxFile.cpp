////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
////////////////////////////////////////////////////////////////

/**
 * @file AffxFile.cpp
 *
 * @brief This file contains the AffxFile class members. 
 */

#include "util/AffxFile.h"
//
#include "util/AffxArray.h"
//
#include <cstring>
#include <string>
//

using namespace std;

enum AffxFile::m_enumOpenOptions;
int AffxFile::INVALIDATED = -2;
int AffxFile::UNMARKED = -1;

// Constructor.
AffxFile::AffxFile()
{
	m_iBufferSize = 1000000;
	m_cb = NULL;	
	m_icbLength = 0;
    m_iMarkedChar = UNMARKED;
    m_iReadAheadLimit = 0; 
    m_bSkipLF = false;
    m_bMarkedSkipLF = false;	
    m_iChars = 0;
	m_iNextChar = 0;
	m_pfileStream = NULL;
}

// Destructor.
AffxFile::~AffxFile()
{
	if (isOpen()) 
	{
		m_pfileStream->close();
		delete m_pfileStream;
	}
	if (m_cb != NULL) {delete[] m_cb;}
}

bool AffxFile::isHdf5(const AffxString& strFileName)
{
	AffxFile file;
	if (!file.open(strFileName, AffxFile::LOAD)) {return false;}
	unsigned int ui = 0;
	char c1 = 0;
	char c2 = 0;
	char c3 = 0;
	file.read(&c1, 1, ui);
	file.read(&c1, 1, ui);
	file.read(&c2, 1, ui);
	file.read(&c3, 1, ui);
	file.close();
	return ((c1 == 'H') && (c2 == 'D') && (c3 == 'F'));
}

// Open the file.
bool AffxFile::open(const AffxString& strFileName, m_enumOpenOptions enumOpenOption) 
{
	m_pfileStream = new std::fstream;
	if (enumOpenOption == LOAD) {m_pfileStream->open(Util::convertPathName(strFileName).c_str(), fstream::in | fstream::binary);}
	else if (enumOpenOption == SAVE) {m_pfileStream->open(Util::convertPathName(strFileName).c_str(), fstream::out | fstream::binary | fstream::trunc);}
	else if (enumOpenOption == APPEND) {m_pfileStream->open(Util::convertPathName(strFileName).c_str(), fstream::out | fstream::binary | fstream::app);}
	if (!isOpen()) {delete m_pfileStream; m_pfileStream = NULL;}
	return isOpen();
}

// Get the file size in bytes.
unsigned int AffxFile::getFileSize(const AffxString& strFileName)
{
	unsigned int uiSize = 0;

	AffxFile file;
	if (file.open(strFileName, LOAD))
	{ 
		file.m_pfileStream->seekg (0, ios::end);
		uiSize = (unsigned int)file.m_pfileStream->tellg();
		file.close();
	}

	return uiSize;
}

// Read a the entire file into a byte array.
bool AffxFile::readFile(const AffxString& strFileName, AffxByteArray& ba)
{
	AffxFile file;
	bool bSuccessful = false;
	unsigned int uiNumberBytes = getFileSize(strFileName);

	if (file.open(strFileName, LOAD))
	{
		ba.setSize(uiNumberBytes);
		if (uiNumberBytes > 0)
		{
			file.m_pfileStream->read(ba.getData(), uiNumberBytes);
			bSuccessful = true;
		}
		file.close();
	}
	ba.setOffset(0);

	return bSuccessful;
}

// Read a the entire file into a byte array.
bool AffxFile::readFile(const AffxString& strFileName, void *p, unsigned int uiNumberBytes)
{
	AffxFile file;
	bool bSuccessful = false;
	unsigned int uiBytesRead = 0;

	if (file.open(strFileName, LOAD))
	{
		if (uiNumberBytes > 0)
		{
			file.read((char*)p, uiNumberBytes, uiBytesRead);
			bSuccessful = true;
		}
		file.close();
	}
	return bSuccessful;
}

// Create a new file from the contents of a byte array.
bool AffxFile::writeFile(const AffxString& strFileName, AffxByteArray& ba)
{
	AffxFile file;
	bool bSuccessful = false;
	int dwNumberBytes = 0;

	if (file.open(strFileName, SAVE))
	{
		dwNumberBytes = ba.getSize();

		if (dwNumberBytes > 0)
		{
			file.m_pfileStream->write(ba.getData(), dwNumberBytes);
			bSuccessful = true;
		}

		file.close();
	}

	return bSuccessful;
}

// Create a new file from the contents of a byte array.
bool AffxFile::writeFile(const AffxString& strFileName, void* p, unsigned int dwNumberBytes)
{
	AffxFile file;
	bool bSuccessful = false;
	if (file.open(strFileName, SAVE))
	{
		if (dwNumberBytes > 0)
		{
			file.m_pfileStream->write((char*)p, dwNumberBytes);
			bSuccessful = true;
		}
		file.close();
	}
	return bSuccessful;
}

void AffxFile::writeLine(const AffxString& str) 
{
	write(str.c_str());
#ifdef WIN32
	write("\r\n"); 
#else
	write("\n"); 
#endif
}

void AffxFile::writeLineFeed() 
{
#ifdef WIN32
	write("\r\n"); 
#else
	write("\n"); 
#endif
}

AffxString AffxFile::parseDirectory(const AffxString& strFilePath)
{
	AffxString strDirectory;
	AffxString strFileName;
	AffxString str = strFilePath;
	int iIndex = 0;
    bool absPath = false;

	while ((iIndex = findIndex(str)) != -1)
	{
        if(iIndex == 0)
            absPath = true;
		strDirectory += ( strDirectory.empty() ? "" : PATH_SEPARATOR) + str.substring(0, (iIndex));
		str = str.substring(iIndex + 1);
	}
	strFileName = str;

    if(absPath)
        strDirectory = PATH_SEPARATOR + strDirectory;
	return strDirectory;
}

int AffxFile::findIndex(AffxString& str)
{
	int iIndex = (int)str.find("\\");
	if (iIndex == -1) {iIndex = (int)str.find("/");}
	return iIndex;
}

AffxString AffxFile::parseFileName(const AffxString& strFilePath)
{
	AffxString strDirectory;
	AffxString strFileName;
	AffxString str = strFilePath;
	int iIndex = 0;

	while ((iIndex = findIndex(str)) != -1)
	{
		strDirectory += str.substring(0, (iIndex + 1));
		str = str.substring(iIndex + 1);
	}
	strFileName = str;

	return strFileName;
}

void AffxFile::read(char* p, unsigned int iLength, unsigned int& iBytesRead)
{
	memset(p, 0, iLength);
	m_pfileStream->read(p, iLength);
	if (p[iLength - 1] != 0) {iBytesRead = iLength;}
	else {iBytesRead = (unsigned int)strlen(p);}
}

unsigned int AffxFile::getRowCount(const AffxString& strFileName)
{
	unsigned int uiCount = 0;
	AffxFile file;
	AffxByteArray baLine;
	if (file.open(strFileName, AffxFile::LOAD))
	{
		while (file.readLine(baLine))
		{
			uiCount++;
		}
		file.close();
	}
	return uiCount;
}

/**
    * Fill the input buffer, taking the mark into account if it is valid.
    */
void AffxFile::fill() 
{
	if (m_cb == NULL) 
	{
		m_cb = new char[m_iBufferSize]; 
		memset(m_cb, 0, m_iBufferSize); 
		m_icbLength = m_iBufferSize;
	}
	int dst;
	if (m_iMarkedChar <= UNMARKED) {
		/* No mark */
		dst = 0;
	} else {
		/* Marked */
		int delta = m_iNextChar - m_iMarkedChar;
		if (delta >= m_iReadAheadLimit) {
		/* Gone past read-ahead limit: Invalidate mark */
		m_iMarkedChar = INVALIDATED;
		m_iReadAheadLimit = 0;
		dst = 0;
		} else {
		if (m_iReadAheadLimit <= m_icbLength) {
			/* Shuffle in the current buffer */
			memcpy(m_cb, ((char *)m_cb + m_iMarkedChar), delta); 
			m_iMarkedChar = 0;
			dst = delta;
		} else {
			/* Reallocate buffer to accomodate read-ahead limit */
			char* ncb = new char[m_iReadAheadLimit];
			memset(ncb, 0, m_iReadAheadLimit);
			memcpy(ncb, ((char *)m_cb + m_iMarkedChar), delta);
			delete[] m_cb;
			m_cb = ncb;
			m_iMarkedChar = 0;
			dst = delta;
		}
				m_iNextChar = m_iChars = delta;
		}
	}

	int n;
	unsigned int uiBytesRead = 0;
	read(m_cb + dst, m_icbLength - dst, uiBytesRead);
	if (uiBytesRead > 0)
	{
		n = uiBytesRead;
	}
	else {n = -1;}
	if (n > 0) 
	{
		m_iChars = dst + n;
		m_iNextChar = dst;
	}
}

bool AffxFile::readLine(AffxByteArray& baLine)
{	
	bool bEOF = false;
	baLine.setSize(0);
	baLine.setOffset(0);
int startChar;
bool ignoreLF = false;
bool omitLF = ignoreLF || m_bSkipLF;

//bufferLoop:
	for (;;) {

	if (m_iNextChar >= m_iChars)
		fill();
	if (m_iNextChar >= m_iChars) { /* EOF */
		return (baLine.length() > 0);
	}
	bool eol = false;
	char c = 0;
	int i;

            /* Skip a leftover '\n', if necessary */
	if (omitLF && (m_cb[m_iNextChar] == '\n'))
                m_iNextChar++;
	m_bSkipLF = false;
	omitLF = false;

	// charLoop:
 	for (i = m_iNextChar; i < m_iChars; i++) {
		c = m_cb[i];
		if ((c == '\n') || (c == '\r')) {
		eol = true;
		// break charLoop;
		break;
		}
	}

	startChar = m_iNextChar;
	m_iNextChar = i;

	if (eol) {
		baLine.append(m_cb, startChar, i - startChar);
		m_iNextChar++;
		if (c == '\r') {
		m_bSkipLF = true;
		}
//		return ((!(nextChar >= nChars)) || (baLine.length() > 0));
		return true;
	}
	baLine.append(m_cb, startChar, i - startChar);
	}
//		return (!bEOF);
	return ((!bEOF) || (baLine.length() > 0));
}


bool AffxFile::readLine()
{	
	bool bEOF = false;
int startChar;
bool ignoreLF = false;
bool omitLF = ignoreLF || m_bSkipLF;
int iCount = 0;

//bufferLoop:
	for (;;) {

	if (m_iNextChar >= m_iChars)
		fill();
	if (m_iNextChar >= m_iChars) { /* EOF */
		return (iCount > 0);
	}
	bool eol = false;
	char c = 0;
	int i;

            /* Skip a leftover '\n', if necessary */
	if (omitLF && (m_cb[m_iNextChar] == '\n'))
                m_iNextChar++;
	m_bSkipLF = false;
	omitLF = false;

	// charLoop:
 	for (i = m_iNextChar; i < m_iChars; i++) {
		c = m_cb[i];
		if ((c == '\n') || (c == '\r')) {
		eol = true;
		// break charLoop;
		break;
		}
	}

	startChar = m_iNextChar;
	m_iNextChar = i;

	if (eol) {
		iCount += (i - startChar);
//		baLine.append(cb, startChar, i - startChar);
		m_iNextChar++;
		if (c == '\r') {
		m_bSkipLF = true;
		}
//		return ((!(nextChar >= nChars)) || (baLine.length() > 0));
		return true;
	}
	iCount += (i - startChar);
//	baLine.append(cb, startChar, i - startChar);
	}
//		return (!bEOF);
	return ((!bEOF) || (iCount > 0));
}

void AffxFile::mkdir(const AffxString& str) 
{
	AffxString strDirectory;
	AffxString strTemp = str;
	int iIndex = 0;

	while ((iIndex = findIndex(strTemp)) != -1)
	{
		strDirectory += strTemp.substring(0, (iIndex + 1));
#ifdef WIN32
		_mkdir(strDirectory.c_str());
#else
		::mkdir(strDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
		strTemp = strTemp.substring(iIndex + 1);
	}
}

void AffxFile::rmdir(const AffxString& str)
{
#ifdef WIN32
	_rmdir(str.c_str());
#else
	::rmdir(str.c_str());
#endif
}

