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
 * @file AffxString.cpp
 *
 * @brief This file contains the AffxString class members. 
 */

//
#include "util/AffxString.h" 
//
#include "util/AffxArray.h"
#include "util/AffxConv.h"
//
#include <cstring>
#include <string>
#include <stdio.h>
//

//AffxStringHashtable AffxString::m_ht(29);

void AffxString::stripPrecedingCharacters (char cChar)
{
    for (unsigned int iIndex = 0; (iIndex < length()); iIndex++)
    {
        if (charAt(iIndex) == cChar)
        {
            *this = substr(1);
            iIndex--;            
        }
        else {break;}
    }
}

void AffxString::stripTrailingCharacters (char cChar)
{
    for (int iIndex = (int)length() - 1; (iIndex >= 0); iIndex--)
    {
        if (charAt(iIndex) == cChar)
        {
            *this = substr(0, length() - 1);
        }
        else {break;}
    }
}

void AffxString::padBlanks (short nMax)
{    
    int nLen = (int)length();
    for (; nLen < nMax; nLen++)
        *this += ' ';
}

bool AffxString::isBlank ()
{     
    for (unsigned int i(0); i < length(); i++)
        if ((*this)[i] != ' ')
            return false;
    return true;
}

int AffxString::printf(const char *pszFormat, ...)
{     
    int iResult = 0;
    char szBuffer[MAX_SPRINTF_SIZE]; 
    
    va_list args;
    va_start(args, pszFormat);
#ifdef WIN32
    iResult = vsprintf_s(szBuffer, MAX_SPRINTF_SIZE, pszFormat, args);
#else
    iResult = vsprintf(szBuffer, pszFormat, args);
#endif
    *this += szBuffer;
    va_end(args);
    
    return iResult;
} 
 
int AffxString::sprintf(const char *pszFormat, ...)
{    
    int iResult = 0;
    char szBuffer[MAX_SPRINTF_SIZE]; 
    
    va_list args;
    va_start(args, pszFormat);
#ifdef WIN32
    iResult = vsprintf_s(szBuffer, MAX_SPRINTF_SIZE, pszFormat, args);
#else
    iResult = vsprintf(szBuffer, pszFormat, args);
#endif
    *this = szBuffer;
    va_end(args);
    
    return iResult;
}

void AffxString::stripAndPad(int iLength)
{   
    stripPrecedingBlanks();
    stripTrailingBlanks();
    padBlanks((short)iLength); 
}                                               

void AffxString::strip()
{   
    stripPrecedingBlanks();
    stripTrailingBlanks();
}

bool AffxString::startsWith(const AffxString& strCompare) const
{
    unsigned int iCompareLength = (unsigned int) strCompare.length();

    if (length() >= iCompareLength)
    {
        return (strncmp(this->c_str(), strCompare.c_str(), iCompareLength) == 0);
    }

    return false;
}
 
AffxString AffxString::reverseComplement()
{
    AffxString str = *this;
    AffxString strComplement = "tvghefcdijmlknopqysaabwxrz";
    char ch = 0;
    int iCount = (int)length();
    int iIndex = 0;
    
    str.toLowerCase();
    if (((iCount / 2) * 2) != iCount)
    {
        iIndex = (iCount / 2);
        if (str.getAt(iIndex) < 'a') {str.setAt(iIndex, 'n');}
        try {str.setAt(iIndex, strComplement.getAt(str.getAt(iIndex) - 'a'));} catch(...) {str.setAt(iIndex, 'n');}
    }
    
    for (int i = 0; (i < (iCount / 2)); i++)
    {
        iIndex = (iCount - i - 1);
        ch = str.getAt(i);
        if (ch < 'a') {ch = 'n';}
        if (str.getAt(iIndex) < 'a') {str.setAt(iIndex, 'n');}
        try {str.setAt(i, strComplement.getAt(str.getAt(iIndex) - 'a'));} catch(...) {str.setAt(i, 'n');}
        try {str.setAt(iIndex, strComplement.getAt(ch - 'a'));} catch(...) {str.setAt(iIndex, 'n');}
    }

    return str;
} 

AffxString AffxString::complement()
{
    AffxString str = *this;
    AffxString strComplement = "tvghefcdijmlknopqywaabsxrz";
    int iCount = (int)length();
    
    str.toLowerCase();

    for (int i = 0; (i < iCount); i++)
    {
        try {str.setAt(i, strComplement.getAt(str.getAt(i) - 'a'));} catch(...) {str.setAt(i, 'n');}
    }

    return str;
} 

AffxString AffxString::reverse()
{
    AffxString str = *this;
    char ch = 0;
    int iCount = (int)length();
    int iIndex = 0;
      
    for (int i = 0; (i < (iCount / 2)); i++)
    {
        iIndex = (iCount - i - 1);
        ch = getAt(i);
        str.setAt(i, str.getAt(iIndex));
        str.setAt(iIndex, ch);
    }

    return str;
} 

void AffxString::convertAmbiguities(char chAmbiguity)
{ 
    int iSeqCount = (int)length();
    for (int iSeqIndex = 0; (iSeqIndex < iSeqCount); iSeqIndex++)
    {
        char ch = getAt(iSeqIndex);
        if ((ch != 'a') && (ch != 'c') && (ch != 'g') && (ch != 't'))
        {
            setAt(iSeqIndex, chAmbiguity);
        }
    }
}

//    Take the int parameters and convert it to a charcter string with
//    commas. (Ex. -99999 converts to "-99,999")  
AffxString AffxString::intToString(int i, bool bCommas)
{    
    return ::getInt(i, bCommas);
}         

//    Take the double parameter and convert it to a charcter string with
//    commas. (Ex. -99999.99 converts to "-99,999.99")  
AffxString AffxString::doubleToString(double d, int iDecimalPlaces, bool bCommas)
{   
    return ::getDouble(d, iDecimalPlaces, bCommas);
}  


// Looks for the first occurance of = and returns whatever is before it.  Returns the input if = not found.
AffxString AffxString::getKey()
{
        size_t iIndex = this->find_first_of("=");
        if( iIndex !=npos)
        {
                return this->substr(0, iIndex);
        }
        else
        {
                return *this;
        }
} 


AffxString AffxString::removeCruft()
{
        if(this->startsWith("#%affymetrix-"))
        {
               return this->substr(13, this->length());
        }
        else
        {
                bool value = this->startsWith("#%");
                if(value)
                {
                        return this->substr(2, this->length());
                }
                else
                {
                        return *this;
                }
        }
}



// Looks for the first occurance of = and returns whatever is after it.  Returns the empty string if = not found.
AffxString AffxString::getValue()
{
        size_t iIndex = this->find_first_of("=");
        if( iIndex !=npos)
        {
                return this->substr(iIndex+1, this->length());
        }
        else
        {
                return *this;
        }
} 


// This function is used by AffxArray for sorting and searching.
int AffxString::compareTo(const AffxString& obj, int iCompareCode) const 
{
    int iCompareResult = 0;

    switch(iCompareCode)
    {
    case 0: // Identifier
        if (*this > obj)
            iCompareResult = 1;
        else if (*this < obj)
            iCompareResult = -1;
        else
            iCompareResult = 0;

        break;
    }
    return iCompareResult;
}

AffxString& AffxString::replace(char cOld, char cNew)
{
    int iFindIndex = 0;

    while ((iFindIndex = (int)find(cOld)) != -1)
        setAt(iFindIndex, cNew);

    return *this;
}

bool AffxString::endsWith(const AffxString& str) const
{
    if (getLength() < str.getLength()) {return false;}
    return (strcmp(c_str() + (getLength() - str.getLength()), str.c_str()) == 0);
/*
    bool bEndsWith = false;
    int iCompareLength = strCompare.length();

    if (length() >= iCompareLength)
    {
        int iIndex2 = 0;
        bEndsWith = true;
        for (int iIndex = (length() - iCompareLength); (iIndex < length()); iIndex++)
        {
            if (GetAt(iIndex) != strCompare.GetAt(iIndex2))
            {
                bEndsWith = false;
                break;
            }
            iIndex2++;
        }
    }

    return bEndsWith;
    */
}

AffxString AffxString::removeSurroundingQuotes()
{
    if ((startsWith("\"")) && (endsWith("\"")))
    {
        *this = substring(1, (int)length() - 1);
    }
    return *this;
}

int AffxString::nextIndexOf(const AffxString& str, int iIndex) const
{
    for (unsigned int i = (iIndex + 1); (i <= (getLength() - str.getLength())); i++)
    {
        if (substring(i, (i + (int)str.getLength())) == str)
        {
            return i;
        }
    }
    return -1;
}

bool AffxString::isBaseAt(int iIndex)
  {
      switch(charAt(iIndex))
      {
      case 'a': return true; 
      case 'A': return true; 
      case 'c': return true; 
      case 'C': return true; 
      case 'g': return true; 
      case 'G': return true;
      case 't': return true; 
      case 'T': return true;  
      default: return false;
      }      
  }

bool AffxString::isSnpAt(int iIndex)
  {
      switch(charAt(iIndex))
      {
      case 'r': return true; 
      case 'R': return true; 
      case 'y': return true; 
      case 'Y': return true; 
      case 'm': return true; 
      case 'M': return true;
      case 'k': return true; 
      case 'K': return true; 
      case 'w': return true;
      case 'W': return true; 
      case 's': return true; 
      case 'S': return true;
      case 'b': return true;
      case 'B': return true; 
      case 'd': return true; 
      case 'D': return true; 
      case 'h': return true;
      case 'H': return true;
      case 'v': return true;
      case 'V': return true; 
      case 'n': return true; 
      case 'N': return true; 
      default: return false;
      }      
  }

AffxString AffxString::getIUPACCode()
{
    if (this->indexOf("-") != -1) {return "-";}
    if (this->CompareNoCase("R") == 0) {return "R";}
    if ((this->CompareNoCase("A/G") == 0) || (this->CompareNoCase("G/A") == 0)) {return "R";}
    if (this->CompareNoCase("Y") == 0) {return "Y";}
    if ((this->CompareNoCase("C/T") == 0) || (this->CompareNoCase("T/C") == 0)) {return "Y";}
    if (this->CompareNoCase("M") == 0) {return "M";}
    if ((this->CompareNoCase("A/C") == 0) || (this->CompareNoCase("C/A") == 0)) {return "M";}
    if (this->CompareNoCase("K") == 0) {return "K";}
    if ((this->CompareNoCase("G/T") == 0) || (this->CompareNoCase("T/G") == 0)) {return "K";}
    if (this->CompareNoCase("W") == 0) {return "W";}
    if ((this->CompareNoCase("A/T") == 0) || (this->CompareNoCase("T/A") == 0)) {return "W";}
    if (this->CompareNoCase("S") == 0) {return "S";}
    if ((this->CompareNoCase("C/G") == 0) || (this->CompareNoCase("G/C") == 0)) {return "S";}
    if (this->CompareNoCase("B") == 0) {return "B";}
    if ((this->CompareNoCase("C/G/T") == 0) || (this->CompareNoCase("T/G/C") == 0)) {return "B";}
    if (this->CompareNoCase("D") == 0) {return "D";}
    if ((this->CompareNoCase("A/G/T") == 0) || (this->CompareNoCase("T/G/A") == 0)) {return "D";}
    if (this->CompareNoCase("H") == 0) {return "H";}
    if ((this->CompareNoCase("A/C/T") == 0) || (this->CompareNoCase("T/C/A") == 0)) {return "H";}
    if (this->CompareNoCase("V") == 0) {return "V";}
    if ((this->CompareNoCase("A/C/G") == 0) || (this->CompareNoCase("G/C/A") == 0)) {return "V";}
    if (this->CompareNoCase("N") == 0) {return "N";}
    if ((this->CompareNoCase("A/C/G/T") == 0) || (this->CompareNoCase("T/G/C/A") == 0)) {return "N";}
    return "?";
}

AffxString AffxString::getIUPACCodeRC()
  {
      if (*this == "") {return "?";}
      switch(this->charAt(0))
      {
      case 'r': return "Y"; 
      case 'R': return "Y"; 
      case 'y': return "R"; 
      case 'Y': return "R"; 
      case 'm': return "K"; 
      case 'M': return "K";
      case 'k': return "M"; 
      case 'K': return "M"; 
      case 'w': return "W";
      case 'W': return "W"; 
      case 's': return "S"; 
      case 'S': return "S";
      case 'b': return "V";
      case 'B': return "V"; 
      case 'd': return "H"; 
      case 'D': return "H"; 
      case 'h': return "D";
      case 'H': return "D";
      case 'v': return "B";
      case 'V': return "B"; 
      case 'n': return "N"; 
      case 'N': return "N"; 
      case '-': return "-"; 
      case '?': return "?"; 
      default: return "?";
      }      
  }

bool AffxString::isBiAllelic()
{
    if (this->CompareNoCase("R") == 0) {return true;}
    if (this->CompareNoCase("A/G") == 0) {return true;}
    if (this->CompareNoCase("Y") == 0) {return true;}
    if (this->CompareNoCase("C/T") == 0) {return true;}
    if (this->CompareNoCase("M") == 0) {return true;}
    if (this->CompareNoCase("A/C") == 0) {return true;}
    if (this->CompareNoCase("K") == 0) {return true;}
    if (this->CompareNoCase("G/T") == 0) {return true;}
    if (this->CompareNoCase("W") == 0) {return true;}
    if (this->CompareNoCase("A/T") == 0) {return true;}
    if (this->CompareNoCase("S") == 0) {return true;}
    if (this->CompareNoCase("C/G") == 0) {return true;}
    return false;
}

bool AffxString::isTriAllelic()
{
    if (this->CompareNoCase("B") == 0) {return true;}
    if (this->CompareNoCase("C/G/T") == 0) {return true;}
    if (this->CompareNoCase("D") == 0) {return true;}
    if (this->CompareNoCase("A/G/T") == 0) {return true;}
    if (this->CompareNoCase("H") == 0) {return true;}
    if (this->CompareNoCase("A/C/T") == 0) {return true;}
    if (this->CompareNoCase("V") == 0) {return true;}
    if (this->CompareNoCase("A/C/G") == 0) {return true;}
    return false;
}

AffxString AffxString::getSnpStringAt(int iIndex)
  {
      switch(charAt(iIndex))
      {
      case 'a': return "A"; 
      case 'A': return "A"; 
      case 'c': return "C"; 
      case 'C': return "C"; 
      case 'g': return "G"; 
      case 'G': return "G"; 
      case 't': return "T"; 
      case 'T': return "T"; 
      case 'r': return "AG"; 
      case 'R': return "AG"; 
      case 'y': return "CT"; 
      case 'Y': return "CT"; 
      case 'm': return "AC"; 
      case 'M': return "AC";
      case 'k': return "GT"; 
      case 'K': return "GT"; 
      case 'w': return "AT";
      case 'W': return "AT"; 
      case 's': return "CG"; 
      case 'S': return "CG";
      case 'b': return "CGT";
      case 'B': return "CGT"; 
      case 'd': return "AGT"; 
      case 'D': return "AGT"; 
      case 'h': return "ACT";
      case 'H': return "ACT";
      case 'v': return "ACG";
      case 'V': return "ACG"; 
      case 'n': return "ACGT"; 
      case 'N': return "ACGT"; 
      default: return "";
      }       
  }

/** calculates the melting temperature of a DNA sequence according to an algorithmn
* provided by Tom. Does not handle ambigous DNA symbols (e.g. SNP b = c/g/t)
*/
double AffxString::calculateMeltingTemp() 
{    
    double dTemp = 0;
    if (length() < 8) 
    {
        int iACount = 0;
        int iCCount = 0;
        int iGCount = 0;
        int iTCount = 0;
        for (unsigned int iIndex = 0; (iIndex < length()); iIndex++)
        {
            switch(charAt(iIndex))
            {
            case 'a': iACount++; break;
            case 'A': iACount++; break;
            case 'c': iCCount++; break;
            case 'C': iCCount++; break;
            case 'g': iGCount++; break;
            case 'G': iGCount++; break;
            case 't': iTCount++; break;
            case 'T': iTCount++; break;
            default: return 0;
            }
        }
        dTemp = (2 * (iACount + iTCount) + 4 * (iCCount + iGCount));
    }
    else
    {
        double dRlnK = 0;
        double dDeltah = 0;
        double dDeltas = 0;
        double dSaltadj = 0;

        for (unsigned int iIndex = 0; (iIndex < (length() - 1)); iIndex++)
        {
            if ((charAt(iIndex + 1) == 'A') || (charAt(iIndex + 1) == 'a'))
            {
                if ((charAt(iIndex) == 'A') || (charAt(iIndex) == 'a'))
                {
                    dDeltah += 8.0;
                    dDeltas += 21.9;
                }
                else if ((charAt(iIndex) == 'C') || (charAt(iIndex) == 'c'))
                {
                    dDeltah += 8.2;
                    dDeltas += 21.0;
                }
                else if ((charAt(iIndex) == 'G') || (charAt(iIndex) == 'g'))
                {
                    dDeltah += 8.8;
                    dDeltas += 23.5;
                }
                else if ((charAt(iIndex) == 'T') || (charAt(iIndex) == 't'))
                {
                    dDeltah += 5.6;
                    dDeltas += 15.2;
                }
            }
            else if ((charAt(iIndex + 1) == 'C') || (charAt(iIndex + 1) == 'c'))
            {
                if ((charAt(iIndex) == 'A') || (charAt(iIndex) == 'a'))
                {
                    dDeltah += 9.4;
                    dDeltas += 25.5;
                }
                else if ((charAt(iIndex) == 'C') || (charAt(iIndex) == 'c'))
                {
                    dDeltah += 10.9;
                    dDeltas += 28.4;
                }
                else if ((charAt(iIndex) == 'G') || (charAt(iIndex) == 'g'))
                {
                    dDeltah += 10.5;
                    dDeltas += 26.4;
                }
                else if ((charAt(iIndex) == 'T') || (charAt(iIndex) == 't'))
                {
                    dDeltah += 8.8;
                    dDeltas += 23.5;
                }
            }
            else if ((charAt(iIndex + 1) == 'G') || (charAt(iIndex + 1) == 'g'))
            {
                if ((charAt(iIndex) == 'A') || (charAt(iIndex) == 'a'))
                {
                    dDeltah += 6.6;
                    dDeltas += 16.4;
                }
                else if ((charAt(iIndex) == 'C') || (charAt(iIndex) == 'c'))
                {
                    dDeltah += 11.8;
                    dDeltas += 29.0;
                }
                else if ((charAt(iIndex) == 'G') || (charAt(iIndex) == 'g'))
                {
                    dDeltah += 10.9;
                    dDeltas += 28.4;
                }
                else if ((charAt(iIndex) == 'T') || (charAt(iIndex) == 't'))
                {
                    dDeltah += 8.2;
                    dDeltas += 21.0;
                }
            }
            else if ((charAt(iIndex + 1) == 'T') || (charAt(iIndex + 1) == 't'))
            {
                if ((charAt(iIndex) == 'A') || (charAt(iIndex) == 'a'))
                {
                    dDeltah += 5.6;
                    dDeltas += 15.2;
                }
                else if ((charAt(iIndex) == 'C') || (charAt(iIndex) == 'c'))
                {
                    dDeltah += 6.6;
                    dDeltas += 16.4;
                }
                else if ((charAt(iIndex) == 'G') || (charAt(iIndex) == 'g'))
                {
                    dDeltah += 9.4;
                    dDeltas += 25.5;
                }
                else if ((charAt(iIndex) == 'T') || (charAt(iIndex) == 't'))
                {
                    dDeltah += 8.0;
                    dDeltas += 21.9;
                }
                else return 0;
            }
            else return 0;
        }

        dRlnK = 1.987 * log(1.0 / (100 * 1.0e-9));
        dSaltadj = 7.21 * log(100.0 / 1000.0);

        dTemp = (1000.0 * (dDeltah - 3.4)) / (dDeltas + dRlnK) - 272.9 + dSaltadj;
    }
    return dTemp;    
}
  
/** calculates the melting temperature of a DNA sequence. If the Sequence contains SNPs (with IUPac codes like
*  b = c/g/t it calcuates the temp for all possible alleles and returns the MINIMUM temp
*/
double AffxString::calculateMeltingTempWithSNPs() 
{
    if (length() == 0) {return 0;}
    AffxArray<AffxString> arSequences; //List of sequences with ambiguities removed
    int iLastStart = 0;
    int iSnpCount = 0;
    for (unsigned int iIndex = 0; (iIndex < length()); iIndex++)
    {
        if (isSnpAt(iIndex))
        {
            iSnpCount++;
            if (iSnpCount > 5) {return 0;}
            int iStart = iIndex;
            AffxString strAlleles = getSnpStringAt(iStart);
            AffxString strSubstring = substring(iLastStart, iStart);
            iLastStart = iStart + 1;
            if (arSequences.getCount() == 0)
            {
                for (unsigned int iAlleleIndex = 0; (iAlleleIndex < strAlleles.length()); iAlleleIndex++)
                {
                    arSequences.add(new AffxString("" + strSubstring + strAlleles.charAt(iAlleleIndex)));
                }
            }
            else
            {
                // create a new list which multiplies the elements by the number of alleles
                AffxArray<AffxString> arNewSequences;
                for (int iSequenceIndex = 0; (iSequenceIndex < arSequences.getCount()); iSequenceIndex++) 
                {
                    // seed the new list with copies of the string
                    AffxString strBase = *arSequences.getAt(iSequenceIndex);
                    for (unsigned int iAlleleIndex = 0; (iAlleleIndex < strAlleles.length()); iAlleleIndex++)
                    {
                        arNewSequences.add(new AffxString("" + strBase + strSubstring + strAlleles.charAt(iAlleleIndex)));
                    }
                }
                arSequences.deleteAll();
                for (int iSequenceIndex = 0; (iSequenceIndex < arNewSequences.getCount()); iSequenceIndex++)
                {
                    arSequences.add(arNewSequences.getAt(iSequenceIndex));
                }
                arNewSequences.nullAll();
            }
        }
    }

    // add the last sequence section to the list
    AffxString strSubstring = substring(iLastStart, (int)length());
    if (arSequences.getCount() == 0)
    {
        arSequences.add(new AffxString(strSubstring));
    }
    else
    {
        AffxArray<AffxString> arNewSequences;
        for (int iSequenceIndex = 0; (iSequenceIndex < arSequences.getCount()); iSequenceIndex++)
        {
            // seed the new list with copies of the string
            AffxString strBase = *arSequences.getAt(iSequenceIndex);
            arNewSequences.add(new AffxString(strBase + strSubstring));
        }
        arSequences.deleteAll();
        for (int iSequenceIndex = 0; (iSequenceIndex < arNewSequences.getCount()); iSequenceIndex++)
        {
            arSequences.add(arNewSequences.getAt(iSequenceIndex));
        }
        arNewSequences.nullAll();
    }

    double dMinTemp = 1.7976931348623157e+308; // Double.MAX_VALUE
    for (int iSequenceIndex = 0; (iSequenceIndex < arSequences.getCount()); iSequenceIndex++)
    {
        AffxString strSequence = *arSequences.getAt(iSequenceIndex);
        double dTemp = strSequence.calculateMeltingTemp();
        if (dTemp < dMinTemp)
        {
            dMinTemp = dTemp;
        }
    }  
    arSequences.deleteAll();
    return dMinTemp;
 }

double AffxString::getGCContent()
{
    double dGCContent = 0;
    int iGCCount = 0;
    int iBaseCount = 0;
    for (unsigned int iBaseIndex = 0; (iBaseIndex < length()); iBaseIndex++)
    {
        if ((charAt(iBaseIndex) == 'C') || (charAt(iBaseIndex) == 'c') ||
            (charAt(iBaseIndex) == 'G') || (charAt(iBaseIndex) == 'g'))
        {
            iGCCount++;
        }
        iBaseCount++;
    }
    if (iBaseCount == 0) {dGCContent = 0;}
    else {dGCContent = ((double)iGCCount / (double)iBaseCount) * 100;}
    return dGCContent;
}
