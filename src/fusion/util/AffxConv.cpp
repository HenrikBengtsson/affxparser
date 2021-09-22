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
 * @file AffxConv.cpp
 *
 * @brief This file contains some data conversion routines. 
 */

/**
 * @brief  Some data conversion routines. 
 * 
 */

//
#include "util/AffxConv.h" 
//
#include "util/Convert.h"
//
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <stdio.h>
//

#ifdef WIN64
# undef WIN32
#endif


using namespace std;

/** 
 * Converts a string with commas into an int.
 * @param str - The string to convert.
 * @return - int value.
 */
int getInt(const std::string& str)
{                                  
    std::string strInt;                              
	int iLength = (int)str.length();   
	
	for (int i = 0; (i < iLength); i++) 
	{
		if (str[i] != ',') {strInt += str[i];}
	}
		
	return atol(strInt.c_str());		
}  

/** 
 * Converts a string with commas into an unsigned int.
 * @param str - The string to convert.
 * @return - unsigned int value.
 */
unsigned int getUnsignedInt(const std::string& str)
{
	return (unsigned int)getDouble(str);
}         
   
/** 
 * Converts a string with commas into a double.
 * @param str - The string to convert.
 * @return - double value.
 */
double getDouble(const std::string& str)
{
                                  
    std::string strDouble;                              
	int iLength = (int)str.length();   
	
	for (int i = 0; (i < iLength); i++) 
	{
		if (str[i] != ',') {strDouble += str[i];}
	}
	return atof(strDouble.c_str());		
}

/** 
 * Converts an int to a string.
 * @param i - The int to convert.
 * @param iLength - The length of the output string.
 * @return - string value.
 */
std::string getInt(int i, unsigned int iLength)
{
	std::string str = getInt(i);
	while (str.length() < iLength) {str = "0" + str;}
	return str;
}
   
/** 
 * Converts an int to a string.
 * @param i - The int to convert.
 * @param bCommas - true if you want commas in the string, else false (defaults to false).
 * @return - string value.
 */ 
std::string getInt(int i, bool bCommas)
{       
	char szValue[64];
#if defined(WIN32) && !defined(__MINGW32__)
	sprintf_s(szValue, 64, "%d", i);
#else
	sprintf(szValue, "%d", i);
#endif
	if (i != i) {return "nan";}
	if (!bCommas) {return szValue;}
	return formatString(szValue, 0, bCommas);
}     

/** 
 * Converts an int64 to a string.
 * @param l - The int64 to convert.
 * @param bCommas - true if you want commas in the string, else false (defaults to false).
 * @return - string value.
 */ 
/*
std::string getLong64(__int64 l, bool bCommas)
{       
	char szValue[64];
#ifdef WIN32
	_i64toa_s(l, szValue, 64, 10);
#else
	_i64toa(l, szValue, 10);
#endif
	return convertString(szValue, 0, bCommas);
} 
*/

/** 
 * Converts an unsigned int64 to a string.
 * @param l - The unsigned int64 to convert.
 * @param bCommas - true if you want commas in the string, else false (defaults to false).
 * @return - string value.
 */
/*
std::string getUnsignedLong64(unsigned __int64 l, bool bCommas)
{       
	char szValue[64];
#ifdef WIN32
	_ui64toa_s(l, szValue, 64, 10);
#else
	_ui64toa(l, szValue, 10);
#endif
	return convertString(szValue, 0, bCommas);
}         
*/
/** 
 * Converts an unsigned int to a string.
 * @param i - The unsigned int to convert.
 * @param bCommas - true if you want commas in the string, else false (defaults to false).
 * @return - string value.
 */
std::string getUnsignedInt(unsigned int i, bool bCommas)
{       
	char szValue[64];
#ifdef WIN32
	sprintf_s(szValue, 64, "%u", i);
#else
	sprintf(szValue, "%u", i);
#endif
	if (!bCommas) {return szValue;}
	return formatString(szValue, 0, bCommas);
}         

/** 
 * Converts a double to a string.
 * @param d - The double to convert.
 * @return - string value.
 */ 
std::string getDouble(double d)
{   
	std::string str = ToStr(d);
	if (str == "-inf") return str;
	if (str == "inf") return str;
	if (str == "nan") return str;

	char szValue[64];
#ifdef WIN32
	sprintf_s(szValue, 64, "%.16g", d);
#else
	sprintf(szValue, "%.16g", d);
#endif
	return szValue;
}  

/** 
 * Converts a double to a string.
 * @param d - The double to convert.
 * @param iDecimalPlaces - The number of decimal places (double will be rounded).
 * @param bCommas - true if you want commas in the string, else false (defaults to false).
 * @return - string value.
 */ 
std::string getDouble(double d, int iDecimalPlaces, bool bCommas)
{   
	d = roundDouble(d, iDecimalPlaces);
	std::string str = ToStr(d);
	if (str == "-inf") return str;
	if (str == "inf") return str;
	if (str == "nan") return str;

	char szValue[64];
#ifdef WIN32
	sprintf_s(szValue, 64, "%.16g", d);
#else
	sprintf(szValue, "%.16g", d);
#endif
	if (d != d) {return "nan";}
	return formatString(szValue, iDecimalPlaces, bCommas);
}  

/** 
 * Formats a number string
 * @param szValue - The const char* to convert.
 * @param iDecimalPlaces - The number of decimal places.
 * @param bCommas - true if you want commas in the string, else false (defaults to false).
 * @return - string value.
 */  
std::string formatString(const char* szValue, int iDecimalPlaces, bool bCommas)
{   
	char szBuffer[64];
	int iValue, iBuffer, iComma;
	int iCount, iMod;
	int i, iLeftofDecimal;
		
	iValue = 0, iBuffer = 0, iComma = 0;
	iCount = (int)(strlen(szValue)); 
	iLeftofDecimal = 0;
	for (i = 0; (i < iCount); i++)
	{
		if ((szValue[i] == 'e') || (szValue[i] == 'E'))
		{
			// The number is probably in scientific notation, return as is.
			return szValue;
		}
	}		 
	for (i = 0; (i < iCount); i++)
	{
		if (szValue[i] == '.') {break;}
		else {iLeftofDecimal++;}
	}		 
	if (szValue[iValue] == '-')
	{    
		iMod = (iLeftofDecimal - 1) % 3;
		szBuffer[iBuffer] = '-'; 
		iValue++;
		iBuffer++;
	}                      
	else {iMod = iLeftofDecimal % 3;}	
	for (iComma = 0; (iComma < iMod); iComma++)
	{
		szBuffer[iBuffer] = szValue[iValue];
		iBuffer++;
		iValue++;
	}
	if ((iMod > 0) && (iValue < iLeftofDecimal))
	{
		if (bCommas)
		{
			szBuffer[iBuffer] = ',';
			iBuffer++;
		}
	}	
	iComma = 0;
	bool bDecimal = false;
	int iNumberPlaces = 0;	
	for (; (iValue < iCount); iValue++)
	{   
		if (szValue[iValue] == '.') {bDecimal = true;} 
		if ((iComma == 3) && (!bDecimal))
		{ 
			iComma = 0;
			if (bCommas)
			{
				szBuffer[iBuffer] = ',';
				iBuffer++;
			}
		} 
		if ((bDecimal) && (iDecimalPlaces == 0)) {break;}
		szBuffer[iBuffer] = szValue[iValue]; 	 
		iBuffer++; 
		iComma++; 
		if (bDecimal)
		{
			iNumberPlaces++;
			if (iNumberPlaces > iDecimalPlaces) {break;}
		}	
	}       
	szBuffer[iBuffer] = '\0';
	std::string str = szBuffer;
	int iFindIndex = (int)str.find(".");
	if (iFindIndex != -1)
	{
		for (int iIndex = ((int)str.length() - 1); (iIndex >= iFindIndex); iIndex--)
		{
			if ((szBuffer[iIndex] == '0') || (szBuffer[iIndex] == '.')) {szBuffer[iIndex] = 0;} else {break;}
		}
	}
	if ((szBuffer[0] == '-') && (szBuffer[1] == '0') && (szBuffer[2] == 0))
	{
		szBuffer[0] = '0';
		szBuffer[1] = 0;
	}
	return szBuffer;	
}        

/** 
 * Converts Month int to Month string.
 * @param iMonth - The Month number.
 * @return - string value.
 */      
std::string getMonth(int iMonth)
{
	std::string str;
	std::string strMonths = "JanFebMarAprMayJunJulAugSepOctNovDec";
    
    if ((iMonth >= 1) && (iMonth <= 12))
    {
		iMonth -= 1; 
		iMonth = iMonth * 3;
   	 
		str = strMonths.substr(iMonth, 3); 
    }
    	
	return	str;
}    

/** 
 * Converts Month string to Month int.
 * @param strMonth - The Month string.
 * @return - int value.
 */ 
int getMonth(const std::string& strMonth)
{
	int i; 
	std::string strMonths = "JanFebMarAprMayJunJulAugSepOctNovDec";
       
	if (strMonth == "") {i = 0;}
    else
    {	   
		i = (int)strMonths.find(strMonth);
   	 
		if (i == -1) {i = 0;}
		else {i = (i / 3) + 1;} 
	}
    
    return i;     
}  

/** 
 * Round the double and return it.
 * @param d - The double to round.
 * @param iDecimalPlaces - The number of decimal places to round to.
 * @return - rounded double value.
 */ 
double roundDouble(double val,int places)
{
  double v_int,v_frac;
  double place_10,v_int10,v_frac10;

  // split into parts.
  v_frac=modf(val,&v_int);

  // create our shift
  place_10=pow(10.0,places);
  // shift the frac part up and truncate
  v_frac10=modf(v_frac*place_10,&v_int10);
  // std-c++ round says that it rounds away from zero.
  // apply the rounding of the last digit.
  if (v_frac10>=0.5) {
    v_int10=v_int10+1.0;
  }
  else if (v_frac10<=-0.5) {
    v_int10=v_int10-1.0;
  }
  // shift fract back down.
  v_frac=v_int10/place_10;

  // put back together.
  return (v_int+v_frac);
}


/** 
 * Check for special characters is a string using in a SQL WHERE xxx LIKE xxx clause.
 * @param str - The string to check.
 * @return - The formatted string.
 */ 
std::string likeString(const std::string& str)
{	
	char c;
	unsigned int i = 0;
	std::string strLike;

	for(i = 0; (i < str.length()); i++)
	{
		c = str[i];

		// Check for a single quote (ascii 39), put an extra quote if it is found
		if (c == 39) 
		{
			strLike += "'";	
			strLike += c;
		}

		// to avoid the error message 'FOR UPDATE clause allowed only for DECLARE CURSOR',
		// do not use the LIKE clause for search strings that contain parentheses
		// make partial filter using right and left side around parentheses
		else if ((c == '(') || (c == ')'))
		{
			strLike += '_';
		}
		
		// LIKE uses wildcards:  % = any number of chars, _ = one char, [ = start of literal block
		// First copy string, enclosing wildcards in square brackets, treating them as literals
		// Also check for a single quote (ascii 39), put an extra quote if it is found
		else if ((c == '%') || (c == '_') || (c == '['))
		{
			strLike += '[';		
			strLike += c;
			strLike += ']';
		}
		else
		{
			strLike += c;
		}
	}

	return strLike;
}
