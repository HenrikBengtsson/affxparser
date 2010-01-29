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

#ifndef _AffxFile_h_
#define _AffxFile_h_
/**
 * @file AffxFile.h
 *
 * @brief This header contains the AffxFile class definition. 
 */
#include "util/AffxByteArray.h"
#include "util/AffxString.h"
#include "util/Util.h"
//
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
//
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

template<class TYPE> class AffxArray;

/**
 * @brief  A class for managing text files. Used in association with the AffxByteArray class. 
 * 
 */
class AffxFile
{
public:
	enum m_enumOpenOptions{LOAD = 0, SAVE, APPEND}; 

public:
	static void mkdir(const AffxString& str);
	static void rmdir(const AffxString& str);
	static void remove(const AffxString& str) {Util::fileRemove(str.c_str());}
	static AffxString parseDirectory(const AffxString& str);
	static AffxString parseFileName(const AffxString& str);

	static unsigned int getFileSize(const AffxString& strFileName);
	static unsigned int getRowCount(const AffxString& strFileName);
	
	static bool readFile(const AffxString& strFileName, AffxByteArray& ba);
	static bool readFile(const AffxString& strFileName, void *p, unsigned int dwNumberBytes);
	static bool writeFile(const AffxString& strFileName, AffxByteArray& ba);
	static bool writeFile(const AffxString& strFileName, void* p, unsigned int dwNumberBytes);

	AffxFile();
	virtual ~AffxFile();

	bool open(const AffxString& strFileName, m_enumOpenOptions enumOpenOption);
	bool isOpen() {return ((m_pfileStream != NULL) && (m_pfileStream->is_open()));}
	
	void read(char* p, unsigned int iLength, unsigned int& iBytesRead);
	void write(const char *psz) {if (m_pfileStream != NULL) {m_pfileStream->write(psz, (std::streamsize)strlen(psz));}}

	bool readLine(AffxByteArray& baLine);
	bool readLine();
	void writeLine(const AffxString& str);
	void writeLineFeed() ; 

	void flush() {if (m_pfileStream != NULL) {m_pfileStream->flush();}}
	void close(void) 
	{
		if (isOpen()) 
		{
			m_pfileStream->close();	
			delete m_pfileStream; 
			m_pfileStream =  NULL;
			if (m_cb != NULL) {delete[] m_cb; m_cb = NULL;}
			m_icbLength = 0;
			m_iMarkedChar = UNMARKED;
			m_iReadAheadLimit = 0; 
			m_bSkipLF = false;
			m_bMarkedSkipLF = false;	
			m_iChars = 0;
			m_iNextChar = 0;
		}
	}
  // This function is ok if you dont want to link to the file5 library.
  // However, if you are, then it would be best to use:
  // int File5_File::isHdf5file(filename)
  // @todo Replace calls to isHdf5 with 'File5_File::isHdf5file'
	static bool isHdf5(const AffxString& strFileName);

protected:
	void fill();
	static int findIndex(AffxString& str);	

protected:	
    static int INVALIDATED;
    static int UNMARKED;

	std::fstream* m_pfileStream;
    char* m_cb;
	int m_icbLength;
    int m_iChars;
	int m_iNextChar;
	int m_iBufferSize;
    int m_iMarkedChar;
    int m_iReadAheadLimit; /* Valid only when m_iMarkedChar > 0 */
    bool m_bSkipLF; /** If the next character is a line feed, skip it */
    bool m_bMarkedSkipLF; /** The SkipLF flag when the mark was set */
};

#endif // _AffxFile_h_
