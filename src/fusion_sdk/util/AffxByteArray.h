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

#ifndef _AffxByteArray_h_
#define _AffxByteArray_h_

/**
 * @file AffxByteArray.h
 *
 * @brief This header contains the AffxByteArray class definition.
 */
#include "util/AffxConv.h"
#include "util/AffxString.h"
//

/**
 * @brief  A class for managing large byte arrays. 
 * 
 */
class AffxByteArray
{
public:
	void lockBuffer(void) {m_bLocked = true;}
	void unlockBuffer(void) {m_bLocked = false;}
	bool isLocked(void) {return m_bLocked;}
	void clear(void);
	void empty(void);
	bool isEmpty(void) {return (m_nMaxSize == 0);}
	bool isClear(void) {return (m_nSize == 0);}
	const AffxByteArray& operator=(const AffxByteArray& ba);
	int getInt(int &iIndex);
	bool isAllWhitespace();
	void replace(const AffxString& str1, const AffxString& str2);
	void replace(char c1, char c2);

// Construction
	AffxByteArray();
	AffxByteArray(const AffxByteArray& ba);
	AffxByteArray(const AffxString& ba);

// Attributes
	int getAllocLength() const;
	int getSize() const;
	int getUpperBound() const;
	void setSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void freeExtra();
	void removeAll();

	// Accessing elements
	char getAt(int nIndex) const;
	void setAt(int nIndex, char newElement);
	char& elementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const char* getData() const;
	char* getData();

	// Potentially growing the array
	void setAtGrow(int nIndex, char newElement);
	int add(char newElement);
	int append(const AffxByteArray& src);
	int append(const AffxString& src);
	int append(int i) {return append(::getInt(i));}
	int append(unsigned int i) {return append(::getUnsignedInt(i));}
	int append(double d) {return append(::getDouble(d, 6));}
	AffxByteArray& append(char bytes[], int offset, int len);
	void copy(const AffxByteArray& src);
	void assign(const AffxString& str) {setSize(0); append(str);}

	// overloaded operator helpers
	char operator[](int nIndex) const;
	char& operator[](int nIndex);

	// Operations that move elements around
	void insertAt(int nIndex, char newElement, int nCount = 1);
	void removeAt(int nIndex, int nCount = 1);
	void insertAt(int nStartIndex, AffxByteArray* pNewArray);
	
	bool operator==(const AffxByteArray &ba);
//	bool operator==(AffxByteArray& ba) {return operator==(ba);}
	bool operator!=(const AffxByteArray& ba) {return !(operator==(ba));}
	

	void copy(int iDestStart, const AffxByteArray& src, int iSrcStart, int iLength);
	void setOffset(int iOffset);
	bool getLine(AffxByteArray& ba);
	int getColumnCount();
	AffxByteArray& getColumn(int iColumnNumber, AffxByteArray& ba);
	int parseInt() const;
	char parseChar() const;
	bool parsebool() const;
	double parseDouble() const;
	AffxByteArray& trim();
	AffxByteArray& trimTabs();
	AffxString toString() const; 
	bool startsWith(const AffxString& strCompare);
	AffxString substring(int iIndex, int iEndIndex = -1);
	AffxByteArray& toLowerCase();
	AffxByteArray& toUpperCase();
	void trimInternal();
	AffxString firstWord(int iOffset = 0);
	AffxByteArray reverseComplement();
	bool equals(const AffxString &str) const;
	bool equalsIgnoreCase(const AffxString &str);
	int length() const {return getSize();}
	int getLength() const {return getSize();}
	bool nextLine(AffxByteArray& ba);
	int getWordCount();
	AffxString getWord(int iWordIndex);
	
	int parameterCount();
	AffxByteArray getParameter(int iParameterIndex);
	int csvParameterCount();
	AffxByteArray getCsvParameter(int iParameterIndex);
	int append(char by);

	AffxByteArray getWord(int iWordIndex, AffxByteArray& ba);
    int indexOf(const AffxString& str) {return toString().indexOf(str);}
	
	bool isValid() {return isValid(false);}
	bool isValid(bool bParameterFile);
	AffxByteArray nextColumn(AffxByteArray& ba);
	int compareTo(const AffxString &that) const ;
	int compareTo(const AffxByteArray &that) const;
	int compareTo(const AffxByteArray &obj, int iCompareCode) const;
	
	int getMaxRun(char byBase);
	int getCountInWindow(char byBase, int iWindowSize);
	int getCount(char byBase);
	AffxByteArray nextField(AffxByteArray& ba);

        bool readFile(const AffxString& strFileName);

// Implementation
protected:
	char* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

	bool m_bLocked;
	int m_iOffset;
	int m_iFieldOffset;

public:
	~AffxByteArray();

#ifdef _DEBUG
	void _ASSERTEValid() const;
#endif

protected:
	// local typedefs for class templates
	typedef char BASE_TYPE;
	typedef char BASE_ARG_TYPE;

private:
	bool getLine(int iStart, int iIndex, int iLength, AffxByteArray& ba, bool bRemoveLastByte);
	bool nextColumn(int iStart, int iIndex, int iLength, AffxByteArray& ba);
	bool nextField(int iStart, int iIndex, int iLength, AffxByteArray& ba);
};


#endif // _AffxByteArray_h_
