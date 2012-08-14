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

/**
 * @file AffxByteArray.cpp
 *
 * @brief This file contains the AffxByteArray class members. 
 */

//
#include "util/AffxByteArray.h"
#include "util/Fs.h"
//
#include <cstring>
#include <limits>
#include <string>
//

using namespace std;

#define __minimum(a,b)(((a)<(b))?(a):(b))
#define __maximum(a,b)(((a)>(b))?(a):(b))

int AffxByteArray::getAllocLength() const
	{ return m_nMaxSize; }
int AffxByteArray::getSize() const
	{ return m_nSize; }
int AffxByteArray::getUpperBound() const
	{ return m_nSize-1; }
void AffxByteArray::removeAll()
	{ setSize(0); }
char AffxByteArray::getAt(int nIndex) const
{
		return m_pData[nIndex]; }
void AffxByteArray::setAt(int nIndex, char newElement)
	{ 
		m_pData[nIndex] = newElement; }
char& AffxByteArray::elementAt(int nIndex)
	{ 
		return m_pData[nIndex]; }
const char* AffxByteArray::getData() const
	{ return (const char*)m_pData; }
char* AffxByteArray::getData()
	{ return (char*)m_pData; }
int AffxByteArray::add(char newElement)
	{ int nIndex = m_nSize;
		setAtGrow(nIndex, newElement);
		return nIndex; }
char AffxByteArray::operator[](int nIndex) const
	{ return getAt(nIndex); }
char& AffxByteArray::operator[](int nIndex)
	{ return elementAt(nIndex); }

// Empty the data and free the memory allocation.
void AffxByteArray::empty()
{
	setSize(0);

	// shrink to nothing
	delete[] (char*)m_pData;
	m_pData = NULL;
	m_nSize = m_nMaxSize = 0;
	m_nSize = 0;
}

// Clear the data without freeing the memory allocation.
void AffxByteArray::clear()
{
	removeAt(0, m_nSize);
}

AffxByteArray::AffxByteArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;

	m_bLocked = false;
}

AffxByteArray::AffxByteArray(const AffxByteArray& ba)
{
	int iLength = ba.getSize();
	m_nSize = m_nMaxSize = iLength;
	m_nGrowBy = 0;
	m_bLocked = false;

//	m_pData = (char*) new char[iLength * sizeof(char)];
	m_pData = (char*) new char[iLength];
	memcpy(m_pData, ba.m_pData, (iLength * sizeof(char)));
}

AffxByteArray::AffxByteArray(const AffxString& str)
{
	AffxString str1 = str;
	int iLength = (int)str1.length();
	m_nSize = m_nMaxSize = iLength;
	m_nGrowBy = 0;
	m_bLocked = false;

	m_pData = (char*) new char[iLength * sizeof(char)];
	for (unsigned int iIndex = 0; (iIndex < str1.length()); iIndex++)
	{
		m_pData[iIndex] = (char)str1.charAt(iIndex);
	}
//	memcpy(m_pData, ba.m_pData, (iLength * sizeof(char)));
}

AffxByteArray::~AffxByteArray()
{
	empty();
}

void AffxByteArray::setSize(int nNewSize, int nGrowBy)
{
	// If locked do not reduce the size of the allocation.
	if (m_bLocked)
	{
		m_nSize = nNewSize;

		return;
	}

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
//		delete[] (char*)m_pData;
//		m_pData = NULL;
//		m_nSize = m_nMaxSize = 0;
		m_nSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
		m_pData = (char*) new char[nNewSize * sizeof(char)];

		memset(m_pData, 0, nNewSize * sizeof(char));  // zero fill

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements

			memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(char));

		}

		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = __minimum(1024, __maximum(4, m_nSize / 8));
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		char* pNewData = (char*) new char[nNewMax * sizeof(char)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(char));

		// construct remaining elements

		memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(char));


		// get rid of old stuff (note: no destructors called)
		delete[] (char*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

int AffxByteArray::append(const AffxString& srcIn)
{
	AffxString src = srcIn;

	int nOldSize = m_nSize;
	setSize(m_nSize + (int)src.length());

	memcpy(m_pData + nOldSize, src.c_str(), src.length() * sizeof(char));

	return nOldSize;
}

int AffxByteArray::append(const AffxByteArray& src)
{

	int nOldSize = m_nSize;
	setSize(m_nSize + src.m_nSize);

	memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(char));

	return nOldSize;
}

void AffxByteArray::copy(const AffxByteArray& src)
{
	setSize(src.m_nSize);

	memmove(m_pData, src.m_pData, src.m_nSize * sizeof(char));

}

void AffxByteArray::freeExtra()
{
	if (m_bLocked)
		return;

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
		char* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (char*) new char[m_nSize * sizeof(char)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(char));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (char*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

/////////////////////////////////////////////////////////////////////////////

void AffxByteArray::setAtGrow(int nIndex, char newElement)
{
	if (nIndex >= m_nSize)
		setSize(nIndex+1);
	m_pData[nIndex] = newElement;
}

void AffxByteArray::insertAt(int nIndex, char newElement, int nCount)
{
	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		setSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		setSize(m_nSize + nCount);  // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(char));

		// re-init slots we copied from

		memset(&m_pData[nIndex], 0, nCount * sizeof(char));

	}

	// insert new m_pData in the gap
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

void AffxByteArray::removeAt(int nIndex, int nCount)
{
	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)
		memcpy(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(char));
	m_nSize -= nCount;
}

void AffxByteArray::insertAt(int nStartIndex, AffxByteArray* pNewArray)
{
	if (pNewArray->getSize() > 0)
	{
		insertAt(nStartIndex, pNewArray->getAt(0), pNewArray->getSize());
		for (int i = 0; i < pNewArray->getSize(); i++)
			setAt(nStartIndex + i, pNewArray->getAt(i));
	}
}

// Assignment operator.
const AffxByteArray& AffxByteArray::operator=(const AffxByteArray& ba)
{
    if (this == &ba)
        return *this;

	empty();
	setSize(ba.getSize());
	copy(0, ba, 0, ba.getSize());
	return *this;
}

bool AffxByteArray::operator==(const AffxByteArray &ba)
{
	bool bEqual = false;

	int iThisSize = getSize();
	int iThatSize = ba.getSize();

	if (iThisSize == iThatSize)
	{
		for (int i = 0; (i < iThisSize); i++)
		{
			if (getAt(i) == ba.getAt(i))
				bEqual = true;
			else
			{
				bEqual = false;
				break;
			}
		}
	}

	return bEqual;
}

bool AffxByteArray::equals(const AffxString &str) const
{
	bool bEqual = false;

	int iThisSize = getSize();
	int iThatSize = (int)str.length();

	if (iThisSize == iThatSize)
	{
		if ((iThisSize == 0) && (iThatSize == 0)) {bEqual = true;}
		else
		{
			for (int i = 0; (i < iThisSize); i++)
			{
				if (getAt(i) == str.getAt(i))
					bEqual = true;
				else
				{
					bEqual = false;
					break;
				}
			}
		}
	}

	return bEqual;
}

bool AffxByteArray::equalsIgnoreCase(const AffxString &str)
{
	bool bEqual = false;

	int iThisSize = getSize();
	int iThatSize = (int)str.length();

	if (iThisSize == iThatSize)
	{
		for (int i = 0; (i < iThisSize); i++)
		{
			if (getAt(i) >= 'A' && getAt(i) <= 'Z')
			{
				if ((getAt(i) == str.getAt(i)) || (getAt(i) + ' ' == str.getAt(i)))
					bEqual = true;
				else
				{
					bEqual = false;
					break;
				}
			}
			else if (getAt(i) >= 'a' && getAt(i) <= 'z')
			{
				if ((getAt(i) == str.getAt(i)) || (getAt(i) - ' ' == str.getAt(i)))
					bEqual = true;
				else
				{
					bEqual = false;
					break;
				}
			}
			else if (getAt(i) == str.getAt(i))
				bEqual = true;
			else
			{
				bEqual = false;
				break;
			}
		}
	}

	return bEqual;
}

int AffxByteArray::getInt(int &iIndex)
{
	char buffer[64];
	short i = 0;
	int iSize = getSize();

	while ((iIndex < iSize) && (i < 63) && (isdigit(getAt(iIndex))))
	{
		buffer[i] = (char)(getAt(iIndex));
		iIndex++;
		i++;
	}
	buffer[i] = '\0';

	return atol(buffer);
}

void AffxByteArray::copy(int iDestStart, const AffxByteArray& src, int iSrcStart, int iLength)
{
	if (getSize() < (iDestStart + iLength))
		setSize(iDestStart + iLength);

	memmove((m_pData + iDestStart), (src.m_pData + iSrcStart), (iLength * sizeof(char)));
}

// Read one line from the byte array into a StringBuffer object.
void AffxByteArray::setOffset(int iOffset) {m_iOffset = iOffset;}
bool AffxByteArray::getLine(AffxByteArray& ba) 
{	 
	bool bSuccessful = false;
	int iLength = 0;
	ba.setSize(255);
	int iStart = m_iOffset;
	int iIndex = m_iOffset;
	
	if (getSize() > 0)
	{
	    int i = 0;
	    int iLast = 0;
    	    
	    while ((iIndex < getSize()) && ((i = (int)getAt(iIndex)) != 10))
	    {
	        iIndex++;
	        iLast = i;
	    }
	    
	    iLength = iIndex - iStart;
	    m_iOffset = m_iOffset + iLength + 1;
    	 
	    if ((iIndex >= getSize()) && (iLength == 0))
	    {
	        ba.setSize(0);
	    }
	    else
	    {	
	        bSuccessful = true;
	        
	        if ((i == 10) && (iLast == 13))
	        {
	            if (iLength > 0)
	                iLength--;
	        }
	        
	        ba.setSize(iLength);
	        ba.copy(0, *this, iStart, iLength);
	    }
   	}
   	
    return bSuccessful;
}	

// Count the number of columns separeted by tab characters.
int AffxByteArray::getColumnCount()
{
	int iColumnCount = 1;
	
	for (int i = 0; (i < getSize()); i++)
	{
	    if (getAt(i) == 9)
	    {
	        iColumnCount++;
	    }	        
	}
	
	return iColumnCount;
}

// Get the column m_pData.
AffxByteArray& AffxByteArray::getColumn(int iColumnNumber, AffxByteArray& ba)
{
	if (iColumnNumber == 0)
	{
	    ba.setSize(0);
	    return ba;
	}
	
	int iColumnCount = 1;
	int iStart = 0;
	int iLength = 0;
	
	for (int i = 0; (i < getSize()); i++)
	{
	    if (getAt(i) == 9)
	    {
	        iColumnCount++;
	        if (iColumnCount == iColumnNumber)
	        {
	            iStart = i + 1;
	            continue;
	        }
	    }
	    
	    if (iColumnCount > iColumnNumber)
	        break;
	    else if (iColumnCount == iColumnNumber)
	        iLength++;
	}
	
	if ((iStart >= getSize()) || (iLength == 0))
	    ba.setSize(0);
	else
	{
	    ba.setSize(iLength);
	    ba.copy(0, *this, iStart, iLength);
	}
	
	return ba;
}

char AffxByteArray::parseChar() const
{
	if (getLength() > 0) {return getAt(0);}
	else return 0;
}

// Convert the byte array into an int value.
int AffxByteArray::parseInt() const
{
  char *endPtr = NULL;
  return (int) strtol(this->toString().c_str(), &endPtr, 0);
}

bool AffxByteArray::parsebool() const
{
	bool b = false;
        AffxByteArray ba(*this);
        
        ba.toLowerCase();
        ba.trimTabs();
	if ((ba.equals("1")) || (ba.equals("y")) || (ba.equals("yes")) || (ba.equals("true")) || (ba.equals("t"))) {b = true;}
   	else if ((ba.equals("")) || (ba.equals("0")) || (ba.equals("n")) || (ba.equals("no")) || (ba.equals("false")) || (ba.equals("f"))) {b = false;}
	return b;
}

AffxByteArray& AffxByteArray::trimTabs()
{
    int i = 0;
    int iStart = 0;
    int iEnd = getSize();

    for (i = 0; (i < getSize()); i++)
    {
        if (getAt(i) <= ' ') {iStart++;}
        else {break;}
    }

    for (i = (getSize() - 1); (i >= 0); i--)
    {
        if (getAt(i) <= ' ') {iEnd--;}
        else {break;}
    }
    
    int iLength = iEnd - iStart;
    if (iLength <= 0)
    {
        setSize(0);
    }
    else
    {
	    copy(0, *this, iStart, iLength); 
	    setSize(iLength);
    }

	return *this;
}

AffxByteArray& AffxByteArray::trim()
{   
    int i = 0;
    int iStart = 0;
    int iEnd = getSize();
    int iLength = 0;
    
    for (i = 0; (i < getSize()); i++)
    {
        if ((getAt(i) <= 32) && (getAt(i) != 9))
            iStart++;
        else
            break;  
    }
    
    for (i = (getSize() - 1); (i >= 0); i--)
    {
        if ((getAt(i) <= 32) && (getAt(i) != 9))
            iEnd--;
        else
            break;  
    }
    
    iLength = iEnd - iStart;
    if (iLength <= 0)
    {
        setSize(0);
    }
    else
    {
	    copy(0, *this, iStart, iLength); 
	    setSize(iLength);
    }

	return *this;
}

AffxString AffxByteArray::toString() const
{
	AffxString str;

	if (getSize() > 0)
	{
		char *psz = (char *)malloc(getSize() + 1);
#if defined WIN32 && !defined WIN64
		strncpy_s(psz, getSize() + 1, (char *)getData(), getSize());
#else
		strncpy(psz, (char *)getData(), getSize());
#endif
		psz[getSize()] = 0;
		str = psz;
		free(psz);
	}

	return str;
}

bool AffxByteArray::startsWith(const AffxString& strCompare)
{
	bool bStartsWith = false;
	int iCompareLength = strCompare.getLength();

	if (getSize() >= iCompareLength)
	{
		bStartsWith = true;
		for (int iIndex = 0; (iIndex < iCompareLength); iIndex++)
		{
			if (getAt(iIndex) != strCompare.getAt(iIndex))
			{
				bStartsWith = false;
				break;
			}
		}
	}

	return bStartsWith;
}

AffxString AffxByteArray::substring(int iIndex, int iEndIndex)
{
	AffxByteArray ba;

	if (iEndIndex == -1)
		iEndIndex = getSize();

	int iLength = iEndIndex - iIndex;
	if (iLength < 0)
		iLength = 0;

	ba.setSize(iLength);
	ba.copy(0, *this, iIndex, iLength);

	return ba.toString();
}

AffxByteArray& AffxByteArray::toLowerCase()
{
	int iCount = getSize();
	char by = 0;
	int iOffset = 'a' - 'A';

	for (int iIndex = 0; (iIndex < iCount); iIndex++)
	{
		by = getAt(iIndex);
		if ((by >= 'A') && (by <= 'Z'))
		{
			by += iOffset;
			setAt(iIndex, by);
		}
	}

	return *this;
}

AffxByteArray& AffxByteArray::toUpperCase()
{
	int iCount = getSize();
	char by = 0;
	int iOffset = 'a' - 'A';

	for (int iIndex = 0; (iIndex < iCount); iIndex++)
	{
		by = getAt(iIndex);
		if ((by >= 'a') && (by <= 'z'))
		{
			by -= iOffset;
			setAt(iIndex, by);
		}
	}

	return *this;
}

void AffxByteArray::trimInternal()
{
	int iCount = getSize();
	char by = 0;

	for (int iIndex = 0; (iIndex < iCount); iIndex++)
	{
		by = getAt(iIndex);
        if (by <= 32)
		{
			copy(iIndex, *this, (iIndex + 1), (iCount - (iIndex + 1)));
			iCount--;
		}
	}

	setSize(iCount);
}

AffxString AffxByteArray::firstWord(int iOffset)
{
    int iIndex = 0;
    AffxByteArray ba;
    
    iIndex = iOffset;
	int iStartIndex = 0;
	int iNewSize = 0;
	int iCount = getSize();
    if (iOffset < iCount)
    {
        while ((iIndex < iCount) && (getAt(iIndex) <= 32))
            iIndex++;

		iStartIndex = iIndex;
                
        while ((iIndex < iCount) && (getAt(iIndex) > 32))
        {
            iIndex++;
			iNewSize++;
        }

		ba.setSize(iNewSize);
		ba.copy(0, *this, iStartIndex, iNewSize);
    }
    
    return ba.toString();
}
    
AffxByteArray AffxByteArray::reverseComplement()
{
	AffxByteArray ba = *this;
    AffxString strComplement = "tvghefcdijmlknopqysaabwxrz";
    char by = 0;
	int iCount = getSize();
	int iIndex = 0;
    
    ba.toLowerCase();
    if (((iCount / 2) * 2) != iCount)
    {
		iIndex = (iCount / 2);
        ba.setAt(iIndex, strComplement.getAt(ba.getAt(iIndex) - 'a'));
    }
    
    for (int i = 0; (i < (iCount / 2)); i++)
    {
		iIndex = (iCount - i - 1);
        by = ba.getAt(i);
        ba.setAt(i, strComplement.getAt(ba.getAt(iIndex) - 'a'));
        ba.setAt(iIndex, strComplement.getAt(by - 'a'));
    }

	return ba;
}  
  
double AffxByteArray::parseDouble() const
{
  char *endPtr = NULL;
  double d = strtod(toString().c_str(), &endPtr);
  if ((equals("1.#IND")) || (equals("-1.#IND")) || (equals("1.#QNAN")) || (equals("nan"))) {d = numeric_limits<double>::quiet_NaN();}
  if ((equals("1.#INF")) || (equals("Inf")) || (equals("inf"))) {d = numeric_limits<double>::infinity();}
  if ((equals("-1.#INF")) || (equals("-Inf")) || (equals("-inf"))) {d = -numeric_limits<double>::infinity();}
  return d;
}

AffxByteArray& AffxByteArray::append(char bytes[], int offset, int len) {
//        try
//        {
		int oldcount = m_nSize;
        int newcount = m_nSize + len;
	if (newcount > m_nMaxSize)
	{
	    setSize(newcount);
	}
	
		memcpy(((char*)m_pData + oldcount), ((char *)bytes + offset), len);
//	System.arraycopy(bytes, offset, value, count, len);
	m_nSize = newcount;

//	    }
//	    catch (Throwable e) {AffxLog.throwable(e);}
	return *this;
    }


bool AffxByteArray::nextLine(AffxByteArray& ba)
{
	bool bSuccessful = false;
	int iLength = 0;
	ba.setSize(255);
	int iStart = m_iOffset;
	int iIndex = m_iOffset;

	if (getSize() > 0)
	{
	    int i = 0;
	    int iLast = 0;

	    while ((iIndex < getSize()) && ((i = (int)m_pData[iIndex]) != '\n') && ((i = (int)m_pData[iIndex]) != '\r'))
	    {
	        iIndex++;
	        iLast = i;
	    }
		if ((i == '\r') && ((iIndex + 1) < getSize()) && ((int)m_pData[iIndex + 1] == '\n'))
		{
			iIndex++;
			iLast = i;
			i = '\n';
		}
	    iLength = iIndex - iStart;
	    m_iOffset += (iLength + 1);
	    bool bRemoveLastByte = ((i == '\n') && (iLast == '\r'));
	    bSuccessful = getLine(iStart, iIndex, iLength, ba, bRemoveLastByte);
   	    ba.m_iOffset = 0;
   	}

    return bSuccessful;
}

bool AffxByteArray::isValid(bool bParameterFile)
{
    bool bValid = false;
	trim();
	if (getLength() > 0)
	{
	    if (!bParameterFile) {bValid = true;}
	    else if (getAt(0) != ';') {bValid = true;} // ';'
	}

	return bValid;
}

bool AffxByteArray::getLine(int iStart, int iIndex, int iLength, AffxByteArray& ba, bool bRemoveLastByte)
{
	bool bSuccessful = false;

	if ((iIndex >= getSize()) && (iLength == 0))
	{
	    ba.setSize(0);
	}
	else
	{
	    bSuccessful = true;

	    if (bRemoveLastByte)
	    {
	        if (iLength > 0)
	        {
	            iLength--;
	        }
	    }

	    ba.setSize(iLength);
		memcpy((void*)(ba.m_pData), (void*)(m_pData + iStart), iLength);
	    //System.arraycopy(value, iStart, ba.value, 0, iLength);
	}

	return bSuccessful;
}

AffxString AffxByteArray::getWord(int iWordIndex)
{
    int iIndex = 0;
    int iOffset = 0;
    int iWordFound = 0;
    AffxByteArray ba;
    
    iIndex = iOffset;
    while ((iIndex < length()) && (iWordFound < iWordIndex))
    {
        while ((iIndex < length()) && (getAt(iIndex) <= 32))
            iIndex++;
        
        ba.setSize(0);
        if (iIndex < length())
            iWordFound++;
            
        while ((iIndex < length()) && (getAt(iIndex) > 32))
        {
            ba.append(getAt(iIndex)); 
            iIndex++;
        }
    }
    
    if (iWordFound < iWordIndex)
        ba.setSize(0);
        
    return ba.toString();
}

int AffxByteArray::append(char by)
{
	int nOldSize = m_nSize;
	setSize(m_nSize + 1);

	memcpy(m_pData + nOldSize, &by, 1 * sizeof(char));

	return nOldSize;
}

int AffxByteArray::parameterCount()
{
    int iIndex = 0;
    int iOffset = 0;
    int iWordFound = 0;
	bool bQuotedWord = false;

    iIndex = iOffset;
    while ((iIndex < getLength()))
    {
        while ((iIndex < getLength()) && (getAt(iIndex) <= ' '))
        {
            iIndex++;
        }

		if ((iIndex < getLength()) && (!bQuotedWord))
		{
			if (getAt(iIndex) == '"') {bQuotedWord = true; iIndex++;}
			if (iIndex < getLength())
			{
				iWordFound++;
			}
		}

        while ((iIndex < getLength()) && (getAt(iIndex) > ' '))
        {
            iIndex++;
        }
		if (((iIndex - 1) < getLength()) && (getAt(iIndex - 1) == '"')) {bQuotedWord = false;}
    }

    return iWordFound;
}

int AffxByteArray::csvParameterCount()
{
    int iIndex = 0;
    int iOffset = 0;
    int iWordFound = 0;
	bool bQuotedWord = false;

    iIndex = iOffset;
    while ((iIndex < getLength()))
    {
        while ((iIndex < getLength()) && (getAt(iIndex) == ','))
        {
            iIndex++;
        }

		if ((iIndex < getLength()) && (!bQuotedWord))
		{
			if (getAt(iIndex) == '"') {bQuotedWord = true; iIndex++;}
			if (iIndex < getLength())
			{
				iWordFound++;
			}
		}

        while ((iIndex < getLength()) && (getAt(iIndex) != ','))
        {
            iIndex++;
        }
		if (((iIndex - 1) < getLength()) && (getAt(iIndex - 1) == '"')) {bQuotedWord = false;}
    }

    return iWordFound;
}

AffxByteArray AffxByteArray::getParameter(int iWordIndex)
{
    int iIndex = 0;
    int iOffset = 0;
    int iWordFound = 0;
	bool bQuotedWord = false;
	AffxByteArray ba;
    ba.setSize(0);

    iIndex = iOffset;
    while ((iIndex < getLength()) && (iWordFound < iWordIndex))
    {
        while ((iIndex < getLength()) && (m_pData[iIndex] <= 32))
            iIndex++;

		if ((iIndex < getLength()) && (!bQuotedWord))
		{
			if (getAt(iIndex) == '"') {bQuotedWord = true; iIndex++;}
			ba.setSize(0);
			if (iIndex < getLength()) {iWordFound++;}
		}
		if (!bQuotedWord)
		{
			while ((iIndex < getLength()) && (m_pData[iIndex] > 32))
			{
				ba.append(m_pData[iIndex]);
				iIndex++;
			}
		}
		else
		{
			while (iIndex < getLength())
			{
				if (((iIndex + 1) < getLength()) && (getAt(iIndex) == '"') && (getAt(iIndex + 1) <= 32)) {bQuotedWord = false; iIndex++; break;}
				if (((iIndex + 1) == getLength()) && (getAt(iIndex) == '"')) {bQuotedWord = false; iIndex++; break;}
				ba.append(m_pData[iIndex]);
				iIndex++;
			}
		}
    }

    if (iWordFound < iWordIndex)
        ba.setSize(0);

    return ba;
}

AffxByteArray AffxByteArray::getCsvParameter(int iWordIndex)
{
    int iIndex = 0;
    int iOffset = 0;
    int iWordFound = 0;
	bool bQuotedWord = false;
	AffxByteArray ba;
    ba.setSize(0);

    iIndex = iOffset;
    while ((iIndex < getLength()) && (iWordFound < iWordIndex))
    {
        while ((iIndex < getLength()) && (m_pData[iIndex] == ','))
            iIndex++;

		if ((iIndex < getLength()) && (!bQuotedWord))
		{
			if (getAt(iIndex) == '"') {bQuotedWord = true; iIndex++;}
			ba.setSize(0);
			if (iIndex < getLength()) {iWordFound++;}
		}
		if (!bQuotedWord)
		{
			while ((iIndex < getLength()) && (m_pData[iIndex] != ','))
			{
				ba.append(m_pData[iIndex]);
				iIndex++;
			}
		}
		else
		{
			while (iIndex < getLength())
			{
				if (((iIndex + 1) < getLength()) && (getAt(iIndex) == '"') && (getAt(iIndex + 1) == ',')) {bQuotedWord = false; iIndex++; break;}
				if (((iIndex + 1) == getLength()) && (getAt(iIndex) == '"')) {bQuotedWord = false; iIndex++; break;}
				ba.append(m_pData[iIndex]);
				iIndex++;
			}
		}
    }

    if (iWordFound < iWordIndex)
        ba.setSize(0);

    return ba;
}

int AffxByteArray::getWordCount()
{
    int iIndex = 0;
    int iOffset = 0;
    int iWordFound = 0;

    iIndex = iOffset;
    while ((iIndex < getLength()))
    {
        while ((iIndex < getLength()) && (getAt(iIndex) <= ' '))
        {
            iIndex++;
        }

        if (iIndex < getLength())
        {
            iWordFound++;
        }

        while ((iIndex < getLength()) && (getAt(iIndex) > ' '))
        {
            iIndex++;
        }
    }

    return iWordFound;
}

AffxByteArray AffxByteArray::getWord(int iWordIndex, AffxByteArray& ba)
{
    int iIndex = 0;
    int iOffset = 0;
    int iWordFound = 0;
    ba.setSize(0);

    iIndex = iOffset;
    while ((iIndex < getLength()) && (iWordFound < iWordIndex))
    {
        while ((iIndex < getLength()) && (m_pData[iIndex] <= 32))
            iIndex++;

        ba.setSize(0);
        if (iIndex < getLength())
            iWordFound++;

        while ((iIndex < getLength()) && (m_pData[iIndex] > 32))
        {
            ba.append(m_pData[iIndex]);
            iIndex++;
        }
    }

    if (iWordFound < iWordIndex)
        ba.setSize(0);

    return ba;
}

AffxByteArray AffxByteArray::nextColumn(AffxByteArray& ba)
{
	bool bSuccessful = false;
	int iLength = 0;
	ba.setSize(255);
	int iStart = m_iOffset;
	int iIndex = m_iOffset;

	if (getSize() > 0)
	{
	    int i = 0;
	    while ((iIndex < getSize()) && ((i = (int)m_pData[iIndex]) != 9))
	    {
	        iIndex++;
	    }
	    iLength = iIndex - iStart;
	    m_iOffset += (iLength + 1);
	    bSuccessful = nextColumn(iStart, iIndex, iLength, ba);
   	}

   	ba.m_iFieldOffset = 0;
    return ba;
}

bool AffxByteArray::nextColumn(int iStart, int iIndex, int iLength, AffxByteArray& ba)
{
	bool bSuccessful = false;

	if ((iIndex >= getSize()) && (iLength == 0))
	{
	    ba.setSize(0);
	}
	else
	{
	    bSuccessful = true;
	    ba.setSize(iLength);
		memcpy((void*)(ba.m_pData), (void*)(m_pData + iStart), iLength);
	}

	return bSuccessful;
}

AffxByteArray AffxByteArray::nextField(AffxByteArray& ba)
{
	bool bSuccessful = false;
	int iLength = 0;
	ba.setSize(255);
	int iStart = m_iFieldOffset;
	int iIndex = m_iFieldOffset;

	if (getSize() > 0)
	{
	    int i = 0;
	    while ((iIndex < getSize()) && ((i = (int)m_pData[iIndex]) != ','))
	    {
	        iIndex++;
	    }
	    iLength = iIndex - iStart;
	    m_iFieldOffset += (iLength + 1);
	    bSuccessful = nextField(iStart, iIndex, iLength, ba);
   	}

    return ba;
}

bool AffxByteArray::nextField(int iStart, int iIndex, int iLength, AffxByteArray& ba)
{
	bool bSuccessful = false;

	if ((iIndex >= getSize()) && (iLength == 0))
	{
	    ba.setSize(0);
	}
	else
	{
	    bSuccessful = true;
	    ba.setSize(iLength);
		memcpy((void*)(ba.m_pData), (void*)(m_pData + iStart), iLength);
	}

	return bSuccessful;
}

// This function is used by AffxArray for sorting and searching.
int AffxByteArray::compareTo(const AffxByteArray &obj, int iCompareCode) const
{
	int iCompareResult = 0;
	int iThis = 0;
	int iThat = 0;
	switch(iCompareCode)
	{
	case 0:
	    iCompareResult = compareTo(obj);
	    break;
	case 1: 
		iThis = parseInt();
		iThat = obj.parseInt();
		if (iThis > iThat)
			iCompareResult = 1;
		else if (iThis < iThat)
			iCompareResult = -1;
		else
			iCompareResult = 0;

		break;
	}

	return iCompareResult;
}

int AffxByteArray::compareTo(const AffxByteArray & that) const
{
    int iResult = 0;
	int iLength = __minimum(this->m_nSize, that.m_nSize);

	bool bEqual = true;
	for (int iIndex = 0; (iIndex < iLength); iIndex++)
	{
	    if (this->m_pData[iIndex] != that.m_pData[iIndex])
	    {
	        bEqual = false;
	        iResult = (this->m_pData[iIndex] - that.m_pData[iIndex]);
	        break;
	    }
	}
	if (bEqual) {iResult = (this->m_nSize - that.m_nSize);}

	return iResult;
}

int AffxByteArray::compareTo(const AffxString & that) const
{
    int iResult = 0;
	int iLength = __minimum(this->m_nSize, (int)that.length());

	bool bEqual = true;
	for (int iIndex = 0; (iIndex < iLength); iIndex++)
	{
	    if (this->m_pData[iIndex] != that.charAt(iIndex))
	    {
	        bEqual = false;
	        iResult = (this->m_pData[iIndex] - that.charAt(iIndex));
	        break;
	    }
	}
	if (bEqual) {iResult = (this->m_nSize - (int)that.length());}

	return iResult;
}

void AffxByteArray::replace(char c1, char c2)
{
	for (int iIndex = 0; (iIndex < getLength()); iIndex++)
	{
		if (getAt(iIndex) == c1) {setAt(iIndex, c2);}
	}
}

void AffxByteArray::replace(const AffxString& str1, const AffxString& str2)
{
	AffxString str1Temp = str1;
	AffxByteArray ba;
	int iIndex = indexOf(str1Temp);
	while (iIndex != -1)
	{
		ba.setSize(0);
		ba.append(substring(0, iIndex));
		ba.append(str2);
		ba.append(substring(iIndex + (int)str1Temp.length()));
		assign(ba.toString());
		iIndex = indexOf(str1Temp);
	}
}

bool AffxByteArray::isAllWhitespace()
{
    bool bAllWhitespace = true;
    for (int i = 0; (i < getLength()); i++)
    {
        if (getAt(i) > ' ') {bAllWhitespace = false; break;}
    }
    return bAllWhitespace;
}

int AffxByteArray::getMaxRun(char byBase)
{
	int iCurrentRun = 0;
	int iMaxRun = 0;
	for (int iBaseIndex = 0; (iBaseIndex < getLength()); iBaseIndex++)
	{
		if (getAt(iBaseIndex) == byBase) {iCurrentRun++;}
		else
		{
			if (iCurrentRun > iMaxRun) {iMaxRun = iCurrentRun;}
			if (getAt(iBaseIndex) == byBase) {iCurrentRun = 1;}
			else {iCurrentRun = 0;}
		}
	}
	if (iCurrentRun > iMaxRun) {iMaxRun = iCurrentRun;}
	return (iMaxRun);
}

int AffxByteArray::getCountInWindow(char byBase, int iWindowSize)
{
    int iBestCount = 0;
    int iCount = 0;
	iWindowSize = __minimum(iWindowSize, getLength());
    for (int iBaseIndex = 0; (iBaseIndex <= (getLength() - iWindowSize)); iBaseIndex++)
    {
        iCount = 0;
        for (int iWindowOffset = 0; (iWindowOffset < iWindowSize); iWindowOffset++)
        {
			if (getAt(iBaseIndex + iWindowOffset) == byBase) {iCount++;}
        }
        if (iCount > iBestCount) {iBestCount = iCount;}
    }
    return (iBestCount);
}

int AffxByteArray::getCount(char byBase)
{
	int iCount = 0;
	for (int iBaseIndex = 0; (iBaseIndex < getLength()); iBaseIndex++)
	{
		if (getAt(iBaseIndex) == byBase) {iCount++;}
	}
	return (iCount);
}

// Read a the entire file into a byte array.
bool AffxByteArray::readFile(const AffxString& strFileName)
{
  std::ifstream file;
  bool bSuccessful = false;
  unsigned int uiNumberBytes = Fs::fileSize(strFileName);

  Fs::aptOpen(file, strFileName);
  setSize(uiNumberBytes);
  if (uiNumberBytes > 0)   {
    memset(getData(), 0, uiNumberBytes);
    file.read(getData(), uiNumberBytes);
    bSuccessful = true;
  }
  file.close();
  
  setOffset(0);

  return bSuccessful;
}
