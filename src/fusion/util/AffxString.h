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

#ifndef _AffxString_h_
#define _AffxString_h_
/**
 * @file AffxString.h
 *
 * @brief This header contains the AffxString class definition. 
 */

#include "util/AffxConv.h"
#include "util/Err.h"
//
#include <cctype> 
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdarg.h>
#include <string>
//

#define MAX_SPRINTF_SIZE 8192
#define _TAB_CHAR        0x09
   
/**
 * @brief  A class derived from std::string.
 * 
 */  
class AffxString : public std::string
{
    /*
private:
    static AffxStringHashtable m_ht;
public:
    static AffxStringHashtable* getHashtable() {return &m_ht;}
    static int getHashtableCount() {return (int)m_ht.GetCount();}
*/
public:
    AffxString ();
    AffxString (const std::string& stringSrc);
    AffxString (const AffxString& stringSrc);
    AffxString (char ch, int nRepeat = 1);
    //AffxString (unsigned char ch, int nRepeat = 1);
    AffxString (const char* psz);
    AffxString (const char* pch, int nLength);

    // Character Query Member Functions
    //
    bool isTab          (int nIndex) const;
    bool isWhiteSpace   (int nIndex) const;
    bool isAlphabetic   (int nIndex) const;
    bool isAlphanumeric (int nIndex) const;
    bool isBlank        (int nIndex) const;
    bool isPunctuation  (int nIndex) const;
    bool isUppercase    (int nIndex) const;
    bool isLowercase    (int nIndex) const;
    bool isNumeric      (int nIndex) const;
    bool isHexNumeric   (int nIndex) const;

    AffxString getKey();
    AffxString getValue();
    AffxString removeCruft();

    // Misc Functions
    //
    void toUpper (int nIndex);
    void toLower (int nIndex);

    // Tab Expansion
    //
    void expandTabs (int nTabSize = 8);


    unsigned int getLength() const {return (unsigned int)length();}
    AffxString mid(int iStartIndex, int iLength) const {return substr(iStartIndex, iStartIndex + iLength);}
    // Stripping Functions
    //
    void stripTrailingBlanks  ();
    void stripPrecedingBlanks ();

    void stripTrailingCharacters  (char cChar);
    void stripPrecedingCharacters (char cChar);       

    void padBlanks (short nMax);
    bool isBlank ();
    bool isBlankOrEmpty ();
    bool isNumber();
      
    int printf(const char *pszFormat, ...);
    int sprintf(const char *pszFormat, ...);  
           
    void stripAndPad(int iLength); 
    void strip(void);
    AffxString& trim() {strip(); return *this;}
    void zeroFill(int i, int iLength);

    static AffxString intToString(int i, bool bCommas);
                  
    static AffxString doubleToString(double d, int iDecimalPlaces, bool bCommas);

    void setAt(unsigned int iIndex, char c) {if (iIndex < length()) {(*this)[iIndex] = c;} else {Err::errAbort("Out of Bounds exception.");}}
    char getAt(unsigned int iIndex) const {if (iIndex < length()) {return (*this)[iIndex];} else {Err::errAbort("Out of Bounds exception."); return 0;}}
    char charAt(unsigned int iIndex) const {if (iIndex < length()) {return (*this)[iIndex];} else {Err::errAbort("Out of Bounds exception."); return 0;}}
    AffxString& toLowerCase() 
    {
        for (unsigned int iIndex = 0; (iIndex < length()); iIndex++)
        {
            toLower(iIndex);
        }
        return *this;
    }
    AffxString& toUpperCase() 
    {
        for (unsigned int iIndex = 0; (iIndex < length()); iIndex++)
        {
            toUpper(iIndex);
        }
        return *this;
    }
    AffxString substring(unsigned int iIndex) const {if (iIndex < length()) {return substr(iIndex);} else {return "";}}
    AffxString& replace(char cOld, char cNew);
    AffxString substring(unsigned int iIndex, unsigned int iEndIndex) const {if (iEndIndex > length()) {iEndIndex = (unsigned int)length();} if (iIndex < length()) {return substr(iIndex, (iEndIndex - iIndex));} else {return "";}}
    bool equals(const AffxString& str) const {return operator==(((const AffxString&)*this), (const AffxString&)str);}
    //    bool operator!=(const AffxString& str) {return !equals(str);}
    AffxString reverseComplement();
    AffxString reverse();
    AffxString complement();
    void convertAmbiguities(char chAmbiguity);
    bool startsWith(const AffxString& strCompare) const;
    int compareTo(const AffxString& obj, int iCompareCode) const;
    bool endsWith(const AffxString& str) const;
    int indexOf(const AffxString& strSubstring) const {return (int)find(strSubstring);}
    int nextIndexOf(const AffxString& strSubstring, int iIndex) const;
    int lastIndexOf(char ch) const {return (int)rfind(ch);}
    AffxString& operator+(char ch) {*this += ch; return *this;}
    AffxString& operator+(int i) 
    {
        *this += ::getInt(i);
        return *this;
    }
    AffxString removeSurroundingQuotes();
    bool isBaseAt(int iIndex);
    bool isSnpAt(int iIndex);
    bool isIUPACCodeAt(int iIndex) {return ((isBaseAt(iIndex)) || (isSnpAt(iIndex)));}
    AffxString getSnpStringAt(int iIndex);
    double calculateMeltingTemp();
    double calculateMeltingTempWithSNPs();
    bool isBiAllelic();
    bool isTriAllelic(); 
    AffxString getIUPACCode();
    AffxString getIUPACCodeRC();
    double getGCContent();

    unsigned int hash() const
    {
        unsigned int nHash = 0;
        for (unsigned int iIndex = 0; (iIndex < length()); iIndex++)
        {
            nHash = (nHash<<5) + nHash + getAt(iIndex);
        }
        return nHash;
    }

    int compareNoCase(const AffxString& str) const {return CompareNoCase(str);}
    int CompareNoCase(const AffxString& str) const
    {
        AffxString strThis = *this;
        AffxString strThat = str;
        strThis.toLowerCase();
        strThat.toLowerCase();
        return strThis.compareTo(strThat, 0);
    }

    template<int k> struct ComparePred {
        bool operator()(const AffxString* lhs, const AffxString* rhs) const {
            Err::errAbort("AffxString: ComparePred instantiated with an invalid compare code = " + ToStr(k));
            return false;
        }
    };
};

template<> struct AffxString::ComparePred<0> {
    bool operator()(const AffxString* lhs, const AffxString* rhs) const {
        return *lhs < *rhs;
    }


};


inline AffxString::AffxString() : std::string() {}
inline AffxString::AffxString(const std::string& stringSrc) : std::string(stringSrc) {}
inline AffxString::AffxString(const AffxString& stringSrc) : std::string(stringSrc) {}
inline AffxString::AffxString(char ch, int nRepeat) : std::string(nRepeat, ch) {}
inline AffxString::AffxString(const char* psz) : std::string(psz) {}
inline AffxString::AffxString(const char* pch, int nLength) : std::string(pch, nLength) {}

inline bool AffxString::isTab (int nIndex) const // Non Zero if ==, 0 if !=
{
   return getAt(nIndex) == _TAB_CHAR;
}

inline bool AffxString::isAlphabetic (int nIndex) const 
{
   return isalpha(getAt(nIndex)) != 0;
}

inline bool AffxString::isAlphanumeric (int nIndex) const
{
   return isalnum(getAt(nIndex)) != 0;
}

inline bool AffxString::isWhiteSpace (int nIndex) const 
{
   return isspace(getAt(nIndex)) != 0;
}

inline bool AffxString::isBlank (int nIndex) const 
{
   return getAt(nIndex) == ' ';
}

inline bool AffxString::isPunctuation (int nIndex) const 
{
   return ispunct(getAt(nIndex)) != 0;
}

inline bool AffxString::isUppercase (int nIndex) const 
{
   return isupper(getAt(nIndex)) != 0;
}

inline bool AffxString::isLowercase (int nIndex) const 
{
   return islower(getAt(nIndex)) != 0;
}

inline bool AffxString::isNumeric (int nIndex) const 
{
   return isdigit(getAt(nIndex)) != 0;
}

inline bool AffxString::isHexNumeric (int nIndex) const
{
   return isxdigit(getAt(nIndex)) != 0;
}



//===== Misc Functions =====
//
inline void AffxString::toUpper (int nIndex)
{
   setAt(nIndex, (char)toupper(getAt(nIndex)));
}

inline void AffxString::toLower (int nIndex)
{
   setAt(nIndex, (char)tolower(getAt(nIndex)));
}



//===== Stripping Functions =====
//
inline void AffxString::stripPrecedingBlanks ()
   { stripPrecedingCharacters (' '); }


inline void AffxString::stripTrailingBlanks ()
   { stripTrailingCharacters (' '); }

                    
// string can't contain blank spaces                    
inline bool AffxString::isBlankOrEmpty ()
{
    if (length() == 0 || (length() > 0 && isBlank()))
        return true;
    else
        return false;
}

inline bool AffxString::isNumber()
{
    // empty string
    if (length() == 0) { return false; }

    bool bIsNumber = true;
    for (int i = 0; i < (int)length(); i++)
    {
        // must be a digit
        // must be a period and it must be in the middle
        // must be a minus sign and it must be at the beginning
        if (!isdigit(getAt(i)) && 
            !(getAt(i) == '.' && 0 < i && i < (int)length() - 1) &&
            !(getAt(i) == '-' && i == 0) &&
            !(getAt(i) == '+' && i == 0))
        {
            bIsNumber = false;
            break;
        }
    }
    return bIsNumber;
}

#endif
