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

#ifndef _AffxBinaryFile_h_
#define _AffxBinaryFile_h_

/**
 * @file AffxBinaryFile.h
 *
 * @brief This header contains the AffxBinaryFile class definition.
 */

//
#include "util/AffxString.h"
//
#include <fstream>
#include <iostream>


/**
 * @brief  A class for managing binary files. 
 * 
 */
class AffxBinaryFile 
{
public:
	enum m_enumOpenOptions{LOAD = 0, SAVE, APPEND}; 

private:
  std::ifstream* m_pistrm;
  std::ofstream* m_postrm;

public:
	AffxBinaryFile();
	virtual ~AffxBinaryFile();

	bool open(const AffxString& strFileName, m_enumOpenOptions enumOpenOption);
	void close();
	
	bool isLittleEndian()
	{
#ifndef BYTE_ORDER
		return true;
#else
		return (BYTE_ORDER == LITTLE_ENDIAN);
#endif
	}

	unsigned int getOffset() {return (unsigned int)m_pistrm->tellg();}
	void setOffset(unsigned int ui) {m_pistrm->seekg(ui, std::ios::beg);}
	void advanceOffset(unsigned int ui) {setOffset(getOffset() + ui);}

	// All Affx binaries should be in Big Endian except for binary CDF file. 
	// This code is written from a Little Endian point of view. For x-platform use the Big Endian / Little Endian functions.
	double readDouble() {double d = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&d, sizeof(double));} return d;}
	float readFloat() {float f = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&f, sizeof(float));} return f;}
	int readInt() {int i = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&i, sizeof(int));} return i;}
	unsigned int readUnsignedInt() {unsigned int ui = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&ui, sizeof(unsigned int));}return ui;}
//	__int64 readLong64() {__int64 l = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&l, sizeof(__int64));} return l;}
	short readShort() {short n = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&n, sizeof(short));}return n;}
	unsigned short readUnsignedShort() {unsigned short un = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&un, sizeof(unsigned short));}return un;}
	char readChar() {char c = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&c, sizeof(char));}return c;}
	unsigned char readUnsignedChar() {unsigned char uc = 0; if (m_pistrm != NULL) {m_pistrm->read((char*)&uc, sizeof(unsigned char));}return uc;}

	AffxString readString()
	{
		AffxString str;
		int iStringLength = readInt();
		if (iStringLength > 0)
		{
			char* pszValue = new char[iStringLength + 1];
			m_pistrm->read(pszValue, iStringLength);
			pszValue[iStringLength] = 0;
			str = pszValue;
			delete[] pszValue;
		}
		return str;
	}

	AffxString readString(char* pszValue, int iStringLength)
	{
		AffxString str;
		if (iStringLength > 0)
		{
			m_pistrm->read(pszValue, iStringLength);
			pszValue[iStringLength] = 0;
			str = pszValue;
		}
		return str;
	}

	AffxString readString(int iStringLength)
	{
		AffxString str;
		if (iStringLength > 0)
		{
			char* pszValue = new char[iStringLength + 1];
			m_pistrm->read(pszValue, iStringLength);
			pszValue[iStringLength] = 0;
			str = pszValue;
			delete[] pszValue;
		}
		return str;
	}

	AffxString readWString(wchar_t* pszValue, int iStringLength)
	{
		AffxString str;
		if (iStringLength > 0)
		{
			for (int iIndex = 0; (iIndex < iStringLength); iIndex++)
			{
				unsigned short un = readUnsignedShort();
				*(pszValue + iIndex) = (wchar_t)un;
				str += (char)(un >> 8);
			}
			pszValue[iStringLength] = 0;
			delete[] pszValue;
		}
		return str;
	}

	AffxString readBigEndianHalfWString()
	{
		AffxString str;
		int iStringLength = readBigEndianInt();
		iStringLength = iStringLength / 2;
		if (iStringLength > 0)
		{
			wchar_t* pszValue = new wchar_t[iStringLength + 1];
			for (int iIndex = 0; (iIndex < iStringLength); iIndex++)
			{
				unsigned short un = readUnsignedShort();
				*(pszValue + iIndex) = (wchar_t)un;
				str += (char)(un >> 8);
			}
			pszValue[iStringLength] = 0;
			delete[] pszValue;
		}
		return str;
	}


	AffxString readHalfWString(int iStringLength)
	{
		AffxString str;
		iStringLength = iStringLength / 2;
		if (iStringLength > 0)
		{
			wchar_t* pszValue = new wchar_t[iStringLength + 1];
			for (int iIndex = 0; (iIndex < iStringLength); iIndex++)
			{
				unsigned short un = readUnsignedShort();
				*(pszValue + iIndex) = (wchar_t)un;
				str += (char)(un >> 8);
			}
			pszValue[iStringLength] = 0;
			delete[] pszValue;
		}
		return str;
	}

    short readBigEndianShort() 
	{
        int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
		if (isLittleEndian()) {i = ((ch1 << 8) + (ch2 << 0));}
		else  {i = ((ch1 << 0) + (ch2 << 8));}
        return (short)i;
    }

    unsigned short readBigEndianUnsignedShort() 
	{
        int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
		if (isLittleEndian()) {i = ((ch1 << 8) + (ch2 << 0));}
		else  {i = ((ch2 << 8) + (ch1 << 0));}
        return (unsigned short)i;
    }

    int readBigEndianInt() 
	{
		int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
        int ch3 = readUnsignedChar();
        int ch4 = readUnsignedChar();
		if (isLittleEndian()) {i = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));}
		else  {i = ((ch4 << 24) + (ch3 << 16) + (ch2 << 8) + (ch1 << 0));}      
        return i;
    }

    unsigned int readBigEndianUnsignedInt() 
	{
		unsigned int ui = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
        int ch3 = readUnsignedChar();
        int ch4 = readUnsignedChar();
		if (isLittleEndian()) {ui = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));}
		else  {ui = ((ch4 << 24) + (ch3 << 16) + (ch2 << 8) + (ch1 << 0));} 
        return ui;
    }

    float readBigEndianFloat() 
	{
		int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
        int ch3 = readUnsignedChar();
        int ch4 = readUnsignedChar();
		if (isLittleEndian()) {i = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));}
		else  {i = ((ch4 << 24) + (ch3 << 16) + (ch2 << 8) + (ch1 << 0));} 
		void* p = &i;
		return *(float*)p;
//      return (float)((float*)&i);
    }

	AffxString readBigEndianString()
	{
		AffxString str;
		int iStringLength = readBigEndianInt();
		if (iStringLength > 0)
		{
			char* pszValue = new char[iStringLength + 1];
			m_pistrm->read(pszValue, iStringLength);
			pszValue[iStringLength] = 0;
			str = pszValue;
			delete[] pszValue;
		}
		return str;
	}	

	AffxString readBigEndianString(int iStringLength)
	{
		AffxString str;
		readBigEndianInt();
		iStringLength -= 4;
		if (iStringLength > 0)
		{
			char* pszValue = new char[iStringLength + 1];
			m_pistrm->read(pszValue, iStringLength);
			pszValue[iStringLength] = 0;
			str = pszValue;
			delete[] pszValue;
		}
		return str;
	}	

	AffxString readBigEndianWString()
	{
		int iStringLength = readBigEndianInt();
		AffxString str;
		if (iStringLength > 0)
		{
			wchar_t* pszValue = new wchar_t[iStringLength + 1];
			for (int iIndex = 0; (iIndex < iStringLength); iIndex++)
			{
				unsigned short un = readUnsignedShort();
				*(pszValue + iIndex) = (wchar_t)un;
				str += (char)(un >> 8);
			}
			pszValue[iStringLength] = 0;
			delete[] pszValue;
		}
		return str;
	}

    short readLittleEndianShort() 
	{
        int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
		if (!isLittleEndian()) {i = ((ch1 << 8) + (ch2 << 0));}
		else  {i = ((ch1 << 0) + (ch2 << 8));}
        return (short)i;
    }

    unsigned short readLittleEndianUnsignedShort() 
	{
        int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
		if (!isLittleEndian()) {i = ((ch1 << 8) + (ch2 << 0));}
		else  {i = ((ch2 << 8) + (ch1 << 0));}
        return (unsigned short)i;
    }

    int readLittleEndianInt() 
	{
		int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
        int ch3 = readUnsignedChar();
        int ch4 = readUnsignedChar();
		if (!isLittleEndian()) {i = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));}
		else  {i = ((ch4 << 24) + (ch3 << 16) + (ch2 << 8) + (ch1 << 0));}      
        return i;
    }

    unsigned int readLittleEndianUnsignedInt() 
	{
		unsigned int ui = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
        int ch3 = readUnsignedChar();
        int ch4 = readUnsignedChar();
		if (!isLittleEndian()) {ui = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));}
		else  {ui = ((ch4 << 24) + (ch3 << 16) + (ch2 << 8) + (ch1 << 0));} 
        return ui;
    }

    float readLittleEndianFloat() 
	{
		int i = 0;
        int ch1 = readUnsignedChar();
        int ch2 = readUnsignedChar();
        int ch3 = readUnsignedChar();
        int ch4 = readUnsignedChar();
		if (!isLittleEndian()) {i = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));}
		else  {i = ((ch4 << 24) + (ch3 << 16) + (ch2 << 8) + (ch1 << 0));} 
		void* p = &i;
		return *(float*)p;
//        return (float)*((float*)&i);
    }

	AffxString readLittleEndianString()
	{
		AffxString str;
		int iStringLength = readLittleEndianInt();
		if (iStringLength > 0)
		{
			char* pszValue = new char[iStringLength + 1];
			m_pistrm->read(pszValue, iStringLength);
			pszValue[iStringLength] = 0;
			str = pszValue;
			delete[] pszValue;
		}
		return str;
	}

	void writeFloat(float f) {if (m_postrm != NULL) {m_postrm->write((char*)&f, sizeof(float));}}
	void writeInt(int i) {if (m_postrm != NULL) {m_postrm->write((char*)&i, sizeof(int));}}
	void writeUnsignedInt(unsigned int ui) {if (m_postrm != NULL) {m_postrm->write((char*)&ui, sizeof(unsigned int));}}
	void writeShort(short n) {if (m_postrm != NULL) {m_postrm->write((char*)&n, sizeof(short));}}
	void writeUnsignedShort(unsigned short un) {if (m_postrm != NULL) {m_postrm->write((char*)&un, sizeof(unsigned short));}}
	void writeChar(char c) {if (m_postrm != NULL) {m_postrm->write((char*)&c, sizeof(char));}}
	void writeUnsignedChar(unsigned char uc) {if (m_postrm != NULL) {m_postrm->write((char*)&uc, sizeof(unsigned char));}}

	void writeString(const AffxString& str)
	{
		int iStringLength = (int)str.length();
		writeInt(iStringLength);
		if (iStringLength > 0)
		{
			if (m_postrm != NULL) {m_postrm->write(str.c_str(), iStringLength);}
		}
	}

	void writeString(const char* pszBuffer, int iStringLength)
	{
		if (m_postrm != NULL) {m_postrm->write(pszBuffer, iStringLength);}
	}	

    void writeBigEndianShort(short i) 
	{
		if (isLittleEndian())
		{
			writeChar(((char*)&i)[1]);
			writeChar(((char*)&i)[0]);
		}
		else
		{
			writeChar(((char*)&i)[0]);
			writeChar(((char*)&i)[1]);
		}
    }

    void writeBigEndianUnsignedShort(unsigned short ui) 
	{
		if (isLittleEndian())
		{
			writeUnsignedChar(((unsigned char*)&ui)[1]);
			writeUnsignedChar(((unsigned char*)&ui)[0]);
		}
		else
		{
			writeUnsignedChar(((unsigned char*)&ui)[0]);
			writeUnsignedChar(((unsigned char*)&ui)[1]);
		}
    }

    void writeBigEndianInt(int i) 
	{
		if (isLittleEndian())
		{
			writeChar(((char*)&i)[3]);
			writeChar(((char*)&i)[2]);
			writeChar(((char*)&i)[1]);
			writeChar(((char*)&i)[0]);
		}
		else
		{
			writeChar(((char*)&i)[0]);
			writeChar(((char*)&i)[1]);
			writeChar(((char*)&i)[2]);
			writeChar(((char*)&i)[3]);
		}
    }

    void writeBigEndianUnsignedInt(unsigned int ui) 
	{
		if (isLittleEndian())
		{
			writeUnsignedChar(((unsigned char*)&ui)[3]);
			writeUnsignedChar(((unsigned char*)&ui)[2]);
			writeUnsignedChar(((unsigned char*)&ui)[1]);
			writeUnsignedChar(((unsigned char*)&ui)[0]);
		}
		else
		{
			writeUnsignedChar(((unsigned char*)&ui)[0]);
			writeUnsignedChar(((unsigned char*)&ui)[1]);
			writeUnsignedChar(((unsigned char*)&ui)[2]);
			writeUnsignedChar(((unsigned char*)&ui)[3]);
		}
    }

    void writeBigEndianFloat(float f) 
	{
		if (isLittleEndian())
		{
			writeChar(((char*)&f)[3]);
			writeChar(((char*)&f)[2]);
			writeChar(((char*)&f)[1]);
			writeChar(((char*)&f)[0]);
		}
		else
		{
			writeChar(((char*)&f)[0]);
			writeChar(((char*)&f)[1]);
			writeChar(((char*)&f)[2]);
			writeChar(((char*)&f)[3]);
		}
    }

	void writeBigEndianString(const AffxString& str)
	{
		int iStringLength = (int)str.length();
		writeBigEndianInt(iStringLength);
		if (iStringLength > 0)
		{
			if (m_postrm != NULL) {m_postrm->write(str.c_str(), iStringLength);}
		}
	}

    void writeLittleEndianShort(short i) 
	{
		if (!isLittleEndian())
		{
			writeChar(((char*)&i)[1]);
			writeChar(((char*)&i)[0]);
		}
		else
		{
			writeChar(((char*)&i)[0]);
			writeChar(((char*)&i)[1]);
		}
    }

    void writeLittleEndianUnsignedShort(unsigned short ui) 
	{
		if (!isLittleEndian())
		{
			writeUnsignedChar(((unsigned char*)&ui)[1]);
			writeUnsignedChar(((unsigned char*)&ui)[0]);
		}
		else
		{
			writeUnsignedChar(((unsigned char*)&ui)[0]);
			writeUnsignedChar(((unsigned char*)&ui)[1]);
		}
    }

    void writeLittleEndianInt(int i) 
	{
		if (!isLittleEndian())
		{
			writeChar(((char*)&i)[3]);
			writeChar(((char*)&i)[2]);
			writeChar(((char*)&i)[1]);
			writeChar(((char*)&i)[0]);
		}
		else
		{
			writeChar(((char*)&i)[0]);
			writeChar(((char*)&i)[1]);
			writeChar(((char*)&i)[2]);
			writeChar(((char*)&i)[3]);
		}
    }

    void writeLittleEndianUnsignedInt(unsigned int ui) 
	{
		if (!isLittleEndian())
		{
			writeUnsignedChar(((unsigned char*)&ui)[3]);
			writeUnsignedChar(((unsigned char*)&ui)[2]);
			writeUnsignedChar(((unsigned char*)&ui)[1]);
			writeUnsignedChar(((unsigned char*)&ui)[0]);
		}
		else
		{
			writeUnsignedChar(((unsigned char*)&ui)[0]);
			writeUnsignedChar(((unsigned char*)&ui)[1]);
			writeUnsignedChar(((unsigned char*)&ui)[2]);
			writeUnsignedChar(((unsigned char*)&ui)[3]);
		}
    }

    void writeLittleEndianFloat(float f) 
	{
		if (!isLittleEndian())
		{
			writeChar(((char*)&f)[3]);
			writeChar(((char*)&f)[2]);
			writeChar(((char*)&f)[1]);
			writeChar(((char*)&f)[0]);
		}
		else
		{
			writeChar(((char*)&f)[0]);
			writeChar(((char*)&f)[1]);
			writeChar(((char*)&f)[2]);
			writeChar(((char*)&f)[3]);
		}
    }

	void writeLittleEndianString(const AffxString& str)
	{
		int iStringLength = (int)str.length();
		writeLittleEndianInt(iStringLength);
		if (iStringLength > 0)
		{
			if (m_postrm != NULL) {m_postrm->write(str.c_str(), iStringLength);}
		}
	}
};

#endif
